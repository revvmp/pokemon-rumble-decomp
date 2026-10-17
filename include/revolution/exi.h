#ifndef REVOLUTION_EXI_H
#define REVOLUTION_EXI_H

#include "revolution/os.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef void (*EXICallback)(s32 chan, OSContext* context);

BOOL EXIImm(s32 chan, void* buf, s32 len, u32 type, EXICallback callback);
BOOL EXISync(s32 chan);
BOOL EXIAttach(s32 chan, EXICallback extCallback);
BOOL EXIDetach(s32 chan);
BOOL EXISelect(s32 chan, u32 dev, u32 freq);
BOOL EXIDeselect(s32 chan);
BOOL EXILock(s32 chan, u32 dev, EXICallback unlockedCallback);
BOOL EXIUnlock(s32 chan);
s32 EXIGetID(s32 chan, u32 dev, u32* id);

BOOL EXIWriteReg(s32 chan, u32 dev, u32 exiCmd, void* reg, s32 size);

void __OSEnableBarnacle(s32 chan, u32 dev);

#ifdef __cplusplus
}
#endif

#endif
