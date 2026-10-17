#ifndef METROTRK_PORTABLE_MSGBUF_H
#define METROTRK_PORTABLE_MSGBUF_H

#include <MetroTRK/trk.h>

#ifdef __cplusplus
extern "C" {
#endif

struct TRKBuffer {
    BOOL isInUse;
    u32 length;
    u32 position;
    u8 data[0x880];
};

void* TRKGetBuffer(int id);
DSError TRKSetBufferPosition(TRKBuffer* msg, u32 position);

#ifdef __cplusplus
}
#endif

#endif
