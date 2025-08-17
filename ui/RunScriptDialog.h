#ifndef RUNSCRIPTDIALOG_H
#define RUNSCRIPTDIALOG_H

#define Uses_TDialog
#define Uses_TButton
#define Uses_TStaticText
#define Uses_TScrollBar
#define Uses_TEditor

#include <memory>
#include <tvision/tv.h>
#include <string>

#include "TProgressBar.h"

class ScriptRunner;

class RunScriptDialog : public TDialog {
public:
    RunScriptDialog(const TRect& bounds, const std::string& scriptPath);

    TProgressBar *pb;
    TButton *runTButton;
    TStaticText *finished;
    TStaticText *aborted;
    TButton *cancelTButton;
    TButton *abortTButton;

private:
    std::string scriptPath_;
    std::unique_ptr<ScriptRunner> runner;
    TEditor* editor = nullptr;

    static std::string readFile(const std::string& path);
    static std::string baseName(const std::string& path);
    void initUi();

    virtual void handleEvent( TEvent& event );
};

#endif //RUNSCRIPTDIALOG_H
