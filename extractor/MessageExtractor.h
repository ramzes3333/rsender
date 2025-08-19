#ifndef MESSAGEEXTRACTOR_H
#define MESSAGEEXTRACTOR_H

#include <set>
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
    bool applyReservedTags = true;
};

static const std::unordered_map<std::string, std::string> kPropRename = {
    {"messageId",   "message_id"},
    {"contentType", "content_type"},
    {"deliveryMode","delivery_mode"}
};

static const std::set<std::string> kReservedTags = {
    "RANDOM_UUID",
    "NOW"
};

static const std::unordered_map<std::string, std::string> kReservedPayloadMap = {
    {"dataUtworzeniaEventu", "NOW"},
};

static const std::string kReservedPropMessageId = "RANDOM_UUID";

LogExtractResult parseFirstEventFromLog(const std::string& logBlob,
                                        const ExtractOptions& opt /* = {} */);

#endif //MESSAGEEXTRACTOR_H
