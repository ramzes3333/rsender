#ifndef TEMPLATEEDITOR_H
#define TEMPLATEEDITOR_H

#define Uses_TEvent

#include "common/EnhancedEditor.h"

class TemplateEditor : public EnhancedEditor {
public:
    using EnhancedEditor::EnhancedEditor;

    void handleEvent(TEvent& event) override;
};

#endif //TEMPLATEEDITOR_H
