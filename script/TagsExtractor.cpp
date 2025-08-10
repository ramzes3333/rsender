#include "TagsExtractor.h"

#include <set>
#include <sstream>
#include <fstream>
#include <random>
#include <regex>

TagsExtractor::TagsExtractor(const std::string& propertiesTemplate,
                             const std::string& payloadTemplate)
    : propertiesTemplate(propertiesTemplate), payloadTemplate(payloadTemplate) {

}

std::string TagsExtractor::extractTags() {
    std::string text = propertiesTemplate + payloadTemplate;
    std::regex tagRegex(R"(\{\{([A-Za-z0-9_]+)\}\})");
    std::smatch match;
    std::set<std::string> uniqueTags;

    auto begin = text.cbegin();
    auto end = text.cend();

    while (std::regex_search(begin, end, match, tagRegex)) {
        std::string tagName = match[1];
        if (ReservedTags.find(tagName) == ReservedTags.end()) {
            uniqueTags.insert(tagName);
        }
        begin = match.suffix().first;
    }

    std::ostringstream joined;
    for (auto it = uniqueTags.begin(); it != uniqueTags.end(); ++it) {
        if (it != uniqueTags.begin())
            joined << "|";
        joined << *it;
    }

    return joined.str();
}
