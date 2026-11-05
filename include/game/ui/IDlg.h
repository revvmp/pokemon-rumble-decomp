#ifndef GAME_IDLG_H
#define GAME_IDLG_H

#include <game/core/ICalcDraw.h>

class IDlg : public ICalcDraw {
public:
    virtual ~IDlg() {}
    virtual bool IsClosable() { return true; }
    virtual bool Close() = 0;
};

#endif
