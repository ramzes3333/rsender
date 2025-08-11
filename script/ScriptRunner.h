#ifndef SCRIPTRUNNER_H
#define SCRIPTRUNNER_H

#include <functional>
#include <string>
#include <thread>
#include <atomic>
#include <sys/types.h>

class ScriptRunner {
public:
    using ProgressFn = std::function<void(int /*percent*/)>;
    using LineFn     = std::function<void(const std::string& /*line*/)>;
    using ExitFn     = std::function<void(int /*exitCode*/)>;

    ScriptRunner();
    ~ScriptRunner();

    void run(const std::string& scriptPath,
             ProgressFn onProgress,
             LineFn onLine = nullptr,
             ExitFn onExit = nullptr);
    void abort(bool force = false);
    bool isRunning() const;
    void setNeedle(std::string s);

private:
    static size_t countOccurrencesInString(const std::string& hay, const std::string& needle);
    static size_t countOccurrencesInFile(const std::string& path, const std::string& needle);
    static size_t consumeAndCount(std::string& acc, const std::string& needle);

    void pushProgress();
    void join();

    template <class Fn, class... Args>
    static void safeCall(Fn& fn, Args&&... args);

    static constexpr const char* kNeedle = "Command executed";

    std::thread runnerThread_;
    std::atomic<pid_t> pid_;
    std::atomic<bool> running_;
    std::atomic<size_t> currentHits_{0};
    size_t totalHits_;
    std::string needleOverride_;

    ProgressFn onProgress_;
    LineFn onLine_;
    ExitFn onExit_;
};

#endif //SCRIPTRUNNER_H
