#ifndef RABBITMQADMINSCRIPTDOWNLOADER_H
#define RABBITMQADMINSCRIPTDOWNLOADER_H

#include <string>

struct DownloadResult {
    bool downloaded;
    std::string finalPath;
    std::string error;
};

DownloadResult ensureRabbitmqAdmin(const std::string& host, const std::string& port);

#endif //RABBITMQADMINSCRIPTDOWNLOADER_H
