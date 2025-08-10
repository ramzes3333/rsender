#ifndef UTILS_H
#define UTILS_H

#define Uses_TInputLine
#include <tvision/tv.h>

#include <string>
#include <set>

class Utils {
public:
    static std::set<std::string> extractTags(const std::string& text);
    static std::string trim(const std::string& str);
    static std::string generateUUID();
    static std::string getCurrentDateTime();
    static std::string getInputLineText(TInputLine* input);
};

#endif //UTILS_H