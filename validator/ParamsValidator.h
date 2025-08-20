#ifndef PARAMSVALIDATOR_H
#define PARAMSVALIDATOR_H

#include <string>
#include <vector>

struct ValidationResult {
    bool ok = true;
    size_t invalidCount = 0;
    size_t firstInvalidLine = 0;   // 1-based; 0 if none
    std::string firstInvalidContent;
    std::string message;

    std::vector<size_t> invalidLines;
};

ValidationResult validateParams(const std::string& editorText,
                                    const std::string& tagsRaw,
                                    bool skipBlankLines = true);

#endif //PARAMSVALIDATOR_H
