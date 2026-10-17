#include "private/cx.h"

s32 CXSecureUncompressRL(const void* srcp, u32 srcSize, void* destp) {
    const u8* pSrc = (const u8*)srcp;
    u8* pDst = (u8*)destp;
    u8 compType = (u8)CXiConvertEndian_(*(u32*)pSrc);
    u32 destCount = CXiConvertEndian_(*(u32*)pSrc) >> 8;
    s32 srcCount = (s32)srcSize;

    if ((compType & CX_COMPRESSION_TYPE_MASK) != CX_COMPRESSION_RL) {
        return CX_ERR_UNSUPPORTED;
    }

    if ((compType & 0xF) != 0) {
        return CX_ERR_UNSUPPORTED;
    }

    if (srcSize <= 4) {
        return CX_ERR_SRC_SHORTAGE;
    }

    pSrc += 4;
    srcCount -= 4;

    if (destCount == 0) {
        if (srcCount < 4) {
            return CX_ERR_SRC_SHORTAGE;
        }

        destCount = CXiConvertEndian_(*(u32*)pSrc);
        pSrc += 4;
        srcCount -= 4;
    }

    while (destCount > 0) {
        u8 flags = *pSrc++;
        s32 length = flags & 0x7F;

        if (--srcCount < 0) {
            return CX_ERR_SRC_SHORTAGE;
        }

        if (!(flags & 0x80)) {
            length += 1;

            if (length > destCount) {
                return CX_ERR_DEST_OVERRUN;
            }

            srcCount -= length;

            if (srcCount < 0) {
                return CX_ERR_SRC_SHORTAGE;
            }

            destCount -= length;

            do {
                *pDst++ = *pSrc++;
            } while (--length > 0);
        } else {
            u8 srcTmp;

            length += 3;

            if (length > destCount) {
                return CX_ERR_DEST_OVERRUN;
            }

            destCount -= length;
            srcTmp = *pSrc++;

            if (--srcCount < 0) {
                return CX_ERR_SRC_SHORTAGE;
            }

            do {
                *pDst++ = srcTmp;
            } while (--length > 0);
        }
    }

    if (srcCount > 32) {
        return CX_ERR_SRC_REMAINDER;
    }

    return CX_ERR_SUCCESS;
}

s32 CXSecureUncompressLZ(const void* srcp, u32 srcSize, void* destp) {
    const u8* pSrc = (const u8*)srcp;
    u8* pDst = (u8*)destp;
    u8 compType = (u8)(CXiConvertEndian_(*(u32*)pSrc) & 0xFF);
    u32 destCount = CXiConvertEndian_(*(u32*)pSrc) >> 8;
    s32 srcCount = (s32)srcSize;
    BOOL exFormat = (*pSrc & 0xF) ? TRUE : FALSE;

    if ((compType & CX_COMPRESSION_TYPE_MASK) != CX_COMPRESSION_LZ) {
        return CX_ERR_UNSUPPORTED;
    }

    if ((compType & 0xF) != 0 && (compType & 0xF) != 1) {
        return CX_ERR_UNSUPPORTED;
    }

    if (srcSize <= 4) {
        return CX_ERR_SRC_SHORTAGE;
    }

    pSrc += 4;
    srcCount -= 4;

    if (destCount == 0) {
        if (srcCount < 4) {
            return CX_ERR_SRC_SHORTAGE;
        }

        destCount = CXiConvertEndian_(*(u32*)pSrc);
        pSrc += 4;
        srcCount -= 4;
    }

    while (destCount > 0) {
        u32 i;
        u32 flags = *pSrc++;

        if (--srcCount < 0) {
            return CX_ERR_SRC_SHORTAGE;
        }

        for (i = 0; i < 8; i++) {
            if (!(flags & 0x80)) {
                *pDst++ = *pSrc++;

                if (--srcCount < 0) {
                    return CX_ERR_SRC_SHORTAGE;
                }

                destCount--;
            } else {
                s32 length = *pSrc >> 4;
                s32 offset;

                if (!exFormat) {
                    length += 3;
                } else if (length == 1) {
                    length = (*pSrc++ & 0xF) << 12;
                    length |= *pSrc++ << 4;
                    length |= *pSrc >> 4;
                    length += 0x111;
                    srcCount -= 2;
                } else if (length == 0) {
                    length = (*pSrc++ & 0xF) << 4;
                    length |= *pSrc >> 4;
                    length += 0x11;
                    srcCount -= 1;
                } else {
                    length += 1;
                }

                offset = (*pSrc++ & 0xF) << 8;
                offset = (offset | *pSrc++) + 1;
                srcCount -= 2;

                if (srcCount < 0) {
                    return CX_ERR_SRC_SHORTAGE;
                }

                if (length > destCount) {
                    return CX_ERR_DEST_OVERRUN;
                }

                if ((u32)&pDst[-offset] < (u32)destp) {
                    return CX_ERR_DEST_OVERRUN;
                }

                destCount -= length;

                do {
                    *pDst = pDst[-offset];
                    pDst++;
                } while (--length > 0);
            }

            if (destCount == 0) {
                break;
            }

            flags <<= 1;
        }
    }

    if (srcCount > 32) {
        return CX_ERR_SRC_REMAINDER;
    }

    return CX_ERR_SUCCESS;
}

BOOL CXiVerifyHuffmanTable_(const void* pTable, u8 bit) {
    const u8* treep = (const u8*)pTable;
    const u8* treeStartp = treep + 1;
    u32 treeSize = *treep;
    const u8* treeEndp = (const u8*)pTable + (treeSize + 1) * 2;
    u32 i;
    u8 end_flags[0x40];
    u32 idx;

    for (i = 0; i < sizeof(end_flags); i++) {
        end_flags[i] = 0;
    }

    if (bit == 4 && treeSize >= 0x10) {
        return FALSE;
    }

    for (idx = 1, treep = treeStartp; treep < treeEndp; idx++, treep++) {
        u32 offset;
        u32 nodep;

        if (end_flags[idx / 8] & (1 << (idx % 8))) {
            continue;
        }

        offset = ((*treep & 0x3F) + 1) << 1;
        nodep = (((u32)treep >> 1) << 1) + offset;

        if (*treep == 0 && idx >= (treeSize << 1)) {
            continue;
        }

        if (nodep >= (u32)treeEndp) {
            return FALSE;
        }

        if (*treep & 0x80) {
            u32 left = (idx & ~1) + offset;

            end_flags[left / 8] |= (u8)(1 << (left % 8));
        }

        if (*treep & 0x40) {
            u32 right = (idx & ~1) + offset + 1;

            end_flags[right / 8] |= (u8)(1 << (right % 8));
        }
    }

    return TRUE;
}

s32 CXSecureUncompressHuffman(const void* srcp, u32 srcSize, void* destp) {
    const u8* pSrc = (const u8*)srcp;
    u32* pDst = (u32*)destp;
    u8 compType = (u8)(CXiConvertEndian_(*(const u32*)pSrc) & 0xFF);
    s32 destCount = (s32)(CXiConvertEndian_(*(u32*)pSrc) >> 8);
    const u8* treep = (destCount != 0) ? pSrc + 4 : pSrc + 8;
    const u8* treeStartp = treep + 1;
    u32 dataBit = *pSrc & 0xF;
    u32 destTmp = 0;
    u32 destTmpCount = 0;
    u32 destTmpDataNum = (dataBit & 7) + 4;
    s32 srcCount = (s32)srcSize;
    u32 treeSize = (*treep + 1) << 1;

    if ((compType & CX_COMPRESSION_TYPE_MASK) != CX_COMPRESSION_HUFFMAN) {
        return CX_ERR_UNSUPPORTED;
    }

    if (dataBit != 4 && dataBit != 8) {
        return CX_ERR_UNSUPPORTED;
    }

    if (destCount == 0) {
        if (srcSize < treeSize + 8) {
            return CX_ERR_SRC_SHORTAGE;
        }

        destCount = (s32)CXiConvertEndian_(*((u32*)pSrc + 1));
    } else if (srcSize < treeSize + 4) {
        return CX_ERR_SRC_SHORTAGE;
    }

    if (!CXiVerifyHuffmanTable_(treep, (u8)dataBit)) {
        return CX_ERR_ILLEGAL_TABLE;
    }

    pSrc = treep + treeSize;
    srcCount -= (u32)pSrc - (u32)srcp;

    if (srcCount < 0) {
        return CX_ERR_SRC_SHORTAGE;
    }

    treep = treeStartp;

    while (destCount > 0) {
        s32 srcTmpCount = 32;
        u32 srcTmp = CXiConvertEndian_(*(u32*)pSrc);

        pSrc += 4;
        srcCount -= 4;

        if (srcCount < 0) {
            return CX_ERR_SRC_SHORTAGE;
        }

        while (--srcTmpCount >= 0) {
            u32 treeShift = srcTmp >> 31;
            u32 node = *treep;

            node <<= treeShift;
            treep = (const u8*)((((u32)treep >> 1) << 1) + (((*treep & 0x3F) + 1) << 1) + treeShift);

            if (node & 0x80) {
                destTmp >>= dataBit;
                destTmp |= *treep << (32 - dataBit);
                treep = treeStartp;
                destTmpCount++;

                if (destCount <= (destTmpCount * dataBit) >> 3) {
                    destTmp >>= dataBit * (destTmpDataNum - destTmpCount);
                    destTmpCount = destTmpDataNum;
                }

                if (destTmpCount == destTmpDataNum) {
                    *pDst++ = CXiConvertEndian_(destTmp);
                    destCount -= 4;
                    destTmpCount = 0;
                }
            }

            if (destCount <= 0) {
                break;
            }

            srcTmp <<= 1;
        }
    }

    if (srcCount > 32) {
        return CX_ERR_SRC_REMAINDER;
    }

    return CX_ERR_SUCCESS;
}

u32 CXiHuffImportTree(u16* pTable, const u8* srcp, u8 bitSize, u32 srcRemainLen) {
    u32 tableSize;
    u32 idx = 1;
    u32 data = 0;
    u32 bitNum = 0;
    u32 bitMask = (1 << bitSize) - 1;
    u32 srcCnt = 0;
    const u32 MAX_IDX = (u32)((1 << bitSize) * 2);

    if (bitSize > 8) {
        tableSize = CXiConvertEndian16_(*(u16*)srcp);
        srcp += 2;
        srcCnt += 2;
    } else {
        tableSize = *srcp;
        srcp += 1;
        srcCnt += 1;
    }

    tableSize = (tableSize + 1) * 4;

    if (srcRemainLen < tableSize) {
        return tableSize;
    }

    while (srcCnt < tableSize) {
        while (bitNum < bitSize) {
            data <<= 8;
            data |= *srcp++;
            srcCnt++;
            bitNum += 8;
        }

        if (idx < MAX_IDX) {
            pTable[idx++] = (u16)((data >> (bitNum - bitSize)) & bitMask);
        }

        bitNum -= bitSize;
    }

    return tableSize;
}

s32 CXSecureUnfilterDiff(const void* srcp, u32 srcSize, void* destp) {
    const u8* pSrc = (const u8*)srcp;
    u8* pDst = (u8*)destp;
    u8 compType = (u8)CXiConvertEndian_(*(u32*)pSrc);
    u32 bitSize = *pSrc & 0xF;
    s32 destCount = (s32)(CXiConvertEndian_(*(u32*)pSrc) >> 8);
    u32 sum = 0;
    s32 srcCount = (s32)srcSize;

    if ((compType & CX_COMPRESSION_TYPE_MASK) != CX_COMPRESSION_DIFF) {
        return CX_ERR_UNSUPPORTED;
    }

    if (bitSize != 0 && bitSize != 1) {
        return CX_ERR_UNSUPPORTED;
    }

    if (srcSize <= 4) {
        return CX_ERR_SRC_SHORTAGE;
    }

    pSrc += 4;
    srcCount -= 4;

    if (bitSize != 1) {
        do {
            u8 data = *pSrc++;

            if (--srcCount < 0) {
                return CX_ERR_SRC_SHORTAGE;
            }

            sum += data;
            *pDst++ = (u8)sum;
        } while (--destCount > 0);
    } else {
        do {
            u16 data = CXiConvertEndian16_(*(u16*)pSrc);

            pSrc += 2;
            srcCount -= 2;

            if (srcCount < 0) {
                return CX_ERR_SRC_SHORTAGE;
            }

            sum += data;
            *(u16*)pDst = CXiConvertEndian16_((u16)sum);
            pDst += 2;
            destCount -= 2;
        } while (destCount > 0);
    }

    if (srcCount > 32) {
        return CX_ERR_SRC_REMAINDER;
    }

    return CX_ERR_SUCCESS;
}

BOOL CXiLHVerifyTable(const void* pTable, u8 bit) {
    const u16* treep = (const u16*)pTable;
    const u16* treeStartp = treep + 1;
    u32 treeSize = *treep;
    const u16* treeEndp = (const u16*)pTable + treeSize;
    u32 i;
    u32 idx;
    const u16 ofs_mask = (u16)((1 << (bit - 2)) - 1);
    const u16 l_mask = (u16)(1 << (bit - 1));
    const u16 r_mask = (u16)(1 << (bit - 2));
    u8 end_flags[0x80];

    for (i = 0; i < sizeof(end_flags); i++) {
        end_flags[i] = 0;
    }

    if (treeSize > (1U << (bit + 1))) {
        return FALSE;
    }

    for (idx = 1, treep = treeStartp; treep < treeEndp; idx++, treep++) {
        u32 offset;
        u32 nodep;

        if (end_flags[idx / 8] & (1 << (idx % 8))) {
            continue;
        }

        offset = ((*treep & ofs_mask) + 1) << 1;
        nodep = ((u32)treep & ~3) + offset * 2;

        if (*treep == 0 && idx >= treeSize - 4) {
            continue;
        }

        if (nodep >= (u32)treeEndp) {
            return FALSE;
        }

        if (*treep & l_mask) {
            u32 left = (idx & ~1) + offset;

            end_flags[left / 8] |= (u8)(1 << (left % 8));
        }

        if (*treep & r_mask) {
            u32 right = offset + (idx & ~1) + 1;

            end_flags[right / 8] |= (u8)(1 << (right % 8));
        }
    }

    return TRUE;
}

typedef struct BitReader {
    const u8* srcp;
    u32 cnt;
    u32 stream;
    u32 stream_len;
    u32 srcSize;
} BitReader;

static inline void BitReader_Init(BitReader* context, const u8* srcp, u32 srcSize) {
    context->srcp = srcp;
    context->cnt = 0;
    context->stream = 0;
    context->stream_len = 0;
    context->srcSize = srcSize;
}

static inline s8 BitReader_Read(BitReader* context) {
    s8 bit;

    if (context->stream_len == 0) {
        if (context->cnt > context->srcSize) {
            return -1;
        }

        context->stream = context->srcp[context->cnt++];
        context->stream_len = 8;
    }

    bit = (s8)((context->stream >> (context->stream_len - 1)) & 1);
    context->stream_len--;

    return bit;
}

#define LENGTH_BITS 9
#define OFFSET_BITS 5

s32 CXSecureUncompressLH(const void* srcp, u32 srcSize, u8* destp, void* work) {
    u32 destSize;
    u32 destCnt = 0;
    const u8* pSrc;
    u16* huffTable12;
    u16* huffTable9;
    u16* nodep;
    u16 val;
    BitReader stream;

    if ((*(const u8*)srcp & CX_COMPRESSION_TYPE_MASK) != CX_COMPRESSION_LH) {
        return CX_ERR_UNSUPPORTED;
    }

    if (srcSize <= 4) {
        return CX_ERR_SRC_SHORTAGE;
    }

    huffTable9 = (u16*)work;
    huffTable12 = (u16*)work + (1 << LENGTH_BITS) * 2;
    destSize = CXiConvertEndian_(*(u32*)srcp) >> 8;
    pSrc = (const u8*)srcp + 4;

    if (destSize == 0) {
        destSize = CXiConvertEndian_(*(u32*)pSrc);
        pSrc += 4;

        if (srcSize < 8) {
            return CX_ERR_SRC_SHORTAGE;
        }
    }

    pSrc += CXiHuffImportTree(huffTable9, pSrc, LENGTH_BITS, srcSize - ((u32)pSrc - (u32)srcp));

    if (pSrc > (const u8*)srcp + srcSize) {
        return CX_ERR_SRC_SHORTAGE;
    }

    if (!CXiLHVerifyTable(huffTable9, LENGTH_BITS)) {
        return CX_ERR_ILLEGAL_TABLE;
    }

    pSrc += CXiHuffImportTree(huffTable12, pSrc, OFFSET_BITS, srcSize - ((u32)pSrc - (u32)srcp));

    if (pSrc > (const u8*)srcp + srcSize) {
        return CX_ERR_SRC_SHORTAGE;
    }

    if (!CXiLHVerifyTable(huffTable12, OFFSET_BITS)) {
        return CX_ERR_ILLEGAL_TABLE;
    }

    BitReader_Init(&stream, pSrc, srcSize - ((u32)pSrc - (u32)srcp));

    while (destCnt < destSize) {
        nodep = huffTable9 + 1;

        while (TRUE) {
            s8 bit = BitReader_Read(&stream);
            u32 offset = (((*nodep & 0x7F) + 1) << 1) + bit;

            if (bit < 0) {
                return CX_ERR_SRC_SHORTAGE;
            }

            if (*nodep & (0x100 >> bit)) {
                nodep = (u16*)((u32)nodep & ~3);
                val = nodep[offset];
                break;
            } else {
                nodep = (u16*)((u32)nodep & ~3);
                nodep += offset;
            }
        }

        if (val < 0x100) {
            destp[destCnt++] = (u8)val;
        } else {
            u16 length = (u16)((val & 0xFF) + 3);
            u16 offset;
            u16 offset_bit;

            nodep = huffTable12 + 1;

            while (TRUE) {
                s8 bit = BitReader_Read(&stream);
                u32 nodeOffset = (((*nodep & 7) + 1) << 1) + bit;

                if (bit < 0) {
                    return CX_ERR_SRC_SHORTAGE;
                }

                if (*nodep & (0x10 >> bit)) {
                    nodep = (u16*)((u32)nodep & ~3);
                    offset_bit = nodep[nodeOffset];
                    break;
                } else {
                    nodep = (u16*)((u32)nodep & ~3);
                    nodep += nodeOffset;
                }
            }

            offset = 0;

            if (offset_bit != 0) {
                offset = 1;

                while (--offset_bit > 0) {
                    offset <<= 1;
                    offset |= BitReader_Read(&stream);
                }
            }

            if (destCnt + length > destSize) {
                return CX_ERR_DEST_OVERRUN;
            }

            offset++;

            while (length--) {
                destp[destCnt] = destp[destCnt - offset];
                destCnt++;
            }
        }
    }

    if (stream.srcSize - stream.cnt > 32) {
        return CX_ERR_SRC_REMAINDER;
    }

    return CX_ERR_SUCCESS;
}

typedef struct RCCompressionInfo {
    u32* freq;
    u32* low_cnt;
    u32 total;
    u8 bitSize;
} RCCompressionInfo;

typedef struct RCState {
    u32 low;
    u32 range;
    u32 code;
} RCState;

static inline void RCInitInfo_(RCCompressionInfo* info, u8 bitSize, void* work) {
    u32 tableMax = 1 << bitSize;
    u32 i;

    info->bitSize = bitSize;
    info->freq = (u32*)work;
    info->low_cnt = (u32*)((u32)work + tableMax * sizeof(u32));

    for (i = 0; i < tableMax; i++) {
        info->freq[i] = 1;
        info->low_cnt[i] = i;
    }

    info->total = tableMax;
}

static void RCAddCount_(RCCompressionInfo* info, u16 val) {
    u32 i;
    u32 tableMax = 1 << info->bitSize;

    info->freq[val]++;
    info->total++;

    for (i = val + 1U; i < tableMax; i++) {
        info->low_cnt[i]++;
    }

    if (info->total >= 0x10000) {
        if (info->freq[0] > 1) {
            info->freq[0] >>= 1;
        }

        info->low_cnt[0] = 0;
        info->total = info->freq[0];

        for (i = 1; i < tableMax; i++) {
            if (info->freq[i] > 1) {
                info->freq[i] >>= 1;
            }

            info->low_cnt[i] = info->low_cnt[i - 1] + info->freq[i - 1];
            info->total += info->freq[i];
        }
    }
}

static __attribute__((noinline)) u16 RCSearch_(RCCompressionInfo* info, u32 code, u32 range, u32 low) {
    u32 tableMax = 1 << info->bitSize;
    u32 codeVal = (code - low) / (range / info->total);
    u32 i = 0;
    u32 j = tableMax - 1;

    while (i < j) {
        u32 k = (i + j) / 2;

        if (info->low_cnt[k] > codeVal) {
            j = k;
        } else {
            i = k + 1;
        }
    }

    while (info->low_cnt[i] > codeVal) {
        i--;
    }

    return (u16)i;
}

static inline void RCInitState_(RCState* state) {
    state->low = 0;
    state->range = 0x80000000;
    state->code = 0;
}

static inline u16 RCGetData_(const u8* srcp, RCCompressionInfo* info, RCState* state, s32* pReadCnt) {
    u16 val = RCSearch_(info, state->code, state->range, state->low);
    s32 cnt = 0;

    {
        u32 tmp = state->range / info->total;

        state->low += info->low_cnt[val] * tmp;
        state->range = info->freq[val] * tmp;
    }

    RCAddCount_(info, val);

    while (state->range < 0x1000000) {
        state->code <<= 8;
        state->code += srcp[cnt++];
        state->range <<= 8;
        state->low <<= 8;
    }

    *pReadCnt = cnt;

    return (u16)val;
}

s32 CXSecureUncompressLRC(const void* srcp, u32 srcSize, u8* destp, void* work) {
    const u8* pSrc = (const u8*)srcp;
    u32 destCnt = 0;
    u32 destSize;
    RCCompressionInfo info9;
    RCCompressionInfo info12;
    RCState rcState;
    s32 readCnt;

    if ((*pSrc & CX_COMPRESSION_TYPE_MASK) != CX_COMPRESSION_LRC) {
        return CX_ERR_UNSUPPORTED;
    }

    if (srcSize <= 4) {
        return CX_ERR_SRC_SHORTAGE;
    }

    RCInitInfo_(&info9, LENGTH_BITS, work);
    RCInitInfo_(&info12, 12, (u8*)work + (1 << LENGTH_BITS) * sizeof(u32) * 2);
    RCInitState_(&rcState);

    destSize = CXiConvertEndian_(*(u32*)pSrc) >> 8;
    pSrc += 4;

    if (destSize == 0) {
        destSize = CXiConvertEndian_(*(u32*)pSrc);
        pSrc += 4;

        if (srcSize < 8) {
            return CX_ERR_SRC_SHORTAGE;
        }
    }

    if (srcSize - ((u32)pSrc - (u32)srcp) < 4) {
        return CX_ERR_SRC_SHORTAGE;
    }

    rcState.code = (u32)((pSrc[0] << 24) | (pSrc[1] << 16) | (pSrc[2] << 8) | pSrc[3]);
    pSrc += 4;

    while (destCnt < destSize) {
        u32 val = RCGetData_(pSrc, &info9, &rcState, &readCnt);

        pSrc += readCnt;

        if (val < 0x100) {
            destp[destCnt++] = (u8)val;
        } else {
            u16 length = (u16)((val & 0xFF) + 3);
            u16 offset;

            val = RCGetData_(pSrc, &info12, &rcState, &readCnt);
            pSrc += readCnt;
            offset = (u16)(val + 1);

            if (destCnt + length > destSize) {
                return CX_ERR_DEST_OVERRUN;
            }

            if (destCnt < offset) {
                return CX_ERR_DEST_OVERRUN;
            }

            if ((u32)pSrc - (u32)srcp > srcSize) {
                return CX_ERR_SRC_SHORTAGE;
            }

            while (length--) {
                destp[destCnt] = destp[destCnt - offset];
                destCnt++;
            }
        }
    }

    return CX_ERR_SUCCESS;
}
