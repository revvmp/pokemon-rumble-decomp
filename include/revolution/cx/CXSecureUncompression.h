#ifndef REVOLUTION_CX_SECUREUNCOMPRESSION_H
#define REVOLUTION_CX_SECUREUNCOMPRESSION_H

#include "revolution/types.h"

#ifdef __cplusplus
extern "C" {
#endif

#define CX_ERR_SUCCESS 0
#define CX_ERR_UNSUPPORTED -1
#define CX_ERR_SRC_SHORTAGE -2
#define CX_ERR_SRC_REMAINDER -3
#define CX_ERR_DEST_OVERRUN -4
#define CX_ERR_ILLEGAL_TABLE -5

s32 CXSecureUncompressRL(const void* srcp, u32 srcSize, void* destp);
s32 CXSecureUncompressLZ(const void* srcp, u32 srcSize, void* destp);
s32 CXSecureUncompressHuffman(const void* srcp, u32 srcSize, void* destp);
s32 CXSecureUnfilterDiff(const void* srcp, u32 srcSize, void* destp);
s32 CXSecureUncompressLH(const void* srcp, u32 srcSize, u8* destp, void* work);
s32 CXSecureUncompressLRC(const void* srcp, u32 srcSize, u8* destp, void* work);

#ifdef __cplusplus
}
#endif

#endif
