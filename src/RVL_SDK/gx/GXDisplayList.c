#include "revolution/gx.h"
#include "revolution/gx/GXRegs.h"

extern void __GXSetDirtyState(void);
extern void __GXSendFlushPrim(void);

void GXCallDisplayList(const void* list, u32 nbytes) {
    if (gx->dirtyState) {
        __GXSetDirtyState();
    }
    if (!(*(u32*)(&gx->vNumNot))) {
        __GXSendFlushPrim();
    }
    GX_WRITE_U8(0x40);
    GX_WRITE_U32((u32)list);
    GX_WRITE_U32(nbytes);
}
