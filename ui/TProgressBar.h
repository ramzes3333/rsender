#ifndef TPROGRESSBAR_H
#define TPROGRESSBAR_H

#define Uses_TView
#include <tvision/tv.h>

class TProgressBar : public TView {
    int percent;

public:
    TProgressBar(const TRect& bounds, int initPercent);

    void setProgress(int newPercent);

    virtual void draw();
    virtual TPalette &getPalette() const;
    virtual TColorAttr mapColor(uchar index) noexcept;
};

#endif //TPROGRESSBAR_H
