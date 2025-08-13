#include "RunWorkspace.h"

#include <filesystem>
#include <fstream>
#include <system_error>
#include <regex>

namespace fs = std::filesystem;

static std::string sanitizeHost(const std::string& host) {
    std::string s = host;
    for (char &c : s) if (c == '.') c = '_';
    return s;
}

static void makeExecutableBestEffort(const fs::path& p) {
    std::error_code ec;
    fs::permissions(p,
        fs::perms::owner_read  | fs::perms::owner_write | fs::perms::owner_exec |
        fs::perms::group_read  |                          fs::perms::group_exec |
        fs::perms::others_read |                          fs::perms::others_exec,
        fs::perm_options::add, ec);
    (void)ec;
}

static std::string extractHostFromScript(const fs::path& scriptPath, std::string& error) {
    std::ifstream in(scriptPath);
    if (!in) {
        error = "Failed to open script: " + scriptPath.string();
        return {};
    }
    std::string line;
    std::regex pattern(R"(RABBITMQ_HOST\s*=\s*(\S+))");
    std::smatch match;
    while (std::getline(in, line)) {
        if (std::regex_search(line, match, pattern) && match.size() > 1) {
            return match[1].str();
        }
    }
    error = "RABBITMQ_HOST not found in script: " + scriptPath.string();
    return {};
}

RunWorkspaceResult prepareRunWorkspace(const std::string& scriptPath)
{
    RunWorkspaceResult r;
    r.scriptSrc = scriptPath;

    std::error_code ec;

    fs::path src(scriptPath);
    if (!fs::exists(src, ec) || ec) {
        r.error = "Script not found: " + scriptPath;
        return r;
    }

    std::string parseErr;
    const std::string host = extractHostFromScript(src, parseErr);
    if (host.empty()) {
        r.error = parseErr;
        return r;
    }

    const fs::path scriptName = src.filename();
    const std::string stem = scriptName.stem().string();
    const fs::path runDir = fs::path("rsender-data") / "run" / stem;
    r.runDir = runDir.string();

    fs::create_directories(runDir, ec);
    if (ec) {
        r.error = "Failed to create run dir: " + runDir.string() + " (" + ec.message() + ")";
        return r;
    }

    const fs::path scriptDst = runDir / scriptName;
    r.scriptDst = scriptDst.string();
    fs::copy_file(src, scriptDst, fs::copy_options::overwrite_existing, ec);
    if (ec) {
        r.error = "Failed to copy script to run dir: " + ec.message();
        return r;
    }
    makeExecutableBestEffort(scriptDst);

    const fs::path helperSrc = fs::path("rsender-data") / "rabbitmqadmin-scripts" /
                               ("rabbitmqadmin_" + sanitizeHost(host) + ".py");
    r.helperSrc = helperSrc.string();

    if (!fs::exists(helperSrc, ec) || ec) {
        r.error = "Helper not found for host: " + helperSrc.string();
        return r;
    }

    const fs::path helperDst = runDir / "rabbitmqadmin.py";
    r.helperDst = helperDst.string();
    fs::copy_file(helperSrc, helperDst, fs::copy_options::overwrite_existing, ec);
    if (ec) {
        r.error = "Failed to copy helper: " + ec.message();
        return r;
    }
    makeExecutableBestEffort(helperDst);

    r.ok = true;
    return r;
}
