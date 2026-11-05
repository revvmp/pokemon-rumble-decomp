#include "revolution/os/OSArena.h"
#include "revolution/os/OSAudioSystem.h"

extern BOOL __OSInIPL;
u32 OSGetTick(void);
void DCFlushRange(void* pAddr, u32 num);
void* memcpy(void* pDest, const void* pSrc, unsigned long size);

u32 __OSBusClock : 0x800000F8;
volatile u16 __DSPRegs[32] : 0xCC005000;

typedef u32 OSTick;

#define OS_BASE_UNCACHED 0xC0000000
#define OSTicksToMicroseconds(ticks) (((ticks) * 8) / ((__OSBusClock / 4) / 125000))
#define OSDiffTick(tick1, tick0) ((s32)(tick1) - (s32)(tick0))

#define PPCACR_OFFSET (OS_BASE_UNCACHED + 0x800000)
#define PPCACR (0xD000000 + PPCACR_OFFSET)

u16 DSPInitCode[] = {
#include "OSDspInit.inc"
};

#define busRd32(addr) (*(volatile u32 *)(addr))
#define busWrt32(addr, val) (*(volatile u32 *)(addr)) = (val)

static void waitMicroSec(int usec) {
    OSTick t = OSGetTick();
    while (OSTicksToMicroseconds(OSGetTick() - t) < usec);
}

void __AIClockInit(u32 compatB) {
    u32 reg;

    reg = busRd32(PPCACR + 0x180);
    reg &= ~((u32)1 << 8);
    reg |= ((u32)compatB << 8);
    reg &= ~((u32)1 << 7);
    busWrt32(PPCACR + 0x180, reg);

    reg = busRd32(PPCACR + 0x1D0);
    reg &= ~((u32)1 << 31);
    reg &= ~((u32)1 << 30);
    busWrt32(PPCACR + 0x1D0, reg);

    waitMicroSec(100);

    if (compatB == 0) {
        reg = busRd32(PPCACR + 0x1CC);
        reg &= ~0x3FFC0;
        reg |= ((u32)63 << 6);
        reg &= ~0x3F;
        reg &= ~0x7FC0000;
        reg |= ((u32)281 << 18);
        busWrt32(PPCACR + 0x1CC, reg);
    }
    else {
        reg = busRd32(PPCACR + 0x1CC);
        reg &= ~0x3FFC0;
        reg |= ((u32)1023 << 6);
        reg &= ~0x3F;
        reg |= ((u32)14);
        reg &= ~0x7FC0000;
        reg |= ((u32)300 << 18);
        busWrt32(PPCACR + 0x1CC, reg);
    }

    waitMicroSec(100);

    reg = busRd32(PPCACR + 0x1D0);
    reg &= ~((u32)1 << 28);
    busWrt32(PPCACR + 0x1D0, reg);

    waitMicroSec(1000);

    reg = busRd32(PPCACR + 0x1D0);
    reg &= ~((u32)1 << 30);
    reg |= ((u32)1 << 30);
    busWrt32(PPCACR + 0x1D0, reg);

    waitMicroSec(1000);

    reg = busRd32(PPCACR + 0x1D0);
    reg &= ~((u32)1 << 31);
    reg |= ((u32)1 << 31);
    busWrt32(PPCACR + 0x1D0, reg);

    waitMicroSec(1000);
}

void __OSInitAudioSystem(void) {
    u8 errFlag;
    u16 reg16;
    OSTick start_tick;

    if (!__OSInIPL) {
        __AIClockInit(1);
    }

    memcpy((u8*)OSGetArenaHi() - 0x80, (u8*)0x81000000, 0x80);
    memcpy((u8*)0x81000000, DSPInitCode, 0x80);
    DCFlushRange((u8*)0x81000000, 0x80);

    __DSPRegs[9] = 0x43;
    __DSPRegs[5] = 0x8AC;
    __DSPRegs[5] |= 1;

    while (__DSPRegs[5] & 1);

    __DSPRegs[0] = 0;
    while ((__DSPRegs[2] << 16 | __DSPRegs[3]) & 0x80000000);

    *(u32*)&__DSPRegs[0x10] = 0x1000000;
    *(u32*)&__DSPRegs[0x12] = 0;
    *(u32*)&__DSPRegs[0x14] = 0x20;

    reg16 = __DSPRegs[5];

    while (!(reg16 & 0x20)) {
        reg16 = __DSPRegs[5];
    }

    __DSPRegs[5] = reg16;

    start_tick = OSGetTick();

    while(OSDiffTick(OSGetTick(), start_tick) < 2194);

    *(u32*)&__DSPRegs[0x10] = 0x1000000;
    *(u32*)&__DSPRegs[0x12] = 0;
    *(u32*)&__DSPRegs[0x14] = 0x20;

    reg16 = __DSPRegs[5];

    while (!(reg16 & 0x20)) {
        reg16 = __DSPRegs[5];
    }

    __DSPRegs[5] = reg16;
    __DSPRegs[5] &= ~0x800;

    while (__DSPRegs[5] & 0x400);

    __DSPRegs[5] &= ~4;

    errFlag = 0;

    reg16 = __DSPRegs[2];

    while (!(reg16 & 0x8000)) {
        reg16 = __DSPRegs[2];
    }

    __DSPRegs[3];

    __DSPRegs[5] |= 4;
    __DSPRegs[5] = 0x8AC;
    __DSPRegs[5] |= 1;

    while (__DSPRegs[5] & 1);

    memcpy((u8*)0x81000000, (u8*)OSGetArenaHi() - 0x80, 0x80);
}

void __OSStopAudioSystem(void) {
    u16 reg16;
    OSTick start_tick;

    __DSPRegs[5] = 0x804;
    reg16 = __DSPRegs[0x1B];
    __DSPRegs[0x1B] = (u16)(reg16 & ~0x8000);

    reg16 = __DSPRegs[5];
    while (reg16 & 0x400) {
        reg16 = __DSPRegs[5];
    }

    reg16 = __DSPRegs[5];
    while (reg16 & 0x200) {
        reg16 = __DSPRegs[5];
    }

    __DSPRegs[5] = 0x8AC;
    __DSPRegs[0] = 0;

    while ((__DSPRegs[2] << 16 | __DSPRegs[3]) & 0x80000000);

    start_tick = OSGetTick();
    while (OSDiffTick(OSGetTick(), start_tick) < 44);

    reg16 = __DSPRegs[5];
    __DSPRegs[5] = (u16)(reg16 | 1);
    reg16 = __DSPRegs[5];

    while (reg16 & 1) {
        reg16 = __DSPRegs[5];
    }
}
