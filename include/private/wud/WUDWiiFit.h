#ifndef PRIVATE_WUD_WIIFIT_H
#define PRIVATE_WUD_WIIFIT_H

#include "revolution/sc.h"

typedef struct WUDWiiFitConfigProfile {
    char devName[20];
    LINK_KEY linkKey;
    u8 profileData[28];
} WUDWiiFitConfigProfile;

typedef struct WUDWiiFitPairingInfo {
    BD_ADDR devAddr;
    LINK_KEY linkKey;
    SCDevInfo devInfo;
    u8 reservedData[42];
    u32 checksum;
    u8 alignmentPadding[28];
} WUDWiiFitPairingInfo;

#endif
