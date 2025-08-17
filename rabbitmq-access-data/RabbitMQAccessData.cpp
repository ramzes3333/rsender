#include "RabbitMQAccessData.h"

#include <nlohmann/json.hpp>
#include <filesystem>
#include <fstream>

using json = nlohmann::json;
namespace fs = std::filesystem;

bool saveRabbitParams(const RabbitMQAccessData& d,
                      const std::string& filePath,
                      std::string* errorOut)
{
    try {
        fs::create_directories(fs::path(filePath).parent_path());
        json j{
                {"host",     d.host},
                {"port",     d.port},
                {"vhost",    d.vhost},
                {"username", d.username},
                {"password", d.password}
        };
        std::ofstream out(filePath);
        if (!out) { if (errorOut) *errorOut = "Cannot open file for write: " + filePath; return false; }
        out << j.dump(2) << '\n';
        return true;
    } catch (const std::exception& e) {
        if (errorOut) *errorOut = e.what();
        return false;
    }
}

std::optional<RabbitMQAccessData> loadRabbitParams(
                      const std::string& filePath,
                      std::string* errorOut)
{
    try {
        if (!fs::exists(filePath)) return std::nullopt;
        std::ifstream in(filePath);
        if (!in) { if (errorOut) *errorOut = "Cannot open file for read: " + filePath; return std::nullopt; }
        json j; in >> j;

        auto getS = [&](const char* k)->std::string {
            return j.contains(k) && j[k].is_string() ? j[k].get<std::string>() : std::string{};
        };

        RabbitMQAccessData data{
            getS("host"),
            getS("port"),
            getS("vhost"),
            getS("username"),
            getS("password")
        };
        return data;
    } catch (const std::exception& e) {
        if (errorOut) *errorOut = e.what();
        return std::nullopt;
    }
}
