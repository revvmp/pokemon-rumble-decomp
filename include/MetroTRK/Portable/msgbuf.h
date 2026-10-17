#ifndef METROTRK_PORTABLE_MSGBUF_H
#define METROTRK_PORTABLE_MSGBUF_H

#include <MetroTRK/trk.h>
#include <stddef.h>

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

DSError TRKInitializeMessageBuffers(void);
DSError TRKGetFreeBuffer(int* bufferID, TRKBuffer** buffer);
void TRKReleaseBuffer(int bufferID);
void TRKResetBuffer(TRKBuffer* buffer, BOOL keepData);
DSError TRKAppendBuffer(TRKBuffer* buffer, const void* data, size_t length);
DSError TRKReadBuffer(TRKBuffer* buffer, void* data, size_t length);
DSError TRKAppendBuffer1_ui8(TRKBuffer* buffer, u8 data);
DSError TRKAppendBuffer1_ui32(TRKBuffer* buffer, u32 data);
DSError TRKAppendBuffer1_ui64(TRKBuffer* buffer, u64 data);
DSError TRKAppendBuffer_ui8(TRKBuffer* buffer, const u8* data, int count);
DSError TRKAppendBuffer_ui32(TRKBuffer* buffer, const u32* data, int count);
DSError TRKReadBuffer1_ui8(TRKBuffer* buffer, u8* data);
DSError TRKReadBuffer1_ui32(TRKBuffer* buffer, u32* data);
DSError TRKReadBuffer1_ui64(TRKBuffer* buffer, u64* data);
DSError TRKReadBuffer_ui8(TRKBuffer* buffer, u8* data, int count);
DSError TRKReadBuffer_ui32(TRKBuffer* buffer, u32* data, int count);

#ifdef __cplusplus
}
#endif

#endif
