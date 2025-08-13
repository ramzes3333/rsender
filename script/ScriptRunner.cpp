#include "ScriptRunner.h"

#include <atomic>
#include <cstdio>
#include <fcntl.h>
#include <functional>
#include <string>
#include <sys/types.h>
#include <sys/wait.h>
#include <thread>
#include <unistd.h>
#include <fstream>
#include <pty.h>
#include <poll.h>
#include <termios.h>
#include <filesystem>

#include "../util/Logger.h"
#include "RunWorkspace.h"

namespace fs = std::filesystem;

ScriptRunner::ScriptRunner() : pid_(-1), running_(false), totalHits_(0) {}

ScriptRunner::~ScriptRunner() { join(); }

void ScriptRunner::run(const std::string& scriptPath,
         ProgressFn onProgress,
         LineFn onLine,
         ExitFn onExit)
{
    if (running_) return;

    RunWorkspaceResult prep = prepareRunWorkspace(scriptPath);
    if (!prep.ok) {
        if (onExit) onExit(-1);
        if (onLine) onLine("[rsender] " + prep.error);
        return;
    }

    const std::string runDirAbs   = fs::absolute(prep.runDir).string();
    const std::string scriptAbs   = fs::absolute(prep.scriptDst).string();
    const std::string scriptBase  = fs::path(prep.scriptDst).filename().string();
    const std::string scriptRel   = "./" + scriptBase;

    totalHits_ = countOccurrencesInFile(scriptAbs, kNeedle); // make sure this actually finds the phrase
    currentHits_ = 0;
    onProgress_ = std::move(onProgress);
    onLine_     = std::move(onLine);
    onExit_     = std::move(onExit);

    running_ = true;

    runnerThread_ = std::thread([this, runDirAbs, scriptRel, scriptAbs]() {
        int mfd = -1;
        pid_t cpid = -1;

        // Create PTY and fork child attached to its slave
        cpid = forkpty(&mfd, nullptr, nullptr, nullptr);
        if (cpid < 0) {
            perror("forkpty");
            running_ = false;
            return;
        }

        if (cpid == 0) {
            if (::chdir(runDirAbs.c_str()) != 0) {
                perror("chdir");
                _exit(126);
            }

            // Optional safety: ensure the script is readable from the new CWD.
            if (::access(scriptRel.c_str(), R_OK) != 0) {
                // As a fallback, try the absolute path (in case chdir is fine but name mismatch).
                if (::access(scriptAbs.c_str(), R_OK) == 0) {
                    execl("/bin/bash", "bash", scriptAbs.c_str(), (char*)nullptr);
                    perror("execl(abs)");
                    _exit(127);
                }
                perror("access(script)");
                _exit(126);
            }

            // Exec relative path from inside run dir.
            execl("/bin/bash", "bash", scriptRel.c_str(), (char*)nullptr);
            perror("execl");
            _exit(127);
        }

        // Parent
        pid_ = cpid;

        // Nonblocking read with poll, so this thread never blocks the app
        fcntl(mfd, F_SETFL, fcntl(mfd, F_GETFL) | O_NONBLOCK);

        std::string acc;
        char buf[4096];

        for (;;) {
            struct pollfd pfd{ mfd, POLLIN, 0 };
            int pr = poll(&pfd, 1, 100); // 100 ms tick
            if (pr > 0 && (pfd.revents & POLLIN)) {
                ssize_t n = ::read(mfd, buf, sizeof(buf));
                if (n > 0) {
                    acc.append(buf, buf + n);

                    // Emit full lines
                    size_t pos = 0;
                    while (true) {
                        size_t nl = acc.find('\n', pos);
                        if (nl == std::string::npos) break;
                        std::string line = acc.substr(pos, nl - pos);
                        if (onLine_) onLine_(line);

                        // Count needle occurrences in this line
                        currentHits_ += countOccurrencesInString(line, kNeedle);

                        pos = nl + 1;
                    }
                    if (pos > 0) acc.erase(0, pos);

                    pushProgress();
                } else if (n == 0) {
                    // EOF
                    break;
                } else if (errno != EAGAIN && errno != EWOULDBLOCK) {
                    perror("read");
                    break;
                }
            }

            // You could also check for abort flags here, etc.
            int status = 0;
            pid_t r = waitpid(pid_, &status, WNOHANG);
            if (r == pid_) {
                // Drain anything left
                while (true) {
                    ssize_t n2 = ::read(mfd, buf, sizeof(buf));
                    if (n2 > 0) {
                        std::string tail(buf, buf + n2);
                        if (onLine_) onLine_(tail);
                        currentHits_ += countOccurrencesInString(tail, kNeedle);
                    } else break;
                }

                if (totalHits_ == 0) safeCall(onProgress_, 100); // unknown total → finish at 100
                safeCall(onExit_, WIFEXITED(status) ? WEXITSTATUS(status) : -1);
                close(mfd);
                running_ = false;
                return;
            }
        }

        // If we broke out of read loop, still waitpid to reap
        int status = 0;
        waitpid(pid_, &status, 0);
        if (totalHits_ == 0) safeCall(onProgress_, 100);
        safeCall(onExit_, WIFEXITED(status) ? WEXITSTATUS(status) : -1);
        close(mfd);
        running_ = false;
    });
}

void ScriptRunner::abort(bool force) {
    if (running_ && pid_ > 0) {
        pid_t pgid = getpgid(pid_);
        if (pgid > 0) {
            kill(-pgid, force ? SIGKILL : SIGTERM);
        } else {
            kill(pid_, force ? SIGKILL : SIGTERM);
        }
    }
}

bool ScriptRunner::isRunning() const { return running_; }

void ScriptRunner::setNeedle(std::string s) { needleOverride_ = std::move(s); }

void ScriptRunner::join() {
    if (runnerThread_.joinable())
        runnerThread_.join();
}

size_t ScriptRunner::countOccurrencesInString(const std::string& hay, const std::string& needle) {
    if (needle.empty()) return 0;
    size_t count = 0, pos = 0;
    while ((pos = hay.find(needle, pos)) != std::string::npos) {
        ++count;
        pos += needle.size();
    }
    return count;
}

size_t ScriptRunner::countOccurrencesInFile(const std::string& path, const std::string& needle) {
    std::ifstream in(path);
    if (!in.is_open()) return 0;
    std::string content((std::istreambuf_iterator<char>(in)), std::istreambuf_iterator<char>());
    return countOccurrencesInString(content, needle);
}

size_t ScriptRunner::consumeAndCount(std::string& acc, const std::string& needle) {
    if (needle.empty()) return 0;

    const size_t tailLen = needle.size() > 1 ? (needle.size() - 1) : 0;
    if (acc.size() <= tailLen) return 0;

    const size_t processLen = acc.size() - tailLen;
    size_t hits = 0;
    size_t pos = 0;
    while (pos < processLen) {
        size_t f = acc.find(needle, pos);
        if (f == std::string::npos || f >= processLen) break;
        ++hits;
        pos = f + needle.size();
    }
    acc.erase(0, processLen);
    return hits;
}

void ScriptRunner::pushProgress() {
    Logger::log("Push progress");
    if (!onProgress_) return;
    if (totalHits_ > 0) {
        int pct = int((currentHits_.load() * 100) / totalHits_);
        if (pct > 100) pct = 100;
        safeCall(onProgress_, pct);
    } else {
        safeCall(onProgress_, 0);
    }
}

template <class Fn, class... Args>
void ScriptRunner::safeCall(Fn& fn, Args&&... args) {
    if (fn) fn(std::forward<Args>(args)...);
}