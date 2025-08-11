#define Uses_TApplication
#define Uses_TInputLine
#define Uses_TLabel
#define Uses_TEditor
#define Uses_TScrollBar
#define Uses_TKeys
#define Uses_TButton
#define Uses_TDialog
#define Uses_TDeskTop
#define Uses_TFileDialog
#define Uses_TFileInfoPane

#include "PrepareScriptDialog.h"

#include <fstream>

#include "../script/ScriptGenerator.h"
#include "../script/TagsExtractor.h"
#include "../Const.h"
#include "EnhancedLabel.h"
#include "EnhancedEditor.h"
#include "PasswordInputLine.h"
#include "RunScriptDialog.h"
#include "../util/Utils.h"
#include "../script/ScriptSaver.h"
#include "../util/Logger.h"

static constexpr const char* kProjectsDir = "rsender-data/projects";
static constexpr const char* kFilter = "*.json";

PrepareScriptDialog::PrepareScriptDialog(const TRect& bounds, const char* title) : TWindowInit(&PrepareScriptDialog::initFrame), TDialog(bounds, title) {
    palette = dpBlueDialog;
    options |= ofCentered | ofSelectable;
    growMode = gfGrowHiX | gfGrowHiY;
    //setState(sfModal, true);

    /*-------------------------------------*/
    tagsLabel = new EnhancedLabel(TRect(1, 1, 50, 2), "Tags: no tags", nullptr);
    insert(tagsLabel);

    paramsLabel = new TLabel(TRect(2, 2, 15, 3), "Parameters:", nullptr);
    insert(paramsLabel);

    paramsHScroll = new TScrollBar(TRect(2, 27, 50, 28));
    paramsHScroll->growMode = gfFixed;
    paramsVScroll = new TScrollBar(TRect(50, 3, 51, 27));
    paramsVScroll->growMode = gfFixed;

    paramsEditor = new EnhancedEditor(TRect(2, 3, 50, 27), paramsHScroll, paramsVScroll, nullptr, 1000);
    paramsEditor->options |= ofSelectable | ofFramed;
    paramsEditor->growMode = gfFixed;
    //paramsEditor->setState(sfActive, true);

    insert(paramsHScroll);
    insert(paramsVScroll);
    insert(paramsEditor);

    /*-------------------------------------*/
    propertiesLabel = new TLabel(TRect(53, 1, 66, 2), "Properties:", nullptr);
    insert(propertiesLabel);

    propertiesHScroll = new TScrollBar(TRect(53, 13, size.x-3, 14));
    propertiesHScroll->growMode = gfGrowHiX;
    propertiesVScroll = new TScrollBar(TRect(size.x-3, 2, size.x-2, 13));
    propertiesVScroll->growMode = gfGrowLoX | gfGrowHiX;

    propertiesEditor = new TemplateEditor(TRect(53, 2, size.x-3, 13), propertiesHScroll, propertiesVScroll, nullptr, 1000);
    propertiesEditor->options |= ofSelectable | ofFramed;
    propertiesEditor->growMode = gfGrowHiX;
    //propertiesEditor->setState(sfActive, true);

    insert(propertiesHScroll);
    insert(propertiesVScroll);
    insert(propertiesEditor);

    /*-------------------------------------*/
    payloadLabel = new TLabel(TRect(53, 15, 63, 16), "Payload:", nullptr);
    insert(payloadLabel);

    payloadHScroll = new TScrollBar(TRect(53, 25, size.x-3, 26));
    payloadHScroll->growMode = gfGrowHiX;
    payloadVScroll = new TScrollBar(TRect(size.x-3, 16, size.x-2, 25));
    payloadVScroll->growMode = gfGrowLoX | gfGrowHiX;

    payloadEditor = new TemplateEditor(TRect(53, 16, size.x-3, 25), payloadHScroll, payloadVScroll, nullptr, 1000);
    payloadEditor->options |= ofSelectable | ofFramed;
    payloadEditor->growMode = gfGrowHiX;
    //payloadEditor->setState(sfActive, true);

    insert(payloadHScroll);
    insert(payloadVScroll);
    insert(payloadEditor);

    insert(new TLabel(TRect(52, 27, 65, 28), "Routing-key:", nullptr));
    routingkeyInputLine = new TInputLine(TRect(67, 27, 100, 28), 255);
    insert(routingkeyInputLine);

    insert(new TLabel(TRect(102, 27, 112, 28), "Exchange:", nullptr));
    exchangeInputLine = new TInputLine(TRect(114, 27, 140, 28), 255);
    insert(exchangeInputLine);

    /*-------------------------------------*/
    insert(new TButton(TRect(1, 29, 17, 31), "Clear params", cmClearParams, bfNormal));
    insert(new TButton(TRect(19, 29, 41, 31), "Default properties", cmDefaultProperties, bfNormal));
    insert(new TButton(TRect(43, 29, 62, 31), "Default payload", cmDefaultPayload, bfNormal));
    insert(new TButton(TRect(64, 29, 76, 31), "Generate", cmGenerateScript, bfNormal));
    insert(new TButton(TRect(78, 29, 85, 31), "Run", cmOpenRunWindow, bfNormal));

    /*-------------------------------------*/
    insert(new TLabel(TRect(1, 32, 7, 33), "Host:", nullptr));
    hostInputLine = new TInputLine(TRect(8, 32, 25, 33), 16);
    insert(hostInputLine);

    insert(new TLabel(TRect(27, 32, 33, 33), "Port:", nullptr));
    portInputLine = new TInputLine(TRect(34, 32, 42, 33), 6);
    insert(portInputLine);

    insert(new TLabel(TRect(44, 32, 52, 33), "Login:", nullptr));
    usernameInputLine = new TInputLine(TRect(53, 32, 70, 33), 64);
    insert(usernameInputLine);

    insert(new TLabel(TRect(72, 32, 80, 33), "Pass:", nullptr));
    passwordInputLine = new PasswordInputLine(TRect(81, 32, 100, 33), 64);
    insert(passwordInputLine);

    insert(new TLabel(TRect(102, 32, 110, 33), "Vhost:", nullptr));
    vhostInputLine = new TInputLine(TRect(111, 32, 130, 33), 64);
    insert(vhostInputLine);

    /*-------------------------------------*/
    scriptLabel = new TLabel(TRect(2, 35, 20, 36), "Generated script:", nullptr);
    insert(scriptLabel);

    scriptHScroll = new TScrollBar(TRect(2, 47, size.x-3, 48));
    scriptHScroll->growMode = gfGrowHiX;
    scriptVScroll = new TScrollBar(TRect(size.x-3, 36, size.x-2, 47));
    scriptVScroll->growMode = gfGrowLoX | gfGrowHiX;

    scriptEditor = new EnhancedEditor(TRect(2, 36, size.x-3, 47), scriptHScroll, scriptVScroll, nullptr, 65025);
    scriptEditor->options |= ofSelectable | ofFramed;
    scriptEditor->growMode = gfGrowHiX;
    //scriptEditor->setState(sfActive, true);

    insert(scriptHScroll);
    insert(scriptVScroll);
    insert(scriptEditor);
}

