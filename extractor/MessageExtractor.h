#ifndef MESSAGEEXTRACTOR_H
#define MESSAGEEXTRACTOR_H

#include <string>
#include <nlohmann/json.hpp>

struct LogExtractResult {
    bool ok = false;
    std::string error;

    std::string routingKey;
    std::string receivedExchange;
    nlohmann::json propertiesJson;
    std::string payloadTemplate;
    std::string tagsLine;
    std::string valuesLine;
};

struct ExtractOptions {
    std::vector<std::string> propertyWhitelist = {
        "messageId", "contentType", "deliveryMode"
    };
    bool includeHeaders = true;
};

static const std::unordered_map<std::string, std::string> kPropRename = {
    {"messageId",   "message_id"},
    {"contentType", "content_type"},
    {"deliveryMode","delivery_mode"}
};

LogExtractResult parseFirstEventFromLog(const std::string& logBlob,
                                        const ExtractOptions& opt /* = {} */);

#endif //MESSAGEEXTRACTOR_H
