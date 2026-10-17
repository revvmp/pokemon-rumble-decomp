#include "revolution/exi.h"

static u32 __EXISwap32(u32 val) {
    return (val >> 24 & 0x000000FF) | (val >> 8 & 0x0000FF00) | (val << 8 & 0x00FF0000) | (val << 24 & 0xFF000000);
}

BOOL EXIWriteReg(s32 chan, u32 dev, u32 exiCmd, void* reg, s32 size) {
    BOOL err = FALSE;
    u32 reg32;

    switch (size) {
    case 1:
        reg32 = (u32)((*(u8*)reg & 0xFF) << 24);
        break;
    case 2:
        reg32 = (u32)(((*(u16*)reg & 0xFF) << 24) | ((*(u16*)reg & 0xFF00) << 8));
        break;
    default:
        reg32 = __EXISwap32(*(u32*)reg);
        break;
    }

    err |= !EXILock(chan, dev, NULL);
    if (err) {
        return FALSE;
    }

    err |= !EXISelect(chan, dev, 4);
    if (err) {
        EXIUnlock(chan);
        return FALSE;
    }

    err |= !EXIImm(chan, &exiCmd, 4, 1, NULL);
    err |= !EXISync(chan);
    err |= !EXIImm(chan, &reg32, 4, 1, NULL);
    err |= !EXISync(chan);
    err |= !EXIDeselect(chan);
    err |= !EXIUnlock(chan);
    return !err;
}
