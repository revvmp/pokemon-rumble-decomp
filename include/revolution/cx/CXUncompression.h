#ifndef REVOLUTION_CX_UNCOMPRESSION_H
#define REVOLUTION_CX_UNCOMPRESSION_H

#include "revolution/types.h"

#ifdef __cplusplus
extern "C" {
#endif

#define CX_COMPRESSION_LZ 0x10
#define CX_COMPRESSION_HUFFMAN 0x20
#define CX_COMPRESSION_RL 0x30
#define CX_COMPRESSION_LH 0x40
#define CX_COMPRESSION_LRC 0x50
#define CX_COMPRESSION_DIFF 0x80
#define CX_COMPRESSION_TYPE_MASK 0xF0

u32 CXGetUncompressedSize(const void* srcp);
void CXUncompressLZ(const void* srcp, void* destp);
void CXUncompressHuffman(const void* srcp, void* destp);

#ifdef __cplusplus
}
#endif

#endif
