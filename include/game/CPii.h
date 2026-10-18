#ifndef GAME_CPII_H
#define GAME_CPII_H

#include <game/BModelObj.h>

class CPii : public BModelObj {
public:
    enum { TYPE = 1 << 6 };

private:
    u8 mPiiStorage[0x54];
};

#endif
