#define Uses_TApplication

#include "ParamsEditor.h"
#include "../Const.h"

#include <functional>

static unsigned long djb2(const char* s, std::size_t len) {
    unsigned long h = 5381;
    for (std::size_t i = 0; i < len; ++i) h = ((h << 5) + h) + static_cast<unsigned char>(s[i]);
    return h;
}

void ParamsEditor::handleEvent(TEvent& ev) {
    TEditor::handleEvent(ev);

    unsigned long h = djb2(buffer, bufLen);
    if (h != lastSizeHash) {
        lastSizeHash = h;
        if (owner)
            message(owner, evBroadcast, cmParamsChanged, this);
    }
}