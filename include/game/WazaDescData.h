#ifndef GAME_WAZADESCDATA_H
#define GAME_WAZADESCDATA_H

#include <revolution/types.h>

struct WazaDescData {
    WazaDescData();
    ~WazaDescData();

    u8 mStorage[0x2C];
};

#endif
