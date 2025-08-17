#ifndef ENHANCEDEDITOR_H
#define ENHANCEDEDITOR_H

#define Uses_TEditor

#include <tvision/tv.h>

class EnhancedEditor : public TEditor {
public:
    using TEditor::TEditor;

    std::string getEditorText();
    void setEditorText(const std::string& s);
};

#endif //ENHANCEDEDITOR_H
