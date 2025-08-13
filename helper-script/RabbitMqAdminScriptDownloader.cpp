#include "RabbitMqAdminScriptDownloader.h"

#include <curl/curl.h>
#include <filesystem>
#include <fstream>
#include <cstdio>
#include <system_error>

namespace fs = std::filesystem;

static std::string sanitizeHost(const std::string& host) {
    std::string s = host;
    for (char &c : s) if (c == '.') c = '_';
    return s;
}

static size_t writeToFile(void* ptr, size_t size, size_t nmemb, void* stream) {
    std::ofstream* out = static_cast<std::ofstream*>(stream);
    out->write(static_cast<const char*>(ptr), static_cast<std::streamsize>(size * nmemb));
    return static_cast<size_t>(out->good() ? size * nmemb : 0);
}

DownloadResult ensureRabbitmqAdmin(const std::string& host, const std::string& port) {
    DownloadResult res{false, "", ""};

    const fs::path baseDir = fs::path("rsender-data") / "rabbitmqadmin-scripts";
    const std::string fileName = "rabbitmqadmin_" + sanitizeHost(host) + ".py";
    const fs::path finalPath = baseDir / fileName;
    res.finalPath = finalPath.string();

    std::error_code ec;
    if (fs::exists(finalPath, ec) && !ec) {
        return res;
    }

    fs::create_directories(baseDir, ec);
    if (ec) {
        res.error = "Failed to create directory: " + baseDir.string() + " (" + ec.message() + ")";
        return res;
    }

    const std::string url = "http://" + host + ":" + port + "/cli/rabbitmqadmin";
    const fs::path tmpPath = finalPath.string() + ".part";

    std::ofstream out(tmpPath, std::ios::binary);
    if (!out.is_open()) {
        res.error = "Failed to open temp file for writing: " + tmpPath.string();
        return res;
    }

    CURL* curl = curl_easy_init();
    if (!curl) {
        res.error = "curl_easy_init failed";
        return res;
    }

    curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
    curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);
    curl_easy_setopt(curl, CURLOPT_CONNECTTIMEOUT, 1L);
    curl_easy_setopt(curl, CURLOPT_TIMEOUT, 3L);
    curl_easy_setopt(curl, CURLOPT_FAILONERROR, 1L);
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, writeToFile);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &out);
    curl_easy_setopt(curl, CURLOPT_USERAGENT, "rsender/1.0");

    CURLcode code = curl_easy_perform(curl);
    long httpCode = 0;
    curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &httpCode);
    curl_easy_cleanup(curl);
    out.close();

    if (code != CURLE_OK) {
        std::remove(tmpPath.string().c_str());
        res.error = "Download failed: " + std::string(curl_easy_strerror(code)) +
                    " (HTTP " + std::to_string(httpCode) + ")";
        return res;
    }

    fs::permissions(tmpPath,
                    fs::perms::owner_read | fs::perms::owner_write | fs::perms::owner_exec |
                    fs::perms::group_read | fs::perms::group_exec |
                    fs::perms::others_read | fs::perms::others_exec,
                    fs::perm_options::add, ec);

    fs::rename(tmpPath, finalPath, ec);
    if (ec) {
        std::error_code ec2;
        fs::copy_file(tmpPath, finalPath, fs::copy_options::overwrite_existing, ec2);
        std::remove(tmpPath.string().c_str());
        if (ec2) {
            res.error = "Failed to finalize file: " + ec2.message();
            return res;
        }
    }

    res.downloaded = true;
    return res;
}
