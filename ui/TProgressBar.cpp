#include "TProgressBar.h"

#include <algorithm>
#include <cstring>

TProgressBar::TProgressBar(const TRect& bounds, int initPercent)
        : TView(bounds),
            //percent(std::ranges::clamp(initPercent, 0, 100))
            percent(std::clamp(initPercent, 0, 100))
{
    growMode = gfGrowHiX;
    options |= ofSelectable;
}

void TProgressBar::setProgress(int newPercent) {
    newPercent = std::clamp(newPercent, 0, 100);
    if (newPercent != percent) {
        percent = newPercent;
        drawView();
    }
}

void TProgressBar::draw() {
    TDrawBuffer buf;
    const int w = size.x;

    // Palette indices:
    // 1 = filled segment background color (used for spaces)
    // 2 = empty  segment background color (used for spaces)
    // 3 = text color over FILLED background (fg tuned for contrast)
    // 4 = text color over EMPTY  background (fg tuned for contrast)
    const ushort filledBg   = getColor(1);
    const ushort emptyBg    = getColor(2);
    const ushort textFilled = getColor(3);
    const ushort textEmpty  = getColor(4);

    // Fill amount
    const int fill = (percent * w) / 100;

    // Paint bar background first (spaces with bg colors)
    if (fill > 0)
        buf.moveChar(0, ' ', filledBg, fill);
    if (fill < w)
        buf.moveChar(fill, ' ', emptyBg, w - fill);

    // Centered text
    char txt[8];
    std::snprintf(txt, sizeof(txt), "%3d%%", percent);
    const int len = (int) std::strlen(txt);
    int x0 = (w - len) / 2;
    if (x0 < 0) x0 = 0;                 // tiny bars safety
    const int maxChars = std::min(len, w - x0);

    // Overlay text char-by-char so bg matches underlying segment
    for (int i = 0; i < maxChars; ++i) {
        const int x = x0 + i;
        const ushort attr = (x < fill) ? textFilled : textEmpty;
        buf.moveChar(x, txt[i], attr, 1);
    }

    writeLine(0, 0, w, 1, buf);
}

static TPalette &palette() {
    static TColorAttr colors[] = {
        // 0: filled background (bg=blue). fg doesn't matter for spaces.
        TColorAttr(TColorBIOS(0x0), TColorBIOS(0x1)),

        // 1: empty background (bg=dark gray). fg doesn't matter for spaces.
        TColorAttr(TColorBIOS(0x0), TColorBIOS(0x8)),

        // 2: text over FILLED area (fg=white, bg=blue)
        TColorAttr(TColorBIOS(0xF), TColorBIOS(0x1)),

        // 3: text over EMPTY area (fg=black, bg=dark gray)
        TColorAttr(TColorBIOS(0x0), TColorBIOS(0x8)),
    };
    static TPalette p(colors, (ushort)(sizeof(colors)/sizeof(colors[0])));
    return p;
}

TPalette &TProgressBar::getPalette() const {
    return palette();
}

TColorAttr TProgressBar::mapColor(uchar index) noexcept {
    return palette()[index];
}

