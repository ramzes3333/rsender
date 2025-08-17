#ifndef MESSAGEEXTRACTDIALOG_H
#define MESSAGEEXTRACTDIALOG_H

#define Uses_TDialog
#define Uses_TButton

#include <tvision/tv.h>

#include <string>

#include "EnhancedEditor.h"
#include "../extractor/MessageExtractor.h"

constexpr ushort cmExtract = 0x5001;

class MessageExtractDialog : public TDialog {
public:
    MessageExtractDialog(const TRect& bounds);

    const LogExtractResult& result() const { return result_; }

    void setText(const std::string& txt);

private:
    void initUi();
    void handleEvent(TEvent& ev) override;

    EnhancedEditor* editor {nullptr};
    TButton* extractBtn {nullptr};
    TButton* cancelBtn {nullptr};
    LogExtractResult result_;
};

#endif //MESSAGEEXTRACTDIALOG_H
