#ifndef PASSWORDINPUTLINE_H
#define PASSWORDINPUTLINE_H

#define Uses_TInputLine
#include <tvision/tv.h>

class PasswordInputLine : public TInputLine {
public:
    using TInputLine::TInputLine;

    virtual void draw();
};


#endif //PASSWORDINPUTLINE_H
