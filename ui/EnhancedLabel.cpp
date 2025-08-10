#include "EnhancedLabel.h"

void EnhancedLabel::setText(const char* newText) {
    textStorage = newText;
    text = textStorage.c_str();
    drawView();
}

void EnhancedLabel::setText(const std::string& newText) {
    textStorage = newText;
    text = textStorage.c_str();
    drawView();
}

std::string EnhancedLabel::getText() const {
    return textStorage;
}