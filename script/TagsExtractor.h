#ifndef TAGSEXTRACTOR_H
#define TAGSEXTRACTOR_H

#include <string>
#include <set>

class TagsExtractor {
public:
    TagsExtractor(const std::string& propertiesTemplate,
                    const std::string& payloadTemplate);

    std::string extractTags();

private:
    std::string propertiesTemplate;
    std::string payloadTemplate;

    const std::set<std::string> ReservedTags = {
        "RANDOM_UUID",
        "NOW"
    };
};

#endif //TAGSEXTRACTOR_H