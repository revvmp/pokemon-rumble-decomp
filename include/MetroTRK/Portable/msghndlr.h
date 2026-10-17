#ifndef METROTRK_PORTABLE_MSGHNDLR_H
#define METROTRK_PORTABLE_MSGHNDLR_H

#include <MetroTRK/trk.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef union TRKMessageWord {
    u8 b;
    u32 r;
} TRKMessageWord;

typedef struct CommandReply {
    u32 messageLength;
    TRKMessageWord commandID;
    TRKMessageWord replyError;
    union {
        u32 sequence;
        u32 filePosition;
    } parameter;
    u8 payload[0x30];
} CommandReply;

BOOL GetTRKConnected(void);
void SetTRKConnected(BOOL connected);
DSError TRKSendACK(TRKBuffer* buffer);
DSError TRKStandardACK(TRKBuffer* buffer, MessageCommandID commandID, DSReplyError replyError);

DSError TRKDoConnect(TRKBuffer* buffer);
DSError TRKDoDisconnect(TRKBuffer* buffer);
DSError TRKDoReset(TRKBuffer* buffer);
DSError TRKDoOverride(TRKBuffer* buffer);
DSError TRKDoReadMemory(TRKBuffer* buffer);
DSError TRKDoWriteMemory(TRKBuffer* buffer);
DSError TRKDoReadRegisters(TRKBuffer* buffer);
DSError TRKDoWriteRegisters(TRKBuffer* buffer);
DSError TRKDoContinue(TRKBuffer* buffer);
DSError TRKDoStep(TRKBuffer* buffer);
DSError TRKDoStop(TRKBuffer* buffer);
DSError TRKDoSetOption(TRKBuffer* buffer);

#ifdef __cplusplus
}
#endif

#endif
