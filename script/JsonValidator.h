#ifndef JSONVALIDATOR_H
#define JSONVALIDATOR_H

#include <string>
#include <nlohmann/json.hpp>

struct JsonValidationResult {
    bool ok = true;
    std::string message;
    size_t errorPos = 0;
    size_t line = 0;
    size_t col = 0;
    std::string snippet;
};

JsonValidationResult validateTemplateJson(const std::string &templateJson);

#endif //JSONVALIDATOR_H
