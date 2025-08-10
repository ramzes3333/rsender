#define Uses_TApplication
#define Uses_TDeskTop
#define Uses_TProgram
#define Uses_TProgInit
#define Uses_TMenuBar
#define Uses_TMenu
#define Uses_TMenuItem
#define Uses_TSubMenu
#define Uses_TStatusLine
#define Uses_TStatusDef
#define Uses_TStatusItem
#define Uses_TKeys
#define Uses_TWindow
#define Uses_TDialog

#include <tvision/tv.h>
#include "ui/PrepareScriptDialog.h"
#include "Const.h"

class MyApp : public TApplication {
public:
    MyApp();

    static TMenuBar *initMenuBar( TRect r );
    static TStatusLine *initStatusLine(TRect r);
    virtual void handleEvent( TEvent& event );
};

MyApp::MyApp() : TProgInit(initStatusLine, initMenuBar, initDeskTop), TApplication() {
    std::filesystem::create_directories("rsender-data/projects");
    std::filesystem::create_directories("rsender-data/generated");
    TRect r(2, 2, size.x - 3, size.y - 3);
    PrepareScriptDialog *dlg = new PrepareScriptDialog(r, "RabbitMQ messages script generator");
    dlg->options |= ofCentered;
    dlg->growMode = gfGrowHiX | gfGrowHiY;

    //execView(dlg);
    deskTop->insert(dlg);
}

void MyApp::handleEvent(TEvent& ev) {
    TApplication::handleEvent(ev);
    if (ev.what == evCommand && (ev.message.command == cmOpenProject || ev.message.command == cmSaveProject)) {
        if (deskTop->current) {
            message(deskTop->current, ev.what, ev.message.command, nullptr);
            clearEvent(ev);
        }
    }
}

TMenuBar* MyApp::initMenuBar(TRect r) {
    r.b.y = r.a.y + 1;
    return new TMenuBar(r,
    *new TSubMenu("~F~ile", kbAltF)
        + *new TMenuItem("~O~pen...", cmOpenProject, kbF3)
        + *new TMenuItem("~S~ave...", cmSaveProject, kbF2)
        + newLine()
        + *new TMenuItem("E~x~it", cmQuit, kbAltX, hcNoContext));
}

TStatusLine *MyApp::initStatusLine( TRect r )
{
    r.a.y = r.b.y - 1;
    return new TStatusLine( r,
      *new TStatusDef( 0, 0xFFFF ) +
        *new TStatusItem( 0, kbShiftDel, cmCut ) +
        *new TStatusItem( 0, kbCtrlIns, cmCopy ) +
        *new TStatusItem( 0, kbShiftIns, cmPaste ) +
        *new TStatusItem( "", kbCtrlF5, cmResize )
        );
}

int main() {
    MyApp *app = new MyApp;
    app->run();
    return 0;
}
