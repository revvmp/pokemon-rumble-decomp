#ifndef METROTRK_PORTABLE_SERPOLL_H
#define METROTRK_PORTABLE_SERPOLL_H


#include <MetroTRK/trk.h>

#ifdef __cplusplus
extern "C" {
#endif

void TRKGetInput(void);
extern void* gTRKInputPendingPtr;

int TRKTestForPacket(void);
void TRKProcessInput(int bufferID);
DSError TRKInitializeSerialHandler(void);
DSError TRKTerminateSerialHandler(void);

#ifdef __cplusplus
}
#endif

#endif
