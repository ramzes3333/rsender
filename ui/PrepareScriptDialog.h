#ifndef PREPARESCRIPTDIALOG_H
#define PREPARESCRIPTDIALOG_H

#define Uses_TDialog
#define Uses_TInputLine
#include <tvision/tv.h>

#include "ParamsEditor.h"
#include "common/EnhancedLabel.h"
#include "common/EnhancedEditor.h"
#include "TemplateEditor.h"
#include "../validator/ParamsValidator.h"

#include <nlohmann/json.hpp>
using nlohmann::json;

class PrepareScriptDialog : public TDialog {

public:
    PrepareScriptDialog(const TRect& bounds, const char* title);

    void setParameters(const std::string& parameters);
    void setProperties(const std::string& properties);
    void setPayload(const std::string& payload);
    void setRoutingKey(const std::string& routingKey);
    void setExchange(const std::string& exchange);

    virtual void handleEvent( TEvent& event );

private:
    void saveRabbitMQAccessData();

    std::string askOpenPath();
    std::string askSavePath();
    void cmdSave();
    void cmdOpen();

    bool isTemplatesCorrect();

    EnhancedLabel *tagsLabel;

    EnhancedLabel *paramsLabel;
    TScrollBar *paramsHScroll;
    TScrollBar *paramsVScroll;
    ParamsEditor *paramsEditor;

    TLabel *propertiesLabel;
    TScrollBar *propertiesHScroll;
    TScrollBar *propertiesVScroll;
    TemplateEditor *propertiesEditor;

    TLabel *payloadLabel;
    TScrollBar *payloadHScroll;
    TScrollBar *payloadVScroll;
    TemplateEditor *payloadEditor;
    TInputLine *routingkeyInputLine;
    TInputLine *exchangeInputLine;

    TInputLine *hostInputLine;
    TInputLine *portInputLine;
    TInputLine *usernameInputLine;
    TInputLine *passwordInputLine;
    TInputLine *vhostInputLine;

    EnhancedLabel *scriptLabel;
    TScrollBar *scriptHScroll;
    TScrollBar *scriptVScroll;
    EnhancedEditor *scriptEditor;

    std::string lastScriptPath;
};

#endif //PREPARESCRIPTDIALOG_H
