#ifndef PARAMSCOUNTER_H
#define PARAMSCOUNTER_H

#include <string>
#include <cctype>

inline std::size_t countParameterLines(const std::string& text) {
    std::size_t count = 0;
    std::size_t i = 0, n = text.size();

    auto trim_view = [&](std::size_t b, std::size_t e) {
        while (b < e && std::isspace(static_cast<unsigned char>(text[b]))) ++b;
        while (e > b && std::isspace(static_cast<unsigned char>(text[e-1]))) --e;
        return std::pair<std::size_t, std::size_t>(b, e);
    };

    while (i < n) {
        std::size_t j = i;
        while (j < n && text[j] != '\n') ++j;
        auto [b, e] = trim_view(i, j);
        if (b < e)
            ++count;
        i = (j < n ? j + 1 : j);
    }
    return count;
}

#endif //PARAMSCOUNTER_H
