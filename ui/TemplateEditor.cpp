#define Uses_TApplication

#include "TemplateEditor.h"
#include "../Const.h"

void TemplateEditor::handleEvent(TEvent& event) {
    if (event.what == evKeyDown) {
        TEditor::handleEvent(event);
        message(owner, evBroadcast, cmUpdateTags, nullptr);
        return;
    }
    TEditor::handleEvent(event);
}