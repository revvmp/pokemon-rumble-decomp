#ifndef GAME_ICALCDRAW_H
#define GAME_ICALCDRAW_H

#include <game/core/ICalc.h>

class ICalcDraw : public ICalc {
public:
    virtual void CalcView() {}
    virtual void Draw() = 0;
};

#endif
