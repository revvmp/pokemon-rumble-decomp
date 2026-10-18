#ifndef NET_H
#define NET_H

#include <revolution/types.h>

#ifdef __cplusplus
extern "C" {
#endif

void* NETMemCpy(void*, const void*, u32);
void* NETMemSet(void*, int, u32);

const char* NETGetRexPPCVersionPrintable(void);

s32 NETGetWirelessMacAddress(u8* macAddr);

#define NET_SHA1_BLOCK_SIZE 64
#define NET_SHA1_DIGEST_SIZE 20

typedef struct NETSHA1Context {
    u32 state[5];
    u8 buffer[NET_SHA1_BLOCK_SIZE];
    u32 bufferLength;
    u32 blockCountLo;
    u32 blockCountHi;
} NETSHA1Context;

void NETSHA1Init(NETSHA1Context* context);
void NETSHA1Update(NETSHA1Context* context, const void* input, u32 length);
void NETSHA1GetDigest(NETSHA1Context* context, void* digest);

#ifdef __cplusplus
}
#endif

#endif
