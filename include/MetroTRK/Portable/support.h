#ifndef METROTRK_PORTABLE_SUPPORT_H
#define METROTRK_PORTABLE_SUPPORT_H

#include <MetroTRK/trk.h>
#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef enum { DS_IONoError = 0, DS_IOError = 1, DS_IOEOF = 2 } DSIOResult;
typedef enum { DSREPLY_NoError = 0 } DSReplyError;
typedef union TRKMessageWord { u8 b; u32 r; } TRKMessageWord;
typedef struct CommandReply {
    u32 messageLength;
    TRKMessageWord commandID;
    TRKMessageWord replyError;
    u32 filePosition;
    u8 payload[48];
} CommandReply;
DSError TRKSuppAccessFile(u32 fileHandle, u8* data, size_t* count, DSIOResult* result, BOOL needReply, BOOL read);
DSError TRKRequestSend(TRKBuffer* buffer, int* replyBufferID);
DSError HandleOpenFileSupportRequest(const char* path, u8 openMode, u32* fileHandle, DSIOResult* result);
DSError HandleCloseFileSupportRequest(int fileHandle, DSIOResult* result);
DSError HandlePositionFileSupportRequest(u32 fileHandle, u32* position, u8 seekOrigin, DSIOResult* result);

#ifdef __cplusplus
}
#endif

#endif
