#ifndef READONLYEDITOR_H
#define READONLYEDITOR_H

#define Uses_TEditor

#include <tvision/tv.h>

#include "EnhancedEditor.h"

class ReadOnlyEditor : public EnhancedEditor {
public:
    using EnhancedEditor::EnhancedEditor;

    virtual void handleEvent( TEvent& event );
};

#endif //READONLYEDITOR_H
