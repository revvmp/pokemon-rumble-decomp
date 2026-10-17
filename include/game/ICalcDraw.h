#ifndef GAME_ICALCDRAW_H
#define GAME_ICALCDRAW_H

#include <game/ICalc.h>

class ICalcDraw : public ICalc {
public:
    virtual ~ICalcDraw() {}
    virtual void PrepareDraw() {}
    virtual void DrawBefore() {}
    virtual void Draw() = 0;
};

#endif