void PrepareScriptDialog::handleEvent(TEvent& event) {
    if (event.what == evBroadcast && event.message.command == cmUpdateTags) {
        if (!tagsLabel)
            return;

        auto *te = new TagsExtractor(propertiesEditor->getEditorText(), payloadEditor->getEditorText());
        std::string tags = te->extractTags();
        if (tags.empty()) {
            tags = "Tags: no tags";
        } else {
            tags = "Tags: " + tags;
        }
        tagsLabel->setText(tags.c_str());
    }
    if (event.what == evCommand) {
        if (event.message.command == cmSaveProject) {
            cmdSave();
            clearEvent(event);
            return;
        }
        if (event.message.command == cmOpenProject) {
            cmdOpen();
            clearEvent(event);
            return;
        }
        if (event.message.command == cmOpenRunWindow) {
            if (lastScriptPath.empty() || !std::filesystem::exists(lastScriptPath)) {
                Logger::log("No generated script file to preview/run. Generate the script first.");
            } else {
                TRect r(0, 0, 90, 24);
                auto *dlg = new RunScriptDialog(r, lastScriptPath);
                if (TView *v = TProgram::application->validView(dlg)) {
                    ushort code = owner->execView(v);
                    if (code == cmOK) {

                    }
                } else {
                    Logger::log("Failed to create Run dialog (invalid rect or out of memory).");
                }
            }
            clearEvent(event);
            return;
        }
        if (event.message.command == cmGenerateScript) {
            auto *rabbit = new RabbitMQAccessData(
                Utils::getInputLineText(hostInputLine),
                Utils::getInputLineText(portInputLine),
                Utils::getInputLineText(vhostInputLine),
                Utils::getInputLineText(usernameInputLine),
                Utils::getInputLineText(passwordInputLine));

            auto *sg = new ScriptGenerator(tagsLabel->getText(), paramsEditor->getEditorText(),
                propertiesEditor->getEditorText(), payloadEditor->getEditorText(),
                Utils::getInputLineText(routingkeyInputLine), Utils::getInputLineText(exchangeInputLine),
                rabbit);

            ScriptResult result = sg->generateScript();
            ScriptSaver::saveScriptToFile(result.name, result.content);
            lastScriptPath = result.name;

            scriptEditor->setSelect(0, scriptEditor->bufLen, false);
            scriptEditor->deleteSelect();
            scriptEditor->insertText(result.content.c_str(), result.content.size(), false);
        } else if (event.message.command == cmDefaultProperties) {
            propertiesEditor->setSelect(0, propertiesEditor->bufLen, false);
            propertiesEditor->deleteSelect();
            propertiesEditor->insertText(defaultProperties.c_str(), strlen(defaultProperties.c_str()), false);
            message(owner, evBroadcast, cmUpdateTags, nullptr);
        } else if (event.message.command == cmDefaultPayload) {
            payloadEditor->setSelect(0, payloadEditor->bufLen, false);
            payloadEditor->deleteSelect();
            payloadEditor->insertText(defaultPayload.c_str(), strlen(defaultPayload.c_str()), false);
            message(owner, evBroadcast, cmUpdateTags, nullptr);
        } else if (event.message.command == cmClearParams) {
            paramsEditor->setSelect(0, paramsEditor->bufLen, false);
            paramsEditor->deleteSelect();
        }
    }

    TDialog::handleEvent(event);
}

