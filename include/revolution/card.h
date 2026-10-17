#ifndef RVL_SDK_CARD_H
#define RVL_SDK_CARD_H

#ifdef __cplusplus
extern "C" {
#endif

#include "revolution.h"

typedef struct CARDFileInfo {
    s32 chan;
    s32 fileNo;
    s32 offset;
    s32 length;
    u16 iBlock;
    u16 padding;
} CARDFileInfo;

#ifdef __cplusplus
}
#endif

#endif
