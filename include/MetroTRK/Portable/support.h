#ifndef METROTRK_PORTABLE_SUPPORT_H
#define METROTRK_PORTABLE_SUPPORT_H

#include <MetroTRK/trk.h>
#include <MetroTRK/Portable/msghndlr.h>
#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

DSError TRKSuppAccessFile(u32 fileHandle, u8* data, size_t* count, DSIOResult* result, BOOL needReply, BOOL read);
DSError TRKRequestSend(TRKBuffer* buffer, int* replyBufferID);
DSError HandleOpenFileSupportRequest(const char* path, u8 openMode, u32* fileHandle, DSIOResult* result);
DSError HandleCloseFileSupportRequest(int fileHandle, DSIOResult* result);
DSError HandlePositionFileSupportRequest(u32 fileHandle, u32* position, u8 seekOrigin, DSIOResult* result);

#ifdef __cplusplus
}
#endif

#endif
