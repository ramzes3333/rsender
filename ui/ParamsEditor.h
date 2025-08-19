#ifndef PARAMSEDITOR_H
#define PARAMSEDITOR_H

#define Uses_TEvent

#include "common/EnhancedEditor.h"

class ParamsEditor : public EnhancedEditor {
public:
    using EnhancedEditor::EnhancedEditor;

    void handleEvent(TEvent& event) override;

private:
    unsigned long lastSizeHash = 0;
};

#endif //PARAMSEDITOR_H
