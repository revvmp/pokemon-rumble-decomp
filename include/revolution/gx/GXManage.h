#ifndef REVOLUTION_GX_GXMANAGE_H
#define REVOLUTION_GX_GXMANAGE_H

#include "revolution/types.h"
#include "revolution/gx/GXEnum.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef void (*GXDrawSyncCallback)(u16 token);
typedef void (*GXDrawDoneCallback)(void);

void GXSetMisc(GXMiscToken token, u32 value);
void GXFlush(void);
void GXResetWriteGatherPipe(void);
void GXAbortFrame(void);
void GXSetDrawSync(u16 token);
void GXSetDrawDone(void);
void GXWaitDrawDone(void);
void GXDrawDone(void);
void GXPixModeSync(void);
void GXTexModeSync(void);
GXDrawSyncCallback GXSetDrawSyncCallback(GXDrawSyncCallback callback);
GXDrawDoneCallback GXSetDrawDoneCallback(GXDrawDoneCallback callback);

#ifdef __cplusplus
}
#endif

#endif
