#include "HelpDialog.h"

#define Uses_TWindow
#define Uses_TDialog
#define Uses_TEditor
#define Uses_TScrollBar
#define Uses_TButton
#define Uses_TStaticText
#include <cstring>
#include <tvision/tv.h>

#include "common/ReadOnlyEditor.h"

HelpDialog::HelpDialog(const TRect& bounds)
    : TWindowInit(&HelpDialog::initFrame),
      TDialog(bounds, "Help — How to use rsender")
{
    palette  = dpGrayDialog;
    options |= ofCentered | ofSelectable;
    growMode = gfGrowHiX | gfGrowHiY;
    initUi();
    populateHelp();
}

void HelpDialog::initUi() {
    insert(new TStaticText(TRect(2, 1, size.x - 2, 2),
        "Read this quick guide. Press OK to close."));

    TRect ed = TRect(2, 3, size.x - 3, size.y - 7);
    auto* v = new TScrollBar(TRect(ed.b.x, ed.a.y, ed.b.x + 1, ed.b.y));
    auto* h = new TScrollBar(TRect(ed.a.x, ed.b.y, ed.b.x, ed.b.y + 1));
    insert(v);
    insert(h);

    editor = new ReadOnlyEditor(ed, h, v, nullptr, 0x10000);
    editor->options |= ofFramed;
    insert(editor);

    okBtn = new TButton(TRect(size.x - 12, size.y - 3, size.x - 2, size.y - 1),
                        "OK", cmOK, bfDefault);
    insert(okBtn);
}

void HelpDialog::populateHelp() {
    const char* helpText =
        "rsender — quick usage guide\n"
        "\n"
        "Two ways to start:\n"
        "1) From log extractor\n"
        "   • Paste a log snippet and press Extract\n"
        "   • Reserved tags (auto-generated):\n"
        "       {{RANDOM_UUID}} → UUID (for message_id)\n"
        "       {{NOW}}         → current timestamp (for dataUtworzeniaEventu)\n"
        "     You can toggle reserved-tag behavior via the checkbox in the extractor dialog.\n"
        "   • Extracted tags/values help you fill Parameters and Payload\n"
        "\n"
        "2) From saved JSON\n"
        "   • Load a previously saved project: rsender-data/projects/*.json\n"
        "   • All fields (parameters, properties, payload, connection) are restored\n"
        "\n"
        "Then:\n"
        "   • Adjust Parameters / Properties / Payload templates (placeholders like {{TAG_NAME}})\n"
        "   • Provide RabbitMQ connection details (host, port, vhost, username, password)\n"
        "   • Generate → script saved to rsender-data/generated/\n"
        "   • Run → messages published via rabbitmqadmin.py (workspace under rsender-data/run/)\n"
        "\n"
        "Reserved tags details:\n"
        "   {{RANDOM_UUID}} — unique per usage; used for message_id when enabled\n"
        "   {{NOW}}         — current timestamp; used for dataUtworzeniaEventu when enabled\n"
        "   When reserved tags are applied, they don't appear in the tags/values lists.\n"
        "\n"
        "Data locations:\n"
        "   • Projects: rsender-data/projects/*.json\n"
        "   • Generated scripts: rsender-data/generated/\n"
        "   • Downloaded helpers: rsender-data/rabbitmqadmin-scripts/\n"
        "   • Run scripts: rsender-data/run/\n";

    // Fill the editor
    editor->deleteSelect();
    editor->insertText(helpText, (uint)std::strlen(helpText), false);
    editor->setCurPtr(0, 0);
}
