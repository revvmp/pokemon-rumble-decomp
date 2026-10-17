#ifndef REVOLUTION_GD_GDBASE_H
#define REVOLUTION_GD_GDBASE_H

#include "revolution/types.h"

#ifdef __cplusplus
extern "C" {
#endif

#define GX_CMD_LOAD_BP_REG 0x61

#define SS_MASK(mask) ((u32)((0xFE << 24) | (mask)))

typedef struct _GDLObj {
    u8* start;
    u32 length;
    u8* ptr;
    u8* top;
} GDLObj;

typedef void (*GDOverflowCallback)(void);

extern GDLObj* __GDCurrentDL;

void GDInitGDLObj(GDLObj* dl, void* start, u32 length);
void GDFlushCurrToMem(void);
void GDPadCurr32(void);
void GDOverflowed(void);

static inline void GDOverflowCheck(u32 size) {
    if (__GDCurrentDL->ptr + size > __GDCurrentDL->top) {
        GDOverflowed();
    }
}

static inline void __GDWrite(u8 data) {
    *__GDCurrentDL->ptr++ = data;
}

static inline void GDWrite_u8(u8 data) {
    GDOverflowCheck(1);
    __GDWrite(data);
}

static inline void GDWrite_u32(u32 data) {
    GDOverflowCheck(4);
    __GDWrite((u8)((data >> 24) & 0xFF));
    __GDWrite((u8)((data >> 16) & 0xFF));
    __GDWrite((u8)((data >> 8) & 0xFF));
    __GDWrite((u8)((data >> 0) & 0xFF));
}

static inline void GDWriteBPCmd(u32 regval) {
    GDWrite_u8(GX_CMD_LOAD_BP_REG);
    GDWrite_u32(regval);
}

#ifdef __cplusplus
}
#endif

#endif
