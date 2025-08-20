#include "JsonValidator.h"

#include <string>
#include <nlohmann/json.hpp>

static std::string replaceTemplateTagsForJson(const std::string &tpl) {
    std::string out;
    out.reserve(tpl.size());

    bool inString = false;
    bool escaped = false;

    for (size_t i = 0; i < tpl.size(); ) {
        char c = tpl[i];

        if (!inString && c == '{' && i + 1 < tpl.size() && tpl[i+1] == '{') {
            size_t j = i + 2;
            while (j + 1 < tpl.size() && !(tpl[j] == '}' && tpl[j+1] == '}'))
                ++j;
            if (j + 1 >= tpl.size()) {
                out.append(tpl.substr(i));
                break;
            }

            i = j + 2;

            out += "\"X\"";
            continue;
        }

        if (inString && c == '{' && i + 1 < tpl.size() && tpl[i+1] == '{') {
            size_t j = i + 2;
            while (j + 1 < tpl.size() && !(tpl[j] == '}' && tpl[j+1] == '}'))
                ++j;
            if (j + 1 >= tpl.size()) {
                out.append(tpl.substr(i));
                break;
            }
            i = j + 2;

            out += "X";
            continue;
        }

        if (c == '\"') {
            if (!escaped) inString = !inString;
            out.push_back(c);
            escaped = false;
            ++i;
            continue;
        }

        escaped = (!escaped && c == '\\');
        if (!escaped && c != '\\') escaped = false;

        out.push_back(c);
        ++i;
    }

    return out;
}

static void posToLineCol(const std::string &s, size_t pos, size_t &line, size_t &col) {
    line = 1; col = 1;
    for (size_t i = 0; i < pos && i < s.size(); ++i) {
        if (s[i] == '\n') { ++line; col = 1; }
        else              { ++col; }
    }
}

static std::string makeSnippet(const std::string &s, size_t pos, size_t span = 50) {
    if (pos > s.size()) pos = s.size();
    size_t start = (pos > span ? pos - span : 0);
    size_t end   = std::min(s.size(), pos + span);
    return s.substr(start, end - start);
}

JsonValidationResult validateTemplateJson(const std::string &templateJson) {
    JsonValidationResult vr;

    std::string replaced = replaceTemplateTagsForJson(templateJson);

    try {
        auto js = nlohmann::json::parse(replaced);
        (void)js;
        vr.ok = true;
        return vr;
    } catch (const nlohmann::json::parse_error &e) {
        vr.ok = false;
        vr.message = e.what();
        vr.errorPos = e.byte; // 1-based byte index in input
        size_t pos0 = (vr.errorPos > 0 ? vr.errorPos - 1 : 0);
        posToLineCol(replaced, pos0, vr.line, vr.col);
        vr.snippet = makeSnippet(replaced, pos0);
        return vr;
    }
}
