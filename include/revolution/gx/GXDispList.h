#ifndef GXDISPLIST_H
#define GXDISPLIST_H

#ifdef __cplusplus
extern "C" {
#endif

#include "revolution/types.h"
#include "revolution/gx/GXVert.h"

void GXBeginDisplayList(void *, u32);
u32 GXEndDisplayList(void);
void GXCallDisplayList(const void *, u32);

static inline void GXFastCallDisplayList(const void *list, u32 nbytes) {
    GXWGFifo.u8 = 0x40;
    GXWGFifo.u32 = (u32)list;
    GXWGFifo.u32 = nbytes;
}

#ifdef __cplusplus
}
#endif

#endif 