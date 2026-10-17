#ifndef REVOLUTION_CX_STREAMINGUNCOMPRESSION_H
#define REVOLUTION_CX_STREAMINGUNCOMPRESSION_H

#include "revolution/types.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct CXUncompContextHuffman {
    u8* destp;
    s32 destCount;
    s32 forceDestCount;
    u8* treep;
    u32 srcTmp;
    u32 destTmp;
    s16 treeSize;
    u8 srcTmpCnt;
    u8 destTmpCnt;
    u8 bitSize;
    u8 headerSize;
    u8 padding_[2];
    u8 tree[0x200];
} CXUncompContextHuffman;

void CXInitUncompContextHuffman(CXUncompContextHuffman* context, void* dest);
s32 CXReadUncompHuffman(CXUncompContextHuffman* context, const void* data, u32 len);

#ifdef __cplusplus
}
#endif

#endif
