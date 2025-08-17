#include "MessageExtractDialog.h"

#define Uses_TWindow
#define Uses_TLabel
#define Uses_TEditor
#define Uses_TScrollBar
#define Uses_TKeys
#define Uses_TButton
#define Uses_TDialog
#define Uses_TEvent
#define Uses_MsgBox

#include <tvision/tv.h>

#include <fstream>

MessageExtractDialog::MessageExtractDialog(const TRect& bounds)
    : TWindowInit(&MessageExtractDialog::initFrame),
      TDialog(bounds, "Extract event data from log")
{
    palette  = dpGrayDialog;
    options |= ofCentered | ofSelectable;
    growMode = gfGrowHiX | gfGrowHiY;
    initUi();
}

void MessageExtractDialog::initUi() {
    insert(new TStaticText(TRect(2, 1, size.x - 2, 2),
        "Paste your log snippet below, then press ~E~xtract."));

    TRect ed = TRect(2, 3, size.x - 3, size.y - 7);
    auto* v = new TScrollBar(TRect(ed.b.x, ed.a.y, ed.b.x + 1, ed.b.y));
    auto* h = new TScrollBar(TRect(ed.a.x, ed.b.y, ed.b.x, ed.b.y + 1));
    insert(v);
    insert(h);

    editor = new EnhancedEditor(ed, h, v, nullptr, 0x10000);
    editor->options |= ofFramed;
    insert(editor);

    extractBtn = new TButton(TRect(size.x - 22, size.y - 3, size.x - 12, size.y - 1),
                             "~E~xtract", cmExtract, bfDefault);
    insert(extractBtn);

    cancelBtn  = new TButton(TRect(size.x - 11, size.y - 3, size.x - 2,  size.y - 1),
                             "Cancel", cmCancel, 0);
    insert(cancelBtn);
}

void MessageExtractDialog::handleEvent(TEvent& ev) {
    TDialog::handleEvent(ev);

    if (ev.what == evCommand && ev.message.command == cmExtract) {
        std::string text = editor->getEditorText();

        ExtractOptions opt;
        LogExtractResult r = parseFirstEventFromLog(text, opt);
        if (!r.ok) {
            messageBox(mfError | mfOKButton, "Error", "%s", r.error.c_str());
            clearEvent(ev);
            return;
        }

        result_ = std::move(r);
        endModal(cmOK);
        clearEvent(ev);
    }
}
