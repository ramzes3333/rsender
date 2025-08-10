#include "ScriptSaver.h"

#include <fstream>

#include "../util/Logger.h"

bool ScriptSaver::saveScriptToFile(const std::string& path, const std::string& content) {
    std::error_code ec;

    std::filesystem::path filePath(path);
    std::filesystem::path dir = filePath.parent_path();

    // Create directory if needed
    if (!dir.empty() && !std::filesystem::exists(dir)) {
        if (!std::filesystem::create_directories(dir, ec)) {
            Logger::log("Failed to create directory: " + dir.string() + " - " + ec.message());
            return false;
        }
    }

    // Write file content
    std::ofstream scriptFile(path);
    if (!scriptFile.is_open()) {
        Logger::log("Failed to open script file for writing: " + path);
        return false;
    }

    scriptFile << content;
    scriptFile.close();

    std::filesystem::permissions(
        path,
        std::filesystem::perms::owner_exec |
        std::filesystem::perms::group_exec |
        std::filesystem::perms::others_exec,
        std::filesystem::perm_options::add,
        ec
    );

    if (ec) {
        Logger::log("Failed to chmod +x on script: " + ec.message());
        return false;
    }

    Logger::log("Script saved and marked executable: " + path);
    return true;
}
