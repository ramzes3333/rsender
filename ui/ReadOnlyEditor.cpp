#include "ReadOnlyEditor.h"

#define Uses_TKeys
#define Uses_TEvent

#include <tvision/tv.h>

void ReadOnlyEditor::handleEvent(TEvent &ev) {
    if (ev.what == evKeyDown) {
        ushort key = ev.keyDown.keyCode;
        if (key == kbBack || key == kbDel || key == kbIns || key == kbCtrlY ||
            (ev.keyDown.charScan.charCode >= 32 && ev.keyDown.charScan.charCode < 127)) {
            clearEvent(ev);
            return;
            }
    }
    TEditor::handleEvent(ev);
}