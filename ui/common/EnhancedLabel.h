#ifndef ENHANCEDLABEL_H
#define ENHANCEDLABEL_H

#define Uses_TLabel
#include <tvision/tv.h>

class EnhancedLabel : public TLabel {
public:
    using TLabel::TLabel;

    void setText(const char* newText);

    void setText(const std::string& newText);

    std::string getText() const;

private:
    std::string textStorage;
};

#endif //ENHANCEDLABEL_H
