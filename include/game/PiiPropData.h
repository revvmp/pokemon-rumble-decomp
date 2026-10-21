#ifndef GAME_PIIPROPDATA_H
#define GAME_PIIPROPDATA_H

#include <revolution/types.h>

struct PiiPropData {
    PiiPropData();
    ~PiiPropData();

    u8 mStorage[0x38];
};

#endif