struct ScopeCwd {
    std::filesystem::path prev;
    bool ok = false;
    explicit ScopeCwd(const std::filesystem::path& p) : prev(std::filesystem::current_path()) {
        std::error_code ec;
        std::filesystem::create_directories(p, ec); // ignore failure; dialog may still work
        std::filesystem::current_path(p, ec);
        ok = !ec;
    }
    ~ScopeCwd() {
        std::error_code ec;
        std::filesystem::current_path(prev, ec); // best-effort
    }
};

std::string PrepareScriptDialog::askOpenPath() {
    ScopeCwd guard(kProjectsDir);
    auto* d = new TFileDialog("*.json", "Open project", "~N~ame", fdOpenButton, 101);

    if (TView* v = TProgram::application->validView(d)) {
        ushort code = owner->execView(v);
        if (code == cmFileOpen) {
            char name[PATH_MAX] = {0};
            d->getData(name);
            return (std::filesystem::path(kProjectsDir) / name).string();
        }
    }
    return {};
}

static inline bool str_ends_with(const std::string &s, std::string_view suf) {
    if (s.size() < suf.size()) return false;
    return std::equal(s.end() - suf.size(), s.end(), suf.begin());
}

std::string PrepareScriptDialog::askSavePath() {
    ScopeCwd guard(kProjectsDir);
    auto* d = new TFileDialog("*.json", "Save project as", "~N~ame", fdOpenButton, 101);

    if (TView* v = TProgram::application->validView(d)) {
        ushort code = owner->execView(v);
        if (code == cmFileOpen) {
            char name[PATH_MAX] = {0};
            d->getData(name);
            std::string out = (std::filesystem::path(kProjectsDir) / name).string();
            //if (!std::string(name).ends_with(".json")) out += ".json";
            if (!str_ends_with(std::string(name), ".json")) out += ".json";
            return out;
        }
    }
    return {};
}

void PrepareScriptDialog::cmdSave() {
    auto path = askSavePath();
    if (path.empty()) return;

    json j = {
        {"params",        std::string(paramsEditor->getEditorText())},
        {"properties",    std::string(propertiesEditor->getEditorText())},
        {"payload",       std::string(payloadEditor->getEditorText())},
        {"routingKey",    Utils::getInputLineText(routingkeyInputLine)},
        {"exchange",      Utils::getInputLineText(exchangeInputLine)},
        {"host",          Utils::getInputLineText(hostInputLine)},
        {"port",          Utils::getInputLineText(portInputLine)},
        {"username",      Utils::getInputLineText(usernameInputLine)},
        {"password",      Utils::getInputLineText(passwordInputLine)}, // consider not storing plain text
        {"vhost",         Utils::getInputLineText(vhostInputLine)}
    };

    std::filesystem::create_directories(kProjectsDir);
    std::ofstream out(path);
    if (!out) { Logger::log("Cannot open file for writing: " + path); return; }
    out << j.dump(2); // pretty 2-space indent
}

void PrepareScriptDialog::cmdOpen() {
    auto path = askOpenPath();
    if (path.empty()) return;

    std::ifstream in(path);
    if (!in) { Logger::log("Cannot open file: " + path); return; }

    json j;
    try { in >> j; }
    catch (const std::exception& e) { Logger::log(std::string("Invalid JSON: ") + e.what()); return; }

    auto setEditor = [](TEditor* ed, const std::string& s) {
        ed->setSelect(0, ed->bufLen, false);
        ed->deleteSelect();
        ed->insertText(s.c_str(), s.size(), false);
    };
    auto setInput = [](TInputLine* in, const std::string& s) {
        // setData copies the null-terminated string; safe to pass c_str()
        in->setData((void*)s.c_str());
    };

    setEditor(paramsEditor,     j.value("params",     ""));
    setEditor(propertiesEditor, j.value("properties", ""));
    setEditor(payloadEditor,    j.value("payload",    ""));

    setInput(routingkeyInputLine, j.value("routingKey", ""));
    setInput(exchangeInputLine,   j.value("exchange",   ""));
    setInput(hostInputLine,       j.value("host",       ""));
    setInput(portInputLine,       j.value("port",       ""));
    setInput(usernameInputLine,   j.value("username",   ""));
    setInput(passwordInputLine,   j.value("password",   ""));
    setInput(vhostInputLine,      j.value("vhost",      ""));

    // refresh tags after loading
    message(owner, evBroadcast, cmUpdateTags, nullptr);
}
