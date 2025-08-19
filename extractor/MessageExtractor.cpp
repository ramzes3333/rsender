#include "MessageExtractor.h"

#include <regex>
#include <vector>
#include <algorithm>
#include <sstream>
#include <cctype>
#include <unordered_set>

using json = nlohmann::json;

// --- helpers -------------------------------------------------------------

static inline std::string trim(const std::string& s) {
    size_t b = 0, e = s.size();
    while (b < e && std::isspace(static_cast<unsigned char>(s[b]))) ++b;
    while (e > b && std::isspace(static_cast<unsigned char>(s[e-1]))) --e;
    return s.substr(b, e - b);
}
static inline bool isInteger(const std::string& s) {
    if (s.empty()) return false;
    size_t i = (s[0] == '-' ? 1 : 0);
    if (i == s.size()) return false;
    for (; i < s.size(); ++i) if (!std::isdigit(static_cast<unsigned char>(s[i]))) return false;
    return true;
}
static inline std::string toUpper(std::string s) {
    for (char &c : s) c = static_cast<char>(std::toupper(static_cast<unsigned char>(c)));
    return s;
}

static std::vector<std::string> splitTopLevelCommaSpace(const std::string& s) {
    std::vector<std::string> out;
    std::string cur;
    int depth = 0;
    for (size_t i = 0; i < s.size(); ++i) {
        char c = s[i];
        if (c == '{') depth++;
        if (c == '}') depth = std::max(0, depth - 1);

        if (depth == 0 && i + 1 < s.size() && c == ',' && s[i+1] == ' ') {
            out.push_back(trim(cur));
            cur.clear();
            ++i;
            continue;
        }
        cur.push_back(c);
    }
    if (!cur.empty()) out.push_back(trim(cur));
    return out;
}

static json parsePropertiesBlock(const std::string& block, std::string& errorOut) {
    json props = json::object();
    json headers = json::object();

    std::string rest = block;
    size_t hpos = rest.find("headers=");
    if (hpos != std::string::npos) {
        size_t brace = rest.find('{', hpos);
        size_t end   = rest.find('}', brace == std::string::npos ? std::string::npos : brace+1);
        if (brace != std::string::npos && end != std::string::npos) {
            std::string hinner = rest.substr(brace+1, end - (brace+1));
            size_t after = end + 1;
            if (after + 1 < rest.size() && rest[after] == ',' && rest[after+1] == ' ')
                after += 2;
            rest = trim(rest.substr(0, hpos) + rest.substr(after));

            for (const auto& kv : splitTopLevelCommaSpace(hinner)) {
                auto eq = kv.find('=');
                if (eq == std::string::npos) continue;
                std::string k = trim(kv.substr(0, eq));
                std::string v = trim(kv.substr(eq+1));
                headers[k] = v;
            }
            props["headers"] = headers;
        }
    }

    for (const auto& kv : splitTopLevelCommaSpace(rest)) {
        auto eq = kv.find('=');
        if (eq == std::string::npos) continue;
        std::string k = trim(kv.substr(0, eq));
        std::string v = trim(kv.substr(eq+1));

        if (k == "deliveryMode") {
            if (v == "PERSISTENT") { props[k] = 2; continue; }
            if (isInteger(v))      { props[k] = std::stoi(v); continue; }
            props[k] = v;
            continue;
        }

        if (isInteger(v))             props[k] = std::stoi(v);
        else if (v == "null")         props[k] = nullptr;
        else if (v == "true")         props[k] = true;
        else if (v == "false")        props[k] = false;
        else                          props[k] = v;
    }

    return props;
}

static std::string buildPayloadTemplateAndCollect(
    const json& bodyObj,
    std::vector<std::string>& tagsOut,
    std::vector<std::string>& valuesOut,
    const ExtractOptions& opt)
{
    if (!bodyObj.is_object())
        return bodyObj.dump();

    std::ostringstream oss;
    oss << "{";
    bool first = true;

    for (auto it = bodyObj.begin(); it != bodyObj.end(); ++it) {
        const std::string key = it.key();
        const std::string TAG = toUpper(key);

        const bool isReservedPayloadField =
            opt.applyReservedTags &&
            kReservedPayloadMap.find(key) != kReservedPayloadMap.end();

        if (!first) oss << ",";
        first = false;
        oss << "\n  " << json(key).dump() << ": ";

        if (isReservedPayloadField) {
            const std::string& reserved = kReservedPayloadMap.at(key);
            if (it.value().is_string() || it.value().is_object() || it.value().is_array()) {
                oss << "\"{{" << reserved << "}}\"";
            } else {
                oss << "{{" << reserved << "}}";
            }
            continue;
        }

        if (it.value().is_string()) {
            valuesOut.push_back(it.value().get<std::string>());
        } else if (it.value().is_number_integer() || it.value().is_number_unsigned()) {
            valuesOut.push_back(std::to_string(it.value().get<long long>()));
        } else if (it.value().is_number_float()) {
            valuesOut.push_back(std::to_string(it.value().get<double>()));
        } else if (it.value().is_boolean()) {
            valuesOut.push_back(it.value().get<bool>() ? "true" : "false");
        } else if (it.value().is_null()) {
            valuesOut.push_back("null");
        } else {
            valuesOut.push_back(it.value().dump());
        }

        tagsOut.push_back(TAG);

        if (it.value().is_string()) {
            oss << "\"{{" << TAG << "}}\"";
        } else if (it.value().is_number() || it.value().is_boolean() || it.value().is_null()) {
            oss << "{{" << TAG << "}}";
        } else {
            oss << "\"{{" << TAG << "}}\"";
        }
    }
    if (!first) oss << "\n";
    oss << "}";

    return oss.str();
}

