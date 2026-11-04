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

static inline void GDSetCurrent(GDLObj* dl) {
    __GDCurrentDL = dl;
}

static inline void GDSetCurrOffset(u32 offset) {
    __GDCurrentDL->ptr = __GDCurrentDL->start + offset;
}

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

static inline void GDWrite_u16(u16 data) {
    GDOverflowCheck(2);
    __GDWrite((u8)(data >> 8));
    __GDWrite((u8)(data & 0xFF));
}

static inline void GDWrite_f32(f32 data) {
    union FloatInt {
        f32 f;
        u32 u;
    };
    union FloatInt value;
    value.f = data;
    GDWrite_u32(value.u);
}

#if defined(NW4R_TEXT_WRITER_EMISSION_LAYOUT)
inline
#endif
static void GDPosition3f32(f32 x, f32 y, f32 z) {
    GDWrite_f32(x);
    GDWrite_f32(y);
    GDWrite_f32(z);
}

#if defined(NW4R_TEXT_WRITER_EMISSION_LAYOUT)
inline
#endif
static void GDColor1u32(u32 color) {
    GDWrite_u32(color);
}

#if defined(NW4R_TEXT_WRITER_EMISSION_LAYOUT)
inline
#endif
static void GDTexCoord2u16(u16 s, u16 t) {
    GDWrite_u16(s);
    GDWrite_u16(t);
}

static inline void GDWriteBPCmd(u32 regval) {
    GDWrite_u8(GX_CMD_LOAD_BP_REG);
    GDWrite_u32(regval);
}

#ifdef __cplusplus
}
#endif

#endif
