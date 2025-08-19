#ifndef HELPDIALOG_H
#define HELPDIALOG_H

#define Uses_TDialog
#define Uses_TEditor
#define Uses_TScrollBar
#define Uses_TButton
#define Uses_TStaticText
#include <tvision/tv.h>

#include "common/ReadOnlyEditor.h"

class HelpDialog : public TDialog {
public:
    explicit HelpDialog(const TRect& bounds);

private:
    void initUi();
    void populateHelp();

    ReadOnlyEditor* editor {nullptr};
    TButton* okBtn {nullptr};
};

#endif //HELPDIALOG_H