static nlohmann::json filterAndMapProperties(const nlohmann::json& props,
                                             const ExtractOptions& opt)
{
    using json = nlohmann::json;
    json out = json::object();

    if (opt.includeHeaders && props.contains("headers"))
        out["headers"] = props["headers"];

    std::unordered_set<std::string> allow(opt.propertyWhitelist.begin(), opt.propertyWhitelist.end());

    for (const auto& k : opt.propertyWhitelist) {
        if (!props.contains(k)) continue;

        auto itRename = kPropRename.find(k);
        const std::string mapped = (itRename != kPropRename.end()) ? itRename->second : k;

        out[mapped] = props[k];
    }

    return out;
}

// --- main ---------------------------------------------------------------

LogExtractResult parseFirstEventFromLog(const std::string& logBlob,
                                        const ExtractOptions& opt) {
    LogExtractResult R;

    // 1) Capture Body JSON + MessageProperties block.
    static const std::regex reBodyProps(
        R"(Body:\s*['"]([\s\S]*?)['"]\s*MessageProperties\s*\[([\s\S]*?)\])",
        std::regex::ECMAScript);

    std::smatch m;
    if (!std::regex_search(logBlob, m, reBodyProps) || m.size() < 3) {
        R.error = "No Body + MessageProperties block found.";
        return R;
    }

    const std::string bodyRaw  = m[1].str();
    const std::string propsRaw = m[2].str();

    // 2) Parse Body JSON
    json bodyObj;
    try {
        bodyObj = json::parse(bodyRaw);
    } catch (const std::exception& e) {
        R.error = std::string("Failed to parse Body JSON: ") + e.what();
        return R;
    }

    // 3) Parse properties block
    std::string perr;
    json props = parsePropertiesBlock(propsRaw, perr);
    if (!perr.empty()) {
        R.error = "Failed to parse properties: " + perr;
        return R;
    }
    R.propertiesJson = props;

    // 4) Determine routingKey:
    //    Prefer 'receivedRoutingKey' from props (listener style),
    //    else fallback to 'routingKey: ...' after the props block (sender style).
    if (props.contains("receivedRoutingKey") && props["receivedRoutingKey"].is_string()) {
        R.routingKey = props["receivedRoutingKey"].get<std::string>();
    } else {
        // Search after the matched block end to avoid accidental earlier matches.
        const size_t tailPos = static_cast<size_t>(m.position(0) + m.length(0));
        const std::string tail = tailPos < logBlob.size() ? logBlob.substr(tailPos) : std::string{};
        std::smatch km;
        static const std::regex reRouting(R"(routingKey:\s*([^\s]+))");
        if (std::regex_search(tail, km, reRouting) && km.size() >= 2) {
            R.routingKey = km[1].str();
        } else {
            R.error = "routingKey not found (neither 'receivedRoutingKey' in properties nor 'routingKey:' in tail).";
            return R;
        }
    }

    // 5) Optional: receivedExchange (only present in listener style)
    if (props.contains("receivedExchange") && props["receivedExchange"].is_string()) {
        R.receivedExchange = props["receivedExchange"].get<std::string>();
    }

    R.propertiesJson = filterAndMapProperties(props, opt);

    if (opt.applyReservedTags) {
        if (R.propertiesJson.contains("message_id")) {
            R.propertiesJson["message_id"] = std::string("{{") + kReservedPropMessageId + "}}";
        }
    }

    // 6) Build payload template + collect tags/values
    std::vector<std::string> tags;
    std::vector<std::string> values;
    R.payloadTemplate = buildPayloadTemplateAndCollect(bodyObj, tags, values, opt);

    std::sort(tags.begin(), tags.end());
    tags.erase(std::unique(tags.begin(), tags.end()), tags.end());

    std::sort(values.begin(), values.end());
    values.erase(std::unique(values.begin(), values.end()), values.end());

    // 7) Join lines
    {
        std::ostringstream oss;
        for (size_t i = 0; i < tags.size(); ++i) { if (i) oss << '|'; oss << tags[i]; }
        R.tagsLine = oss.str();
    }
    {
        std::ostringstream oss;
        for (size_t i = 0; i < values.size(); ++i) { if (i) oss << '|'; oss << values[i]; }
        R.valuesLine = oss.str();
    }

    R.ok = true;
    return R;
}
