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
#include "ui/MessageExtractDialog.h"
#include "Const.h"

class MyApp : public TApplication {
public:
    MyApp();

    static TMenuBar *initMenuBar( TRect r );
    static TStatusLine *initStatusLine(TRect r);
    virtual void handleEvent( TEvent& event );

private:
    PrepareScriptDialog *prepareDcriptDialog;

    void openLogExtractor();
};

MyApp::MyApp() : TProgInit(initStatusLine, initMenuBar, initDeskTop), TApplication() {
    std::filesystem::create_directories("rsender-data/projects");
    std::filesystem::create_directories("rsender-data/generated");
    std::filesystem::create_directories("rsender-data/rabbitmqadmin-scripts");
    std::filesystem::create_directories("rsender-data/run");

    TRect r(2, 2, size.x - 3, size.y - 3);
    prepareDcriptDialog = new PrepareScriptDialog(r, "RabbitMQ messages script generator");
    prepareDcriptDialog->options |= ofCentered;
    prepareDcriptDialog->growMode = gfGrowHiX | gfGrowHiY;

    //execView(dlg);
    deskTop->insert(prepareDcriptDialog);
}

void MyApp::openLogExtractor() {
    TRect r(0, 0, 90, 24);
    auto* dlg = new MessageExtractDialog(r);

    // dlg->setText(clipboardText);

    ushort code = execView(dlg);
    if (code == cmOK) {
        const LogExtractResult& res = dlg->result();
        prepareDcriptDialog->setRoutingKey(res.routingKey);
        prepareDcriptDialog->setParameters(res.valuesLine);
        prepareDcriptDialog->setProperties(res.propertiesJson.dump(2));
        prepareDcriptDialog->setPayload(res.payloadTemplate);
        message(prepareDcriptDialog, evBroadcast, cmUpdateTags, nullptr);
    }
    destroy(dlg);
}

void MyApp::handleEvent(TEvent& ev) {
    TApplication::handleEvent(ev);
    if (ev.what == evCommand && (ev.message.command == cmOpenProject || ev.message.command == cmSaveProject)) {
        if (deskTop->current) {
            message(deskTop->current, ev.what, ev.message.command, nullptr);
            clearEvent(ev);
        }
    }
    if (ev.what == evCommand && (ev.message.command == cmExtractFromLog)) {
        openLogExtractor();
        clearEvent(ev);
    }
}

TMenuBar* MyApp::initMenuBar(TRect r) {
    r.b.y = r.a.y + 1;
    return new TMenuBar(r,
    *new TSubMenu("~F~ile", kbAltF)
        + *new TMenuItem("~O~pen...", cmOpenProject, kbF2)
        + *new TMenuItem("~S~ave...", cmSaveProject, kbF3)
        + *new TMenuItem("~E~xtract...", cmExtractFromLog, kbF4)
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
