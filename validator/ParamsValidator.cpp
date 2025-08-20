#include "ParamsValidator.h"

#include <string>
#include <vector>
#include <cctype>
#include <algorithm>

static std::string trim(std::string s) {
    auto issp = [](unsigned char c){ return std::isspace(c); };
    s.erase(s.begin(), std::find_if_not(s.begin(), s.end(), issp));
    s.erase(std::find_if_not(s.rbegin(), s.rend(), issp).base(), s.end());
    return s;
}

static std::vector<std::string> splitKeepEmpty(const std::string& s, char sep) {
    std::vector<std::string> out;
    std::string cur;
    for (char c : s) {
        if (c == sep) { out.push_back(cur); cur.clear(); }
        else          { cur.push_back(c); }
    }
    out.push_back(cur);
    return out;
}

static std::vector<std::string> splitLines(const std::string& s) {
    std::vector<std::string> lines;
    std::string cur;
    for (size_t i = 0; i < s.size(); ++i) {
        char c = s[i];
        if (c == '\r') {
            if (i + 1 < s.size() && s[i+1] == '\n') ++i; // CRLF
            lines.push_back(cur); cur.clear();
        } else if (c == '\n') {
            lines.push_back(cur); cur.clear();
        } else {
            cur.push_back(c);
        }
    }
    lines.push_back(cur);
    return lines;
}

ValidationResult validateParams(const std::string& editorText,
                                    const std::string& tagsRaw,
                                    bool skipBlankLines) {
    ValidationResult res;

    const auto tags = splitKeepEmpty(tagsRaw, '|');
    const size_t requiredFields = tags.size();

    if (requiredFields == 0) {
        res.ok = false;
        res.message = "No tags provided.";
        return res;
    }

    const auto lines = splitLines(editorText);

    size_t lineNo = 0;
    for (const auto& rawLine : lines) {
        ++lineNo;
        const std::string line = trim(rawLine);

        if (skipBlankLines && line.empty()) continue;

        auto fields = splitKeepEmpty(line, '|');

        if (fields.size() != requiredFields) {
            res.ok = false;
            ++res.invalidCount;
            res.invalidLines.push_back(lineNo);

            if (res.firstInvalidLine == 0) {
                res.firstInvalidLine = lineNo;
                res.firstInvalidContent = rawLine;
                res.message = "Field count mismatch";
            }
            continue;
        }

        bool anyEmpty = false;
        for (auto& f : fields) {
            if (trim(f).empty()) { anyEmpty = true; break; }
        }
        if (anyEmpty) {
            res.ok = false;
            ++res.invalidCount;
            res.invalidLines.push_back(lineNo);
            if (res.firstInvalidLine == 0) {
                res.firstInvalidLine = lineNo;
                res.firstInvalidContent = rawLine;
                res.message = "Empty value";
            }
        }
    }

    return res;
}