#ifndef PARAMSEDITOR_H
#define PARAMSEDITOR_H

#define Uses_TEvent

#include <unordered_map>

#include "common/EnhancedEditor.h"

class ParamsEditor : public EnhancedEditor {
public:
    using EnhancedEditor::EnhancedEditor;

    void handleEvent(TEvent& event) override;
    void setLineColor(int line, TColorDesired fg);
    void clearLineColor(int line);
    void clearAllLineColors();

    virtual void draw();

private:
    unsigned long lastSizeHash = 0;

protected:
    std::unordered_map<size_t, TColorDesired> lineColors;
};

#endif //PARAMSEDITOR_H
