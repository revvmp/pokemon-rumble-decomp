#include "private/cx.h"

u32 CXGetUncompressedSize(const void* srcp) {
    u32 size = CXiConvertEndian_(*(u32*)srcp) >> 8;

    if (size == 0) {
        size = CXiConvertEndian_(*((u32*)srcp + 1));
    }

    return size;
}

void CXUncompressLZ(const void* srcp, void* destp) {
    const u8* pSrc = (const u8*)srcp;
    u8* pDst = (u8*)destp;
    u32 destCount = CXiConvertEndian_(*(u32*)pSrc) >> 8;
    BOOL exFormat = (*pSrc & 0xF) ? TRUE : FALSE;

    pSrc += 4;

    if (destCount == 0) {
        destCount = CXiConvertEndian_(*(u32*)pSrc);
        pSrc += 4;
    }

    while (destCount > 0) {
        u32 i;
        u32 flags = *pSrc++;

        for (i = 0; i < 8; i++) {
            if (!(flags & 0x80)) {
                *pDst++ = *pSrc++;
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
                } else if (length == 0) {
                    length = (*pSrc++ & 0xF) << 4;
                    length |= *pSrc >> 4;
                    length += 0x11;
                } else {
                    length += 1;
                }

                offset = (*pSrc++ & 0xF) << 8;
                offset = (offset | *pSrc++) + 1;

                if (length > destCount) {
                    length = (s32)destCount;
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
}

void CXUncompressHuffman(const void* srcp, void* destp) {
    u32* pSrc = (u32*)srcp;
    u32* pDst = (u32*)destp;
    s32 destCount = (s32)(CXiConvertEndian_(*pSrc) >> 8);
    u8* treep = (destCount != 0) ? (u8*)pSrc + 4 : (u8*)pSrc + 8;
    u8* treeStartp = treep + 1;
    u32 dataBit = *(u8*)pSrc & 0xF;
    u32 destTmp = 0;
    u32 destTmpCount = 0;
    u32 destTmpDataNum = 4 + (dataBit & 7);

    if (destCount == 0) {
        destCount = (s32)CXiConvertEndian_(*(pSrc + 1));
    }

    pSrc = (u32*)(treep + ((*treep + 1) << 1));
    treep = treeStartp;

    while (destCount > 0) {
        s32 srcCount = 32;
        u32 srcTmp = CXiConvertEndian_(*pSrc++);

        while (--srcCount >= 0) {
            u32 treeShift = srcTmp >> 31;
            u32 node = *treep;

            node <<= treeShift;
            treep = (u8*)(((u32)treep & ~1) + (((*treep & 0x3F) + 1) << 1) + treeShift);

            if (node & 0x80) {
                destTmp >>= dataBit;
                destTmp |= *treep << (32 - dataBit);
                treep = treeStartp;

                if (++destTmpCount == destTmpDataNum) {
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
}
