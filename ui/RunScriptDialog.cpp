#include "RunScriptDialog.h"

#define Uses_TEvent
#define Uses_TProgram

#include <tvision/tv.h>
#include <complex>
#include <filesystem>

#include "common/ReadOnlyEditor.h"
#include "../Const.h"
#include "../util/Logger.h"
#include "../script/ScriptRunner.h"
#include <fstream>
#include <cmath>
#include <memory>
#include <sstream>

#include "TProgressBar.h"


std::string RunScriptDialog::readFile(const std::string& path) {
    std::ifstream in(path, std::ios::binary);
    std::ostringstream ss;
    ss << in.rdbuf();
    return ss.str();
}

std::string RunScriptDialog::baseName(const std::string& path) {
    auto pos = path.find_last_of("/\\");
    return pos == std::string::npos ? path : path.substr(pos + 1);
}

RunScriptDialog::RunScriptDialog(const TRect& bounds, const std::string& scriptPath)
    : TWindowInit(&RunScriptDialog::initFrame),
      TDialog(bounds, "Run script"),
      scriptPath_(scriptPath)
{
    palette = dpGrayDialog;
    options |= ofCentered | ofSelectable;
    growMode = gfGrowHiX | gfGrowHiY;
    initUi();
}

void RunScriptDialog::initUi() {
    std::string q = "Are you sure you want to run script: " + baseName(scriptPath_) + "?";
    insert(new TStaticText(TRect(2, 1, size.x-2, 2), q.c_str()));

    TRect ed = TRect(2, 3, size.x-3, size.y-7);
    auto *v = new TScrollBar(TRect(ed.b.x, ed.a.y, ed.b.x+1, ed.b.y));
    auto *h = new TScrollBar(TRect(ed.a.x, ed.b.y, ed.b.x, ed.b.y+1));
    insert(v);
    insert(h);

    editor = new ReadOnlyEditor(ed, h, v, nullptr, 0x10000);
    editor->options |= ofFramed;
    insert(editor);

    pb = new TProgressBar(TRect(ed.a.x, ed.b.y+2, ed.b.x+1, ed.b.y+3), 0);
    insert(pb);

    finished = new TStaticText(TRect(size.x-32, size.y-3, size.x-24, size.y-2), "Finished");
    finished->hide();
    insert(finished);

    aborted = new TStaticText(TRect(size.x-32, size.y-3, size.x-24, size.y-2), "Aborted");
    aborted->hide();
    insert(aborted);

    std::string content = readFile(scriptPath_);
    if (!content.empty())
        editor->insertText(content.data(), (uint)content.size(), false);

    runTButton = new TButton(TRect(size.x-22, size.y-3, size.x-12, size.y-1), "~R~un", cmRunScriptNow, bfNormal);
    insert(runTButton);
    cancelTButton = new TButton(TRect(size.x-11, size.y-3, size.x-2,  size.y-1), "Cancel", cmCancel, 0);
    insert(cancelTButton);
    abortTButton = new TButton(TRect(size.x-11, size.y-3, size.x-2,  size.y-1), "Abort", cmAbort, 0);
    abortTButton->hide();
    insert(abortTButton);
}

void RunScriptDialog::handleEvent(TEvent& event) {
    if (event.what == evCommand) {
        /*if (event.message.command == cmRunScriptNow) {
            Logger::log("Running script: " + baseName(scriptPath_));
            std::unique_ptr<ScriptRunner> runner = std::make_unique<ScriptRunner>();

            runner->run(
                scriptPath_,
                /* onProgress = #1# [this](int pct) {
                    Logger::log("Running script (onProgress): " + std::to_string(pct) + "%");
                },
                /* onLine = #1# [this](const std::string& line) {
                    Logger::log("Running script (onLine): " + line);
                },
                /* onExit = #1# [this](int code) {
                    Logger::log("Running script (onExit): " + code);
                }
            );
        }*/
        switch (event.message.command) {
            case cmAbort: {
                runner->abort();
                cancelTButton->show();
                abortTButton->hide();
                aborted->show();
                return;
            }
            case cmRunScriptNow: {
                clearEvent(event);
                /*editor->setSelect(0, editor->bufLen, false);
                editor->deleteSelect();*/
                finished->hide();
                aborted->hide();
                cancelTButton->hide();
                abortTButton->show();
                runTButton->setState(sfDisabled, True);
                pb->setProgress(0);

                runner = std::make_unique<ScriptRunner>();
                runner->run(scriptPath_,
                    // onProgress
                    [this](int pct) {
                        Logger::log("Running script (onProgress): " + std::to_string(pct) + "%");
                        //static std::atomic<int> last{-1};
                        //int prev = last.exchange(pct);
                        //if (pct != prev) {
                            TEvent e{};
                            e.what = evCommand;
                            e.message.command = cmUpdateProgress;
                            e.message.infoLong = pct;
                            TProgram::application->putEvent(e);
                        //}
                    },
                    // onLine
                    [this](const std::string &line) {
                        Logger::log("Running script (onLine): " + line);
                        TEvent e{};
                        e.what = evCommand;
                        e.message.command = cmAppendLog;
                        e.message.infoPtr = (void*) strdup(line.c_str());
                        TProgram::application->putEvent(e);
                    },
                    // onExit
                    [this](int code) {
                        Logger::log("Running script (onExit): " + code);
                        TEvent e{};
                        e.what = evCommand;
                        e.message.command = cmScriptFinished;
                        e.message.infoLong = code;
                        TProgram::application->putEvent(e);
                    }
                );
                return;
            }
            case cmUpdateProgress: {
                Logger::log("Running script, received event: updateProgress");
                pb->setProgress((int) event.message.infoLong);
                clearEvent(event);
                return;
            }
            case cmAppendLog: {
                Logger::log("Running script, received event: appendLog");
                char *p = (char*) event.message.infoPtr;
                if (p) {
                    /*Logger::log("Running script, inserting text: " + std::string(p) + " " + std::to_string(strlen(p)));
                    editor->insertText(p, strlen(p), false);*/
                    free(p);
                }
                clearEvent(event);
                return;
            }
            case cmScriptFinished: {
                Logger::log("Running script, received event: scriptFinished");
                int exitCode = (int) event.message.infoLong;
                Logger::log("Exit code: " + std::to_string(exitCode));
                if (exitCode != -1) {
                    pb->setProgress(100);
                }
                //runTButton->setState(sfDisabled, False);
                finished->show();
                cancelTButton->show();
                abortTButton->hide();
                clearEvent(event);
                return;
            }
        }
    }

    TDialog::handleEvent(event);
}
