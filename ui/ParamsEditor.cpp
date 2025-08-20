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
        if (owner) {
            clearAllLineColors();
            message(owner, evBroadcast, cmParamsChanged, this);
        }
    }
}

void ParamsEditor::setLineColor(int line, TColorDesired fg) {
    lineColors[line] = fg;
    drawView();
}
void ParamsEditor::clearLineColor(int line) {
    lineColors.erase(line);
    drawView();
}
void ParamsEditor::clearAllLineColors() {
    lineColors.clear();
    drawView();
}

void ParamsEditor::draw() {
    // Keep base logic to sync drawPtr/drawLine with delta.y
    if (drawLine != delta.y) {
        drawPtr  = lineMove(drawPtr, delta.y - drawLine);
        drawLine = delta.y;
    }

    // Base color pair: normal+selected
    TAttrPair basePair = getColor(0x0201);
    // Selected attribute to preserve selection overlay
    const TColorAttr selAttr = getColor(2);

#ifndef __FLAT__
    TScreenCell b[maxLineLength];
#else
    TScreenCell *b = (TScreenCell*) alloca(sizeof(TScreenCell) * (delta.x + size.x));
#endif

    uint linePtr = drawPtr;
    int y = 0;
    int lineIdx = delta.y;
    int count = size.y;

    while (count-- > 0) {
        // 1) Let Turbo Vision build the line normally
        formatLine(b, linePtr, delta.x + size.x, basePair);

        // 2) If this line is marked, recolor cells that are NOT selected
        if (auto it = lineColors.find(lineIdx); it != lineColors.end()) {
            const TColorDesired color = it->second;
            const uint width = delta.x + size.x;
            for (uint i = 0; i < width; ++i) {
                if (b[i].attr != selAttr)
                    b[i].attr._fg = color.bitCast();
            }
        }

        // 3) Paint visible part
        writeBuf(0, y, size.x, 1, &b[delta.x]);

        // Next line
        linePtr = nextLine(linePtr);
        ++y;
        ++lineIdx;
    }
}