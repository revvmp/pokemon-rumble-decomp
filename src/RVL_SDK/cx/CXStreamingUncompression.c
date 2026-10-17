#include "private/cx.h"

static inline u32 ReadHeader(u8* pHeaderSize, s32* pDestCount, const u8* srcp, u32 srcSize, s32 forceDestCount) {
    u32 readLen = 0;

    while (*pHeaderSize > 0) {
        --*pHeaderSize;

        if (*pHeaderSize <= 3) {
            *pDestCount |= *srcp << ((3 - *pHeaderSize) * 8);
        } else if (*pHeaderSize <= 6) {
            *pDestCount |= *srcp << ((6 - *pHeaderSize) * 8);
        }

        srcp++;
        readLen++;

        if (*pHeaderSize == 4 && *pDestCount > 0) {
            *pHeaderSize = 0;
        }

        if (--srcSize == 0 && *pHeaderSize > 0) {
            return readLen;
        }
    }

    if (forceDestCount > 0 && forceDestCount < *pDestCount) {
        *pDestCount = forceDestCount;
    }

    return readLen;
}

void CXInitUncompContextHuffman(CXUncompContextHuffman* context, void* dest) {
    context->destp = (u8*)dest;
    context->destCount = 0;
    context->bitSize = 0;
    context->treeSize = -1;
    context->treep = &context->tree[0];
    context->destTmp = 0;
    context->destTmpCnt = 0;
    context->srcTmp = 0;
    context->srcTmpCnt = 0;
    context->headerSize = 8;
    context->forceDestCount = 0;
}

s32 CXReadUncompHuffman(CXUncompContextHuffman* context, const void* data, u32 len) {
    const u8* srcp = (const u8*)data;

    if (context->headerSize > 0) {
        u32 readLen;

        if (context->headerSize == 8) {
            context->bitSize = (u8)(*srcp & 0xF);

            if ((*srcp & CX_COMPRESSION_TYPE_MASK) != CX_COMPRESSION_HUFFMAN) {
                return CX_ERR_UNSUPPORTED;
            }

            if (context->bitSize != 4 && context->bitSize != 8) {
                return CX_ERR_UNSUPPORTED;
            }
        }

        readLen = ReadHeader(&context->headerSize, &context->destCount, srcp, len, context->forceDestCount);
        srcp += readLen;
        len -= readLen;

        if (len == 0) {
            return context->headerSize == 0 ? context->destCount : CX_ERR_UNSUPPORTED;
        }
    }

    if (context->treeSize < 0) {
        context->treeSize = (s16)((*srcp + 1) * 2 - 1);
        *context->treep++ = *srcp++;
        len--;
    }

    while (context->treeSize > 0) {
        if (len == 0) {
            return context->destCount;
        }

        *context->treep++ = *srcp++;
        context->treeSize--;
        len--;

        if (context->treeSize == 0) {
            context->treep = &context->tree[1];

            if (!CXiVerifyHuffmanTable_(context->tree, context->bitSize)) {
                return CX_ERR_ILLEGAL_TABLE;
            }
        }
    }

    while (context->destCount > 0) {
        while (context->srcTmpCnt < 32) {
            if (len == 0) {
                return context->destCount;
            }

            context->srcTmp |= *srcp++ << context->srcTmpCnt;
            len--;
            context->srcTmpCnt += 8;
        }

        while (context->srcTmpCnt > 0) {
            u32 select = context->srcTmp >> 31;
            u32 endFlag = (*(vu8*)context->treep << select) & 0x80;

            context->treep = (u8*)(((u32)context->treep & ~1) + (((*context->treep & 0x3F) + 1) * 2) + select);
            context->srcTmp <<= 1;
            context->srcTmpCnt--;

            if (endFlag) {
                context->destTmp >>= context->bitSize;
                context->destTmp |= *context->treep << (32 - context->bitSize);
                context->treep = &context->tree[1];
                context->destTmpCnt += context->bitSize;

                if (context->destCount <= context->destTmpCnt / 8) {
                    context->destTmp >>= 32 - context->destTmpCnt;
                    context->destTmpCnt = 32;
                }

                if (context->destTmpCnt == 32) {
                    *(u32*)context->destp = CXiConvertEndian_(context->destTmp);
                    context->destp += 4;
                    context->destCount -= 4;
                    context->destTmpCnt = 0;

                    if (context->destCount <= 0) {
                        goto end;
                    }
                }
            }
        }
    }

end:
    if (context->forceDestCount == 0 && len > 32) {
        return CX_ERR_SRC_REMAINDER;
    }

    return CX_ERR_SUCCESS;
}
