#include "EnhancedEditor.h"

std::string EnhancedEditor::getEditorText() {
    std::string result;
    result.reserve(bufLen);
    for (size_t i = 0; i < bufLen; ++i) {
        result.push_back(bufChar((ushort)i));
    }
    return result;
}