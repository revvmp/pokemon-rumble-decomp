#ifndef GAME_ISTRAPREMINDER_H
#define GAME_ISTRAPREMINDER_H

#include <game/ICalcDraw.h>

class IStrapReminder : public ICalcDraw {
public:
    virtual ~IStrapReminder() {}
    virtual void Finish() = 0;
};

#endif
