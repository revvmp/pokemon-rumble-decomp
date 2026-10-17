#ifndef RVL_SDK_GXFIFO_H
#define RVL_SDK_GXFIFO_H

#include <revolution/types.h>

typedef u8 GXBool;
typedef void (*GXBreakPtCallback)(void);

typedef struct _GXFifoObj {
    u8 pad[128];
} GXFifoObj;

typedef struct __GXFifoObj {
    u8* base;
    u8* top;
    u32 size;
    u32 hiWatermark;
    u32 loWatermark;
    void* rdPtr;
    void* wrPtr;
    s32 count;
    GXBool wrap;
    GXBool bind_cpu;
    GXBool bind_gp;
} __GXFifoObj;

void GXInitFifoBase(GXFifoObj* fifo, void* base, u32 size);
GXBool CPGPLinkCheck(void);
GXBool __GXIsGPFifoReady(void);
GXBreakPtCallback GXSetBreakPtCallback(GXBreakPtCallback cb);
void __GXFifoInit(void);

#endif
