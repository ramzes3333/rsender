#ifndef READONLYEDITOR_H
#define READONLYEDITOR_H

#define Uses_TEditor

#include <tvision/tv.h>

class ReadOnlyEditor : public TEditor {
public:
    using TEditor::TEditor;

    virtual void handleEvent( TEvent& event );
};

#endif //READONLYEDITOR_H
