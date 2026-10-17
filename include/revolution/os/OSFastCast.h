#ifndef OSFASTCAST_H
#define OSFASTCAST_H

#include "revolution/types.h"

#ifdef __cplusplus
extern "C" {
#endif

#ifdef __MWERKS__
static inline u8 __OSf32tou8(register f32 in) {
    f32 a;
    register f32* ptr = &a;
    register u8 r;
    asm {
        psq_st in, 0(ptr), 1, 2
        lbz r, 0(ptr)
    }
    return r;
}

static inline void OSf32tou8(register f32* in, volatile register u8* out) {
    *out = __OSf32tou8(*in);
}

static inline u16 __OSf32tou16(register f32 in) {
    f32 a;
    register f32* ptr = &a;
    register u16 r;
    asm {
        psq_st in, 0(ptr), 1, 3
        lhz r, 0(ptr)
    }
    return r;
}

static inline void OSf32tou16(register f32* in, volatile register u16* out) {
    *out = __OSf32tou16(*in);
}

static inline s16 __OSf32tos16(register f32 in) {
    f32 a;
    register f32* ptr = &a;
    register s16 r;
    asm {
        psq_st in, 0(ptr), 1, 5
        lha r, 0(ptr)
    }
    return r;
}

static inline void OSf32tos16(register f32* in, volatile register s16* out) {
    *out = __OSf32tos16(*in);
}

static inline f32 __OSu8tof32(register const u8* arg) {
    register f32 ret;
    asm {
        psq_l ret, 0(arg), 1, 2
    }
    return ret;
}

static inline void OSu8tof32(register const u8* in, volatile register f32* out) {
    *out = __OSu8tof32(in);
}

static inline f32 __OSu16tof32(register const u16* arg) {
    register f32 ret;
    asm {
        psq_l ret, 0(arg), 1, 3
    }
    return ret;
}

static inline void OSu16tof32(register const u16* in, volatile register f32* out) {
    *out = __OSu16tof32(in);
}

static inline f32 __OSs16tof32(register const s16* arg) {
    register f32 ret;
    asm {
        psq_l ret, 0(arg), 1, 5
    }
    return ret;
}

static inline void OSs16tof32(register const s16* in, volatile register f32* out) {
    *out = __OSs16tof32(in);
}
#else
#define OSf32tou16(in, out) asm volatile("psq_st   %1, 0(%0), 1, 3 " : : "b"(out), "f"(*(in)) : "memory")
#define OSu16tof32(in, out) asm volatile("psq_l   %0, 0(%1), 1, 3  " : "=f"(*(out)) : "b"(in))
#endif

static inline void OSInitFastCast(void) {
#ifdef __MWERKS__
    asm
    {
        li      r3, 4
        oris    r3, r3, 4
        mtspr   0x392, r3

        li      r3, 5
        oris    r3, r3, 5
        mtspr   0x393, r3

        li      r3, 6
        oris    r3, r3, 6
        mtspr   0x394, r3

        li      r3, 7
        oris    r3, r3, 7
        mtspr   0x395, r3
    }
#endif
}

#ifdef __cplusplus
}
#endif

#endif
