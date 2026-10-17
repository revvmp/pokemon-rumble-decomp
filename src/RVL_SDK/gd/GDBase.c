#include "revolution/gd.h"
#include "revolution/os/OSCache.h"

GDLObj* __GDCurrentDL = NULL;
static GDOverflowCallback overflowcb = NULL;

void GDInitGDLObj(GDLObj* dl, void* start, u32 length) {
    dl->start = start;
    dl->ptr = (u8*)start;
    dl->top = (u8*)start + length;
    dl->length = length;
}

void GDFlushCurrToMem(void) {
    DCFlushRange(__GDCurrentDL->start, __GDCurrentDL->length);
}

void GDPadCurr32(void) {
    u32 n = (u32)__GDCurrentDL->ptr & 31;

    if (n) {
        for (; n < 32; n++) {
            __GDWrite(0);
        }
    }
}

void GDOverflowed(void) {
    if (overflowcb) {
        overflowcb();
    }
}
