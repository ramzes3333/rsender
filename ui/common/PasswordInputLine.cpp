#include "PasswordInputLine.h"

#define Uses_TInputLine
#include <tvision/tv.h>
#include <cstring>
#include <string>

void PasswordInputLine::draw() {
    if (!data) {
        TInputLine::draw();
        return;
    }
    size_t len = std::strlen(data);
    std::string backup(data, len);

    std::string masked(len, '*');
    std::memcpy(data, masked.data(), len);
    data[len] = '\0';

    TInputLine::draw();

    std::memcpy(data, backup.data(), len);
    data[len] = '\0';
}
