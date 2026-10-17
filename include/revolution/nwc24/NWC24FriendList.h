#ifndef NWC24FRIENDLIST_H
#define NWC24FRIENDLIST_H

#include "revolution/nwc24.h"

#ifdef __cplusplus
extern "C" {
#endif

#define NWC24_FRIEND_LIST_CAPACITY 100

typedef struct NWC24FLHeader {
    u32 magic;
    u32 version;
    u32 capacity;
    u32 size;
    char UNK_0x10[0x40 - 0x10];
    u64 friendCodes[NWC24_FRIEND_LIST_CAPACITY];
} NWC24FLHeader;

NWC24Err NWC24iOpenFriendList(void);

#ifdef __cplusplus
}
#endif

#endif
