#include <nw4r/ef/ef_animcurve.h>
#include <nw4r/ef/ef_animcurveinternal.h>
#include <nw4r/ef/ef_particle.h>
#include <nw4r/ef/ef_effect.h>
#include <nw4r/ef/ef_effectsystem.h>

namespace nw4r {
namespace ef {

static u8 sChannelCount[256] = {
    0, 1, 1, 2, 1, 2, 2, 3, 1, 2, 2, 3, 2, 3, 3, 4,
    1, 2, 2, 3, 2, 3, 3, 4, 2, 3, 3, 4, 3, 4, 4, 5,
    1, 2, 2, 3, 2, 3, 3, 4, 2, 3, 3, 4, 3, 4, 4, 5,
    2, 3, 3, 4, 3, 4, 4, 5, 3, 4, 4, 5, 4, 5, 5, 6,
    1, 2, 2, 3, 2, 3, 3, 4, 2, 3, 3, 4, 3, 4, 4, 5,
    2, 3, 3, 4, 3, 4, 4, 5, 3, 4, 4, 5, 4, 5, 5, 6,
    2, 3, 3, 4, 3, 4, 4, 5, 3, 4, 4, 5, 4, 5, 5, 6,
    3, 4, 4, 5, 4, 5, 5, 6, 4, 5, 5, 6, 5, 6, 6, 7,
    1, 2, 2, 3, 2, 3, 3, 4, 2, 3, 3, 4, 3, 4, 4, 5,
    2, 3, 3, 4, 3, 4, 4, 5, 3, 4, 4, 5, 4, 5, 5, 6,
    2, 3, 3, 4, 3, 4, 4, 5, 3, 4, 4, 5, 4, 5, 5, 6,
    3, 4, 4, 5, 4, 5, 5, 6, 4, 5, 5, 6, 5, 6, 6, 7,
    2, 3, 3, 4, 3, 4, 4, 5, 3, 4, 4, 5, 4, 5, 5, 6,
    3, 4, 4, 5, 4, 5, 5, 6, 4, 5, 5, 6, 5, 6, 6, 7,
    3, 4, 4, 5, 4, 5, 5, 6, 4, 5, 5, 6, 5, 6, 6, 7,
    4, 5, 5, 6, 5, 6, 6, 7, 5, 6, 6, 7, 6, 7, 7, 8,
};

static u8 sHighestChannel[256] = {
    0, 1, 2, 2, 4, 4, 4, 4, 8, 8, 8, 8, 8, 8, 8, 8,
    16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16,
    32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32,
    32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32,
    64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64,
    64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64,
    64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64,
    64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64,
    128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128,
    128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128,
    128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128,
    128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128,
    128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128,
    128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128,
    128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128,
    128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128,
};

f32 AnimCurveCalcTime(u8* pCmdList, u32 tick, u32 life) {
    u16 frame;
    u32 loop;
    return detail::CalcTime(reinterpret_cast<const AnimCurveHeader*>(pCmdList),
                             tick, life, frame, loop);
}

u16 AnimCurveCalcFrameStep(u8* pCmdList, u32 tick, u32 life) {
    u32 loop;
    bool final;
    return detail::CalcFrameStep(reinterpret_cast<const AnimCurveHeader*>(pCmdList),
                                  tick, life, loop, final);
}

void AnimCurveExecuteU8(u8* pCmdList, u8* pTarget, u32 tick, u16 seed, u32 life) {

    if (pCmdList[0] == 0xAB) {
        AnimCurveExecuteBakedU8(pCmdList, pTarget, tick, life);
        return;
    }

    u8 mask = pCmdList[3];
    const AnimCurveHeader* pHeader;
    const u8* pKeys;
    const u8* pRanges;
    const u8* pRandomTable;
    const u8* pNameTable;
    f32 frameF32;
    u32 divL;
    u32 divH;
    u32 idx;
    u16 frame;
    u16 keyLo;
    u16 keyHi;
    BOOL exact;

    u8 end = sHighestChannel[mask];
    u32 count = sChannelCount[mask];
    if (count == 0) {
        return;
    }

    u32 stride = (count + 0xD) & ~1;
    detail::SplitCurveResource(pCmdList, &pHeader, &pKeys, &pRanges, &pRandomTable, &pNameTable);
    u32 rstride = count * 2;
    frameF32 = detail::CalcTime(pHeader, tick, life, frame, divL);
    divH = divL;

    const u8* pKeyData = pKeys + 4;
    detail::FindCurveKeys(&idx, &exact, &keyLo, &keyHi, frame, frameF32, (const u16*)pKeyData, stride, 0,
                ((const AnimCurveKey*)pKeys)->count - 1);

    if (exact) {
        const detail::ValueCurveKey* pRecord = (const detail::ValueCurveKey*)(pKeyData + idx * stride);
        const u8* pValues = pRecord->mData.mByte;
        if (pRecord->mFlags == 0) {
            for (u16 bit = 1; bit <= end; bit = (u16)(bit << 1)) {
                if ((mask & bit) != 0) {
                    *pTarget = *pValues++;
                }
                pTarget++;
            }
            return;
        }

        u16 r = *(const u16*)pValues;
        u32 hash = detail::HashCurveSeed(seed, pHeader->randomSeed, r, divL);
        const detail::CurveRandomByte* pRange;
        if ((pRecord->mFlags & 2) == 0) {
            pRange = (const detail::CurveRandomByte*)(pRanges + r * rstride + 4);
        } else {
            pRange = (const detail::CurveRandomByte*)(pRandomTable +
                                              rstride * ((hash >> 16) % *(const u16*)pRandomTable) + 4);
            hash = detail::NextCurveSeed(hash);
        }
        for (u16 bit = 1; bit <= end; bit = (u16)(bit << 1)) {
            if ((mask & bit) != 0) {
                u8 base = pRange->mBase;
                u8 scale = pRange->mRange;
                pRange++;
                s32 v = base + scale * (s16)(hash >> 16) / 32768;
                if (v < 0) {
                    v = 0;
                }
                if (v > 0xFF) {
                    v = 0xFF;
                }
                *pTarget = v;
            }
            if ((pHeader->processFlag & 4) == 0) {
                hash = detail::NextCurveSeed(hash);
            }
            pTarget++;
        }
        return;
    }

    detail::AdvanceCurveLoops(pHeader, (const AnimCurveKey*)pKeys, idx, &divL, &divH);
    u32 t = (u32)(65536.0f * (frameF32 - (f32)keyLo)) / (keyHi - keyLo);
    const detail::ValueCurveKey* pRecordA = (const detail::ValueCurveKey*)(pKeyData + idx * stride);
    const u8* pValuesA = pRecordA->mData.mByte;
    const detail::ValueCurveKey* pRecordB = (const detail::ValueCurveKey*)(pKeyData + (idx + 1) * stride);
    const u8* pValuesB = pRecordB->mData.mByte;
    const u8* pInterpolation = pRecordA->mType;
    BOOL randA = pRecordA->mFlags != 0;
    BOOL randB = pRecordB->mFlags != 0;

    if (!randA && !randB) {
        for (u16 bit = 1; bit <= end; bit = (u16)(bit << 1)) {
            if ((mask & bit) != 0) {
                *pTarget = detail::InterpolateCurveU8(t, *pValuesA, *pValuesB, *pInterpolation);
                pValuesA++;
                pValuesB++;
            }
            pInterpolation++;
            pTarget++;
        }
        return;
    }

    if (randA && !randB) {
        u16 rA = *(const u16*)pValuesA;
        u32 hashA = detail::HashCurveSeed(seed, pHeader->randomSeed, rA, divL);
        const detail::CurveRandomByte* pRangeA;
        if ((pRecordA->mFlags & 2) == 0) {
            pRangeA = (const detail::CurveRandomByte*)(pRanges + rA * rstride + 4);
        } else {
            pRangeA = (const detail::CurveRandomByte*)(pRandomTable +
                                              rstride * ((hashA >> 16) % *(const u16*)pRandomTable) + 4);
            hashA = detail::NextCurveSeed(hashA);
        }
        for (u16 bit = 1; bit <= end; bit = (u16)(bit << 1)) {
            if ((mask & bit) != 0) {
                u8 baseA = pRangeA->mBase;
                u8 scaleA = pRangeA->mRange;
                pRangeA++;
                s32 vA = baseA + scaleA * (s16)(hashA >> 16) / 32768;
                if (vA < 0) {
                    vA = 0;
                }
                if (vA > 0xFF) {
                    vA = 0xFF;
                }
                *pTarget = detail::InterpolateCurveU8(t, vA, *pValuesB, *pInterpolation);
                pValuesB++;
            }
            if ((pHeader->processFlag & 4) == 0) {
                hashA = detail::NextCurveSeed(hashA);
            }
            pInterpolation++;
            pTarget++;
        }
        return;
    }

    if (!randA && randB) {
        u16 rB = *(const u16*)pValuesB;
        u32 hashB = detail::HashCurveSeed(seed, pHeader->randomSeed, rB, divH);
        const detail::CurveRandomByte* pRangeB;
        if ((pRecordB->mFlags & 2) == 0) {
            pRangeB = (const detail::CurveRandomByte*)(pRanges + rB * rstride + 4);
        } else {
            pRangeB = (const detail::CurveRandomByte*)(pRandomTable +
                                              rstride * ((hashB >> 16) % *(const u16*)pRandomTable) + 4);
            hashB = detail::NextCurveSeed(hashB);
        }
        for (u16 bit = 1; bit <= end; bit = (u16)(bit << 1)) {
            if ((mask & bit) != 0) {
                u8 baseB = pRangeB->mBase;
                u8 scaleB = pRangeB->mRange;
                pRangeB++;
                s32 vB = baseB + scaleB * (s16)(hashB >> 16) / 32768;
                if (vB < 0) {
                    vB = 0;
                }
                if (vB > 0xFF) {
                    vB = 0xFF;
                }
                *pTarget = detail::InterpolateCurveU8(t, *pValuesA, vB, *pInterpolation);
                pValuesA++;
            }
            if ((pHeader->processFlag & 4) == 0) {
                hashB = detail::NextCurveSeed(hashB);
            }
            pInterpolation++;
            pTarget++;
        }
        return;
    }

    u16 rA = *(const u16*)pValuesA;
    u32 hashA = detail::HashCurveSeed(seed, pHeader->randomSeed, rA, divL);
    const detail::CurveRandomByte* pRangeA;
    if ((pRecordA->mFlags & 2) == 0) {
        pRangeA = (const detail::CurveRandomByte*)(pRanges + rA * rstride + 4);
    } else {
        pRangeA = (const detail::CurveRandomByte*)(pRandomTable +
                                          rstride * ((hashA >> 16) % *(const u16*)pRandomTable) + 4);
        hashA = detail::NextCurveSeed(hashA);
    }
    u16 rB = *(const u16*)pValuesB;
    u32 hashB = detail::HashCurveSeed(seed, pHeader->randomSeed, rB, divH);
    const detail::CurveRandomByte* pRangeB;
    if ((pRecordB->mFlags & 2) == 0) {
        pRangeB = (const detail::CurveRandomByte*)(pRanges + rB * rstride + 4);
    } else {
        pRangeB = (const detail::CurveRandomByte*)(pRandomTable +
                                          rstride * ((hashB >> 16) % *(const u16*)pRandomTable) + 4);
        hashB = detail::NextCurveSeed(hashB);
    }
    for (u16 bit = 1; bit <= end; bit = (u16)(bit << 1)) {
        if ((mask & bit) != 0) {
            u8 baseA = pRangeA->mBase;
            u8 scaleA = pRangeA->mRange;
            pRangeA++;
            s32 vA = baseA + scaleA * (s16)(hashA >> 16) / 32768;
            if (vA < 0) {
                vA = 0;
            }
            if (vA > 0xFF) {
                vA = 0xFF;
            }
            if ((pHeader->processFlag & 4) == 0) {
                hashA = detail::NextCurveSeed(hashA);
            }
            u8 baseB = pRangeB->mBase;
            u8 scaleB = pRangeB->mRange;
            pRangeB++;
            s32 vB = baseB + scaleB * (s16)(hashB >> 16) / 32768;
            if (vB < 0) {
                vB = 0;
            }
            if (vB > 0xFF) {
                vB = 0xFF;
            }
            if ((pHeader->processFlag & 4) == 0) {
                hashB = detail::NextCurveSeed(hashB);
            }
            *pTarget = detail::InterpolateCurveU8(t, vA, vB, *pInterpolation);
        } else if ((pHeader->processFlag & 4) == 0) {
            hashA = detail::NextCurveSeed(hashA);
            hashB = detail::NextCurveSeed(hashB);
        }
        pInterpolation++;
        pTarget++;
    }
}

void AnimCurveExecuteF32(u8* pCmdList, f32* pTarget, u32 tick, u16 seed, u32 life) {

    if (pCmdList[0] == 0xAB) {
        AnimCurveExecuteBakedF32(pCmdList, pTarget, tick, life);
        return;
    }

    u8 mask = pCmdList[3];
    const AnimCurveHeader* pHeader;
    const u8* pKeys;
    const u8* pRanges;
    const u8* pRandomTable;
    const u8* pNameTable;
    f32 frameF32;
    u32 divL;
    u32 divH;
    u32 idx;
    u16 frame;
    u16 keyLo;
    u16 keyHi;
    BOOL exact;

    detail::SplitCurveResource(pCmdList, &pHeader, &pKeys, &pRanges, &pRandomTable, &pNameTable);
    frameF32 = detail::CalcTime(pHeader, tick, life, frame, divL);
    divH = divL;

    u8 end = sHighestChannel[mask];
    u32 count = sChannelCount[mask];
    if (count == 0) {
        return;
    }

    u32 stride = count * 4 + 0xC;
    u32 rstride = count * 8;
    const u8* pKeyData = pKeys + 4;
    detail::FindCurveKeys(&idx, &exact, &keyLo, &keyHi, frame, frameF32, (const u16*)pKeyData, stride, 0,
                ((const AnimCurveKey*)pKeys)->count - 1);

    if (exact) {
        const detail::ValueCurveKey* pRecord = (const detail::ValueCurveKey*)(pKeyData + idx * stride);
        const f32* pValues = pRecord->mData.mValue;
        if (pRecord->mFlags == 0) {
            for (u16 bit = 1; bit <= end; bit = (u16)(bit << 1)) {
                if ((mask & bit) != 0) {
                    *pTarget = *pValues++;
                }
                pTarget++;
            }
            return;
        }

        u16 r = *(const u16*)pValues;
        u32 hash = detail::HashCurveSeed(seed, pHeader->randomSeed, r, divL);
        const detail::CurveRandomRange* pRange;
        if ((pRecord->mFlags & 2) == 0) {
            pRange = (const detail::CurveRandomRange*)(pRanges + r * rstride + 4);
        } else {
            pRange = (const detail::CurveRandomRange*)(pRandomTable +
                                               rstride * ((hash >> 16) % *(const u16*)pRandomTable) + 4);
            hash = detail::NextCurveSeed(hash);
        }
        for (u16 bit = 1; bit <= end; bit = (u16)(bit << 1)) {
            if ((mask & bit) != 0) {
                f32 base = pRange->mBase;
                f32 scale = pRange->mRange;
                pRange++;
                *pTarget = base + scale * (f32)(hash >> 16);
                if ((pHeader->processFlag & 4) == 0) {
                    hash = detail::NextCurveSeed(hash);
                }
            }
            pTarget++;
        }
        return;
    }

    detail::AdvanceCurveLoops(pHeader, (const AnimCurveKey*)pKeys, idx, &divL, &divH);
    f32 t = (frameF32 - (f32)keyLo) / (f32)(keyHi - keyLo);
    const detail::ValueCurveKey* pRecordA = (const detail::ValueCurveKey*)(pKeyData + idx * stride);
    const f32* pValuesA = pRecordA->mData.mValue;
    const detail::ValueCurveKey* pRecordB = (const detail::ValueCurveKey*)(pKeyData + (idx + 1) * stride);
    const f32* pValuesB = pRecordB->mData.mValue;
    const u8* pInterpolation = pRecordA->mType;
    BOOL randA = pRecordA->mFlags != 0;
    BOOL randB = pRecordB->mFlags != 0;

    if (!randA && !randB) {
        for (u16 bit = 1; bit <= end; bit = (u16)(bit << 1)) {
            if ((mask & bit) != 0) {
                *pTarget = detail::InterpolateCurveF32(t, *pValuesA, *pValuesB, *pInterpolation);
                pValuesA++;
                pValuesB++;
            }
            pInterpolation++;
            pTarget++;
        }
        return;
    }

    if (randA && !randB) {
        u16 r = *(const u16*)pValuesA;
        u32 hash = detail::HashCurveSeed(seed, pHeader->randomSeed, r, divL);
        const detail::CurveRandomRange* pRange;
        if ((pRecordA->mFlags & 2) == 0) {
            pRange = (const detail::CurveRandomRange*)(pRanges + r * rstride + 4);
        } else {
            pRange = (const detail::CurveRandomRange*)(pRandomTable +
                                               rstride * ((hash >> 16) % *(const u16*)pRandomTable) + 4);
            hash = detail::NextCurveSeed(hash);
        }
        for (u16 bit = 1; bit <= end; bit = (u16)(bit << 1)) {
            if ((mask & bit) != 0) {
                f32 base = pRange->mBase;
                f32 scale = pRange->mRange;
                pRange++;
                f32 a = base + scale * (f32)(hash >> 16);
                if ((pHeader->processFlag & 4) == 0) {
                    hash = detail::NextCurveSeed(hash);
                }
                *pTarget = detail::InterpolateCurveF32(t, a, *pValuesB, *pInterpolation);
                pValuesB++;
            }
            pInterpolation++;
            pTarget++;
        }
        return;
    }

    if (!randA && randB) {
        u16 r = *(const u16*)pValuesB;
        u32 hash = detail::HashCurveSeed(seed, pHeader->randomSeed, r, divH);
        const detail::CurveRandomRange* pRange;
        if ((pRecordB->mFlags & 2) == 0) {
            pRange = (const detail::CurveRandomRange*)(pRanges + r * rstride + 4);
        } else {
            pRange = (const detail::CurveRandomRange*)(pRandomTable +
                                               rstride * ((hash >> 16) % *(const u16*)pRandomTable) + 4);
            hash = detail::NextCurveSeed(hash);
        }
        for (u16 bit = 1; bit <= end; bit = (u16)(bit << 1)) {
            if ((mask & bit) != 0) {
                f32 base = pRange->mBase;
                f32 scale = pRange->mRange;
                pRange++;
                f32 b = base + scale * (f32)(hash >> 16);
                if ((pHeader->processFlag & 4) == 0) {
                    hash = detail::NextCurveSeed(hash);
                }
                *pTarget = detail::InterpolateCurveF32(t, *pValuesA, b, *pInterpolation);
                pValuesA++;
            }
            pInterpolation++;
            pTarget++;
        }
        return;
    }

    u16 rA = *(const u16*)pValuesA;
    u32 hashA = detail::HashCurveSeed(seed, pHeader->randomSeed, rA, divL);
    const detail::CurveRandomRange* pRangeA;
    if ((pRecordA->mFlags & 2) == 0) {
        pRangeA = (const detail::CurveRandomRange*)(pRanges + rA * rstride + 4);
    } else {
        pRangeA = (const detail::CurveRandomRange*)(pRandomTable +
                                            rstride * ((hashA >> 16) % *(const u16*)pRandomTable) + 4);
        hashA = detail::NextCurveSeed(hashA);
    }
    u16 rB = *(const u16*)pValuesB;
    u32 hashB = detail::HashCurveSeed(seed, pHeader->randomSeed, rB, divH);
    const detail::CurveRandomRange* pRangeB;
    if ((pRecordB->mFlags & 2) == 0) {
        pRangeB = (const detail::CurveRandomRange*)(pRanges + rB * rstride + 4);
    } else {
        pRangeB = (const detail::CurveRandomRange*)(pRandomTable +
                                            rstride * ((hashB >> 16) % *(const u16*)pRandomTable) + 4);
        hashB = detail::NextCurveSeed(hashB);
    }
    for (u16 bit = 1; bit <= end; bit = (u16)(bit << 1)) {
        if ((mask & bit) != 0) {
            f32 baseA = pRangeA->mBase;
            f32 scaleA = pRangeA->mRange;
            pRangeA++;
            f32 a = baseA + scaleA * (f32)(hashA >> 16);
            if ((pHeader->processFlag & 4) == 0) {
                hashA = detail::NextCurveSeed(hashA);
            }
            f32 baseB = pRangeB->mBase;
            f32 scaleB = pRangeB->mRange;
            pRangeB++;
            f32 b = baseB + scaleB * (f32)(hashB >> 16);
            if ((pHeader->processFlag & 4) == 0) {
                hashB = detail::NextCurveSeed(hashB);
            }
            *pTarget = detail::InterpolateCurveF32(t, a, b, *pInterpolation);
        }
        pInterpolation++;
        pTarget++;
    }
}

void AnimCurveExecuteRotate(u8* pCmdList, f32* pTarget, u32 tick, u16 seed, u32 life) {

    u8 mask = pCmdList[3];
    const AnimCurveHeader* pHeader;
    const u8* pKeys;
    const u8* pRanges;
    const u8* pRandomTable;
    const u8* pNameTable;
    f32 frameF32;
    u32 divL;
    u32 divH;
    u32 idx;
    u16 frame;
    u16 keyLo;
    u16 keyHi;
    BOOL exact;

    detail::SplitCurveResource(pCmdList, &pHeader, &pKeys, &pRanges, &pRandomTable, &pNameTable);
    frameF32 = detail::CalcTime(pHeader, tick, life, frame, divL);
    divH = divL;

    u8 end = sHighestChannel[mask];
    u32 count = sChannelCount[mask];
    if (count == 0) {
        return;
    }

    u32 stride = count * 4 + 0xC;
    u32 pairBytes = count * 8;
    u32 rstride = pairBytes + 4;
    const u8* pKeyData = pKeys + 4;
    detail::FindCurveKeys(&idx, &exact, &keyLo, &keyHi, frame, frameF32, (const u16*)pKeyData, stride, 0,
                ((const AnimCurveKey*)pKeys)->count - 1);

    if (exact) {
        const detail::ValueCurveKey* pRecord = (const detail::ValueCurveKey*)(pKeyData + idx * stride);
        const f32* pValues = pRecord->mData.mValue;
        if (pRecord->mFlags == 0) {
            for (u16 bit = 1; bit <= end; bit = (u16)(bit << 1)) {
                if ((mask & bit) != 0) {
                    *pTarget = *pValues++;
                }
                pTarget++;
            }
            return;
        }

        u16 r = *(const u16*)pValues;
        u32 hash = detail::HashCurveSeed(seed, pHeader->randomSeed, r, divL);
        const detail::CurveRandomRange* pRange;
        if ((pRecord->mFlags & 2) == 0) {
            pRange = (const detail::CurveRandomRange*)(pRanges + r * rstride + 4);
        } else {
            pRange = (const detail::CurveRandomRange*)(pRandomTable +
                                               rstride * ((hash >> 16) % *(const u16*)pRandomTable) + 4);
            hash = detail::NextCurveSeed(hash);
        }
        BOOL negate = FALSE;
        if (*((const u8*)pRange + pairBytes) != 0) {
            if ((hash & 0x10000) == 0) {
                negate = TRUE;
            }
            hash = detail::NextCurveSeed(hash);
        }
        for (u16 bit = 1; bit <= end; bit = (u16)(bit << 1)) {
            if ((mask & bit) != 0) {
                f32 base = pRange->mBase;
                f32 scale = pRange->mRange;
                pRange++;
                f32 v = base + scale * (f32)(hash >> 16);
                if (negate) {
                    v = -v;
                }
                *pTarget = v;
            }
            pTarget++;
            if ((pHeader->processFlag & 4) == 0) {
                hash = detail::NextCurveSeed(hash);
            }
        }
        return;
    }

    detail::AdvanceCurveLoops(pHeader, (const AnimCurveKey*)pKeys, idx, &divL, &divH);
    f32 t = (frameF32 - (f32)keyLo) / (f32)(keyHi - keyLo);
    const detail::ValueCurveKey* pRecordA = (const detail::ValueCurveKey*)(pKeyData + idx * stride);
    const f32* pValuesA = pRecordA->mData.mValue;
    const detail::ValueCurveKey* pRecordB = (const detail::ValueCurveKey*)(pKeyData + (idx + 1) * stride);
    const f32* pValuesB = pRecordB->mData.mValue;
    const u8* pInterpolation = pRecordA->mType;
    BOOL randA = pRecordA->mFlags != 0;
    BOOL randB = pRecordB->mFlags != 0;

    if (!randA && !randB) {
        for (u16 bit = 1; bit <= end; bit = (u16)(bit << 1)) {
            if ((mask & bit) != 0) {
                *pTarget = detail::InterpolateCurveF32(t, *pValuesA, *pValuesB, *pInterpolation);
                pValuesA++;
                pValuesB++;
            }
            pInterpolation++;
            pTarget++;
        }
        return;
    }

    if (randA && !randB) {
        u16 rA = *(const u16*)pValuesA;
        u32 hashA = detail::HashCurveSeed(seed, pHeader->randomSeed, rA, divL);
        const detail::CurveRandomRange* pRangeA;
        if ((pRecordA->mFlags & 2) == 0) {
            pRangeA = (const detail::CurveRandomRange*)(pRanges + rA * rstride + 4);
        } else {
            pRangeA = (const detail::CurveRandomRange*)(pRandomTable +
                                               rstride * ((hashA >> 16) % *(const u16*)pRandomTable) + 4);
            hashA = detail::NextCurveSeed(hashA);
        }
        BOOL negateA = FALSE;
        if (*((const u8*)pRangeA + pairBytes) != 0) {
            if ((hashA & 0x10000) == 0) {
                negateA = TRUE;
            }
            hashA = detail::NextCurveSeed(hashA);
        }
        for (u16 bit = 1; bit <= end; bit = (u16)(bit << 1)) {
            if ((mask & bit) != 0) {
                f32 baseA = pRangeA->mBase;
                f32 scaleA = pRangeA->mRange;
                pRangeA++;
                f32 vA = baseA + scaleA * (f32)(hashA >> 16);
                if (negateA) {
                    vA = -vA;
                }
                *pTarget = detail::InterpolateCurveF32(t, vA, *pValuesB, *pInterpolation);
                pValuesB++;
            }
            pInterpolation++;
            pTarget++;
            if ((pHeader->processFlag & 4) == 0) {
                hashA = detail::NextCurveSeed(hashA);
            }
        }
        return;
    }

    if (!randA && randB) {
        u16 rB = *(const u16*)pValuesB;
        u32 hashB = detail::HashCurveSeed(seed, pHeader->randomSeed, rB, divH);
        const detail::CurveRandomRange* pRangeB;
        if ((pRecordB->mFlags & 2) == 0) {
            pRangeB = (const detail::CurveRandomRange*)(pRanges + rB * rstride + 4);
        } else {
            pRangeB = (const detail::CurveRandomRange*)(pRandomTable +
                                               rstride * ((hashB >> 16) % *(const u16*)pRandomTable) + 4);
            hashB = detail::NextCurveSeed(hashB);
        }
        BOOL negateB = FALSE;
        if (*((const u8*)pRangeB + pairBytes) != 0) {
            if ((hashB & 0x10000) == 0) {
                negateB = TRUE;
            }
            hashB = detail::NextCurveSeed(hashB);
        }
        for (u16 bit = 1; bit <= end; bit = (u16)(bit << 1)) {
            if ((mask & bit) != 0) {
                f32 baseB = pRangeB->mBase;
                f32 scaleB = pRangeB->mRange;
                pRangeB++;
                f32 vB = baseB + scaleB * (f32)(hashB >> 16);
                if (negateB) {
                    vB = -vB;
                }
                *pTarget = detail::InterpolateCurveF32(t, *pValuesA, vB, *pInterpolation);
                pValuesA++;
            }
            pInterpolation++;
            pTarget++;
            if ((pHeader->processFlag & 4) == 0) {
                hashB = detail::NextCurveSeed(hashB);
            }
        }
        return;
    }

    u16 rA = *(const u16*)pValuesA;
    u32 hashA = detail::HashCurveSeed(seed, pHeader->randomSeed, rA, divL);
    const detail::CurveRandomRange* pRangeA;
    if ((pRecordA->mFlags & 2) == 0) {
        pRangeA = (const detail::CurveRandomRange*)(pRanges + rA * rstride + 4);
    } else {
        pRangeA = (const detail::CurveRandomRange*)(pRandomTable +
                                           rstride * ((hashA >> 16) % *(const u16*)pRandomTable) + 4);
        hashA = detail::NextCurveSeed(hashA);
    }
    u16 rB = *(const u16*)pValuesB;
    u32 hashB = detail::HashCurveSeed(seed, pHeader->randomSeed, rB, divH);
    const detail::CurveRandomRange* pRangeB;
    if ((pRecordB->mFlags & 2) == 0) {
        pRangeB = (const detail::CurveRandomRange*)(pRanges + rB * rstride + 4);
    } else {
        pRangeB = (const detail::CurveRandomRange*)(pRandomTable +
                                           rstride * ((hashB >> 16) % *(const u16*)pRandomTable) + 4);
        hashB = detail::NextCurveSeed(hashB);
    }
    BOOL negateA = FALSE;
    if (*((const u8*)pRangeA + pairBytes) != 0) {
        if ((hashA & 0x10000) == 0) {
            negateA = TRUE;
        }
        hashA = detail::NextCurveSeed(hashA);
    }
    BOOL negateB = FALSE;
    if (*((const u8*)pRangeB + pairBytes) != 0) {
        if ((hashB & 0x10000) == 0) {
            negateB = TRUE;
        }
        hashB = detail::NextCurveSeed(hashB);
    }
    for (u16 bit = 1; bit <= end; bit = (u16)(bit << 1)) {
        if ((mask & bit) != 0) {
            f32 baseA = pRangeA->mBase;
            f32 scaleA = pRangeA->mRange;
            pRangeA++;
            f32 vA = baseA + scaleA * (f32)(hashA >> 16);
            if (negateA) {
                vA = -vA;
            }
            f32 baseB = pRangeB->mBase;
            f32 scaleB = pRangeB->mRange;
            pRangeB++;
            f32 vB = baseB + scaleB * (f32)(hashB >> 16);
            if (negateB) {
                vB = -vB;
            }
            *pTarget = detail::InterpolateCurveF32(t, vA, vB, *pInterpolation);
        }
        pInterpolation++;
        pTarget++;
        if ((pHeader->processFlag & 4) == 0) {
            hashA = detail::NextCurveSeed(hashA);
            hashB = detail::NextCurveSeed(hashB);
        }
    }
}

void AnimCurveExecuteTexture(u8* pCmdList, Particle* pParticle, u32 tick, u16 seed, u32 life) {

    u32 resDiv;
    s32 idx;
    detail::TextureCurveKey keyValue;
    u16 frame;
    bool final;
    u32 ch;
    TextureData** pChannel;

    const AnimCurveHeader* pHeader = reinterpret_cast<const AnimCurveHeader*>(pCmdList);
    const u8* pKeys = pCmdList + sizeof(AnimCurveHeader);
    const u8* pRanges = pKeys + pHeader->keyTable;
    const u8* pRandomTable = pRanges + pHeader->rangeTable;
    const u8* pNameTable = pRandomTable + pHeader->randomTable;
    frame = detail::CalcFrameStep(pHeader, tick, life, resDiv, final);

    switch (pHeader->kindType) {
    case 'h':
        pChannel = &pParticle->mParameter.mTexture[0];
        ch = 0;
        break;
    case 'l':
        pChannel = &pParticle->mParameter.mTexture[1];
        ch = 1;
        break;
    case 'p':
        pChannel = &pParticle->mParameter.mTexture[2];
        ch = 2;
        break;
    default:
        return;
    }

    const detail::TextureCurveRecord* pEntries = ((const detail::TextureCurveKeys*)pKeys)->mEntry;
    detail::FindCurveKeyIndex(&idx, frame, &pEntries->mFrame, sizeof(detail::TextureCurveRecord), 0,
                ((const detail::TextureCurveKeys*)pKeys)->mCount - 1, 0);

    const detail::TextureCurveRecord* pEntry = &pEntries[idx];
    if (pEntry->mFlags == 0) {
        detail::CopyTextureCurveKey(&keyValue, &pEntry->mValue.mKey);
    } else {
        u16 r = pEntry->mValue.mRandomIndex;
        u32 hash = detail::HashCurveSeed(seed, pHeader->randomSeed, r, resDiv);
        if ((pEntry->mFlags & 2) == 0) {
            const detail::TextureCurveRandomEntry* pRandomEntry = &((const detail::TextureCurveRandom*)pRanges)->mEntry[r];
            detail::CopyTextureCurveKey(&keyValue, &pRandomEntry->mKey);
            switch (pRandomEntry->mMode) {
            case 1:
                keyValue.mType = (keyValue.mType & 2) | ((hash >> 16) & 1);
                break;
            case 2:
                keyValue.mType = (keyValue.mType & 1) | ((hash >> 16) & 2);
                break;
            case 3:
                keyValue.mType = (hash >> 16) & 3;
                break;
            }
        } else {
            const detail::TextureCurveRandomTable* pTable = (const detail::TextureCurveRandomTable*)pRandomTable;
            detail::CopyTextureCurveKey(&keyValue, &pTable->mKey[(hash >> 16) % pTable->mCount]);
        }
    }

    *pChannel = reinterpret_cast<TextureData*>(((const AnimCurveNameTable*)pNameTable)->datas[keyValue.mNameIndex].work);

    pParticle->mParameter.mTextureWrap &= (u16)~(0xF << (ch * 4));
    pParticle->mParameter.mTextureWrap |= (u16)((keyValue.mValue & 0xF) << (ch * 4));
    pParticle->mParameter.mTextureReverse &= (u8)~(3 << (ch * 2));
    pParticle->mParameter.mTextureReverse |= (u8)((keyValue.mType & 3) << (ch * 2));

}


namespace detail {

void CreateCurveChild(const ChildCurveKey* pKey, u16 seed,
                      const AnimCurveHeader* pHeader,
                      const AnimCurveNameTable* pNames,
                      const ChildCurveTable* pRandom,
                      Particle* pParticle, u32 loop) {
    const ChildCurveSetting* pSetting;
    if (pKey->mFlags == 0) {
        pSetting = &pKey->mData.mSetting;
    } else {
        u32 hash = HashCurveSeed(seed, pHeader->randomSeed, pKey->mData.mRandomIndex, loop);
        if (pKey->mFlags & 2) {
            if (pRandom->mCount == 0) {
                return;
            }
            pSetting = &pRandom->mEntry[(hash >> 16) % pRandom->mCount];
        }
    }
    EmitterResource* pResource = reinterpret_cast<EmitterResource*>(pNames->datas[pSetting->mNameIndex].work);
    if (pResource == nullptr) {
        return;
    }
    if (pSetting->mSetting.type == 0) {
        pParticle->mParticleManager->mManagerEM->mManagerEF->mManagerES->mCreationQueue.AddParticleCreation(
            &pSetting->mSetting, pParticle, pResource, pParticle->mCalcRemain,
            nullptr, nullptr);
    } else {
        pParticle->mParticleManager->mManagerEM->mManagerEF->mManagerES->mCreationQueue.AddEmitterCreation(
            &pSetting->mSetting, pParticle, pResource, pParticle->mCalcRemain,
            nullptr, nullptr);
    }
}

}

void AnimCurveExecuteChild(u8* pCmdList, Particle* pParticle, u32 tick, u16 seed, u32 life) {

    const AnimCurveHeader* pHeader;
    const u8* pKeys;
    const u8* pRanges;
    const u8* pRandomTable;
    const u8* pNameTable;
    u32 divA;
    u32 divB;
    s32 idx;
    u16 tickA;
    u16 tickB;
    bool finalA = false;
    bool finalB;
    const detail::ChildCurveKey* pEntry;

    detail::SplitCurveResource(pCmdList, &pHeader, &pKeys, &pRanges, &pRandomTable, &pNameTable);
    tickA = detail::CalcFrameStep(pHeader, tick, life, divA, finalA);
    tickB = detail::CalcFrameStep(pHeader, tick + 1, life, divB, finalB);

    const detail::ChildCurveKey* pKeyData = ((const detail::ChildCurveKeys*)pKeys)->mEntry;
    u32 div;
    if (divA == divB) {
        if (tickA == tickB) {
            if (finalA == 0) {
                return;
            }
            divB = divA + 1;
            tickB = 1;
        }
        if (tickA < tickB) {
            detail::FindCurveKeyIndex(&idx, tickA, &pKeyData->mFrame, sizeof(detail::ChildCurveKey), 0,
                        ((const detail::ChildCurveKeys*)pKeys)->mCount - 1, 1);
            for (pEntry = &pKeyData[idx]; idx < ((const detail::ChildCurveKeys*)pKeys)->mCount; pEntry++, idx++) {
                if (pEntry->mFrame >= tickA) {
                    if (tickB <= pEntry->mFrame) {
                        return;
                    }
                    detail::CreateCurveChild(pEntry, seed, pHeader, reinterpret_cast<const AnimCurveNameTable*>(pNameTable), reinterpret_cast<const detail::ChildCurveTable*>(pRandomTable), pParticle, divA);
                }
            }
        } else {
            detail::FindCurveKeyIndex(&idx, tickA, &pKeyData->mFrame, sizeof(detail::ChildCurveKey), 0,
                        ((const detail::ChildCurveKeys*)pKeys)->mCount - 1, 0);
            for (pEntry = &pKeyData[idx]; idx >= 0; pEntry--, idx--) {
                if (tickA >= pEntry->mFrame) {
                    if (pEntry->mFrame <= tickB) {
                        return;
                    }
                    detail::CreateCurveChild(pEntry, seed, pHeader, reinterpret_cast<const AnimCurveNameTable*>(pNameTable), reinterpret_cast<const detail::ChildCurveTable*>(pRandomTable), pParticle, divA);
                }
            }
        }
        return;
    }

    if ((pHeader->processFlag & 0x40) != 0) {
        if ((divA & 1) == 0) {
            detail::FindCurveKeyIndex(&idx, tickA, &pKeyData->mFrame, sizeof(detail::ChildCurveKey), 0,
                        ((const detail::ChildCurveKeys*)pKeys)->mCount - 1, 1);
            for (pEntry = &pKeyData[idx]; idx < ((const detail::ChildCurveKeys*)pKeys)->mCount; pEntry++, idx++) {
                if (pEntry->mFrame >= tickA) {
                    if (pEntry->mFrame == pHeader->frameLength - 1) {
                        break;
                    }
                    detail::CreateCurveChild(pEntry, seed, pHeader, reinterpret_cast<const AnimCurveNameTable*>(pNameTable), reinterpret_cast<const detail::ChildCurveTable*>(pRandomTable), pParticle, divA);
                }
            }
        } else {
            detail::FindCurveKeyIndex(&idx, tickA, &pKeyData->mFrame, sizeof(detail::ChildCurveKey), 0,
                        ((const detail::ChildCurveKeys*)pKeys)->mCount - 1, 0);
            for (pEntry = &pKeyData[idx]; idx >= 0; pEntry--, idx--) {
                if (tickA >= pEntry->mFrame) {
                    if (pEntry->mFrame == 0) {
                        break;
                    }
                    detail::CreateCurveChild(pEntry, seed, pHeader, reinterpret_cast<const AnimCurveNameTable*>(pNameTable), reinterpret_cast<const detail::ChildCurveTable*>(pRandomTable), pParticle, divA);
                }
            }
        }

        for (div = divA + 1; div < divB; div++) {
            if ((divA & 1) == 0) {
                idx = 0;
                for (pEntry = pKeyData; idx < ((const detail::ChildCurveKeys*)pKeys)->mCount; pEntry++, idx++) {
                    if (pEntry->mFrame == pHeader->frameLength - 1) {
                        break;
                    }
                    detail::CreateCurveChild(pEntry, seed, pHeader, reinterpret_cast<const AnimCurveNameTable*>(pNameTable), reinterpret_cast<const detail::ChildCurveTable*>(pRandomTable), pParticle, divA);
                }
            } else {
                idx = ((const detail::ChildCurveKeys*)pKeys)->mCount - 1;
                for (pEntry = &pKeyData[idx]; idx >= 0; pEntry--, idx--) {
                    if (pEntry->mFrame == 0) {
                        break;
                    }
                    detail::CreateCurveChild(pEntry, seed, pHeader, reinterpret_cast<const AnimCurveNameTable*>(pNameTable), reinterpret_cast<const detail::ChildCurveTable*>(pRandomTable), pParticle, divA);
                }
            }
        }

        if ((divB & 1) == 0) {
            idx = 0;
            for (pEntry = pKeyData; idx < ((const detail::ChildCurveKeys*)pKeys)->mCount; pEntry++, idx++) {
                if (tickB <= pEntry->mFrame) {
                    return;
                }
                detail::CreateCurveChild(pEntry, seed, pHeader, reinterpret_cast<const AnimCurveNameTable*>(pNameTable), reinterpret_cast<const detail::ChildCurveTable*>(pRandomTable), pParticle, divA);
            }
        } else {
            idx = ((const detail::ChildCurveKeys*)pKeys)->mCount - 1;
            for (pEntry = &pKeyData[idx]; idx >= 0; pEntry--, idx--) {
                if (tickB >= pEntry->mFrame) {
                    return;
                }
                detail::CreateCurveChild(pEntry, seed, pHeader, reinterpret_cast<const AnimCurveNameTable*>(pNameTable), reinterpret_cast<const detail::ChildCurveTable*>(pRandomTable), pParticle, divA);
            }
        }
        return;
    }

    detail::FindCurveKeyIndex(&idx, tickA, &pKeyData->mFrame, sizeof(detail::ChildCurveKey), 0,
                ((const detail::ChildCurveKeys*)pKeys)->mCount - 1, 1);
    for (pEntry = &pKeyData[idx]; idx < ((const detail::ChildCurveKeys*)pKeys)->mCount; pEntry++, idx++) {
        if (pEntry->mFrame >= tickA) {
            detail::CreateCurveChild(pEntry, seed, pHeader, reinterpret_cast<const AnimCurveNameTable*>(pNameTable), reinterpret_cast<const detail::ChildCurveTable*>(pRandomTable), pParticle, divA);
        }
    }
    for (div = divA + 1; div < divB; div++) {
        idx = 0;
        for (pEntry = pKeyData; idx < ((const detail::ChildCurveKeys*)pKeys)->mCount; pEntry++, idx++) {
            detail::CreateCurveChild(pEntry, seed, pHeader, reinterpret_cast<const AnimCurveNameTable*>(pNameTable), reinterpret_cast<const detail::ChildCurveTable*>(pRandomTable), pParticle, divA);
        }
    }
    idx = 0;
    for (pEntry = pKeyData; idx < ((const detail::ChildCurveKeys*)pKeys)->mCount; pEntry++, idx++) {
        if (tickB <= pEntry->mFrame) {
            return;
        }
        detail::CreateCurveChild(pEntry, seed, pHeader, reinterpret_cast<const AnimCurveNameTable*>(pNameTable), reinterpret_cast<const detail::ChildCurveTable*>(pRandomTable), pParticle, divA);
    }
}

void AnimCurveExecuteBakedU8(u8* pCmdList, u8* pTarget, u32 tick, u32 life) {
    u8 mask = pCmdList[3];
    u8 lastChannel = sHighestChannel[mask];
    u32 numChannels = sChannelCount[mask];
    if (numChannels != 0) {
        const AnimCurveHeader* pHeader;
        const u8* pKey;
        const u8* pRanges;
        const u8* pRandom;
        const u8* pNames;
        u16 frame;
        detail::SplitCurveResource(pCmdList, &pHeader, &pKey, &pRanges, &pRandom, &pNames);
        u16 count = pHeader->frameLength;
        if (count <= 1) {
            frame = 0;
        } else {
            u32 flags = pHeader->processFlag;
            u32 infiniteLoop = flags & AnimCurveHeader::PROC_FLAG_INFLOOP;
            if (infiniteLoop == 0 && pHeader->loopCount <= 1) {
                if (!(flags & AnimCurveHeader::PROC_FLAG_FITTING)) {
                    if (tick >= count - 1) {
                        frame = count - 1;
                    } else {
                        frame = tick;
                    }
                } else if (life == 1) {
                    frame = count - 1;
                } else {
                    s32 last = count - 1;
                    f32 value = static_cast<f32>(tick) * (static_cast<f32>(last) / static_cast<f32>(life - 1));
                    if (value > static_cast<f32>(last)) {
                        value = static_cast<f32>(last);
                    }
                    frame = value;
                }
            } else if (!(flags & AnimCurveHeader::PROC_FLAG_FITTING)) {
                u32 span = count - 1;
                u32 loop = tick / span;
                if (!(flags & AnimCurveHeader::PROC_FLAG_TURN)) {
                    if (infiniteLoop == 0 && loop >= pHeader->loopCount) {
                        frame = span;
                    } else {
                        frame = tick - loop * span;
                    }
                } else if (infiniteLoop == 0 && loop >= pHeader->loopCount) {
                    if (pHeader->loopCount % 2 == 0) {
                        frame = 0;
                    } else {
                        frame = span;
                    }
                } else if ((loop & 1) == 0) {
                    frame = tick - loop * span;
                } else {
                    frame = span * (loop + 1) - tick;
                }
            } else {
                u32 last = life - 1;
                if (tick >= last) {
                    if (!(flags & AnimCurveHeader::PROC_FLAG_TURN) || pHeader->loopCount % 2 != 0) {
                        frame = count - 1;
                    } else {
                        frame = 0;
                    }
                } else {
                    s32 span = count - 1;
                    f32 rate = static_cast<f32>(pHeader->loopCount) * (static_cast<f32>(span) / static_cast<f32>(last));
                    u32 loop = static_cast<u32>(static_cast<f32>(tick) * rate / static_cast<f32>(span));
                    f32 value = static_cast<f32>(tick) * rate - static_cast<f32>(loop * span);
                    if (!(flags & AnimCurveHeader::PROC_FLAG_TURN)) {
                        frame = value;
                    } else if ((loop & 1) == 0) {
                        frame = value;
                    } else {
                        frame = static_cast<f32>(span) - value;
                    }
                }
            }
        }
        const u8* pSource = reinterpret_cast<const u8*>(pKey) + numChannels * frame;
        for (u16 bit = 1; bit <= lastChannel; bit = static_cast<u16>(bit << 1)) {
            if (mask & bit) {
                *pTarget = *pSource++;
            }
            pTarget++;
        }
    }
}

void AnimCurveExecuteBakedF32(u8* pCmdList, f32* pTarget, u32 tick, u32 life) {
    u8 mask = pCmdList[3];
    u8 lastChannel = sHighestChannel[mask];
    u32 numChannels = sChannelCount[mask];
    if (numChannels != 0) {
        const AnimCurveHeader* pHeader;
        const u8* pKey;
        const u8* pRanges;
        const u8* pRandom;
        const u8* pNames;
        u16 frame;
        detail::SplitCurveResource(pCmdList, &pHeader, &pKey, &pRanges, &pRandom, &pNames);
        u16 count = pHeader->frameLength;
        if (count <= 1) {
            frame = 0;
        } else {
            u32 flags = pHeader->processFlag;
            u32 infiniteLoop = flags & AnimCurveHeader::PROC_FLAG_INFLOOP;
            if (infiniteLoop == 0 && pHeader->loopCount <= 1) {
                if (!(flags & AnimCurveHeader::PROC_FLAG_FITTING)) {
                    if (tick >= count - 1) {
                        frame = count - 1;
                    } else {
                        frame = tick;
                    }
                } else if (life == 1) {
                    frame = count - 1;
                } else {
                    s32 last = count - 1;
                    f32 value = static_cast<f32>(tick) * (static_cast<f32>(last) / static_cast<f32>(life - 1));
                    if (value > static_cast<f32>(last)) {
                        value = static_cast<f32>(last);
                    }
                    frame = value;
                }
            } else if (!(flags & AnimCurveHeader::PROC_FLAG_FITTING)) {
                u32 span = count - 1;
                u32 loop = tick / span;
                if (!(flags & AnimCurveHeader::PROC_FLAG_TURN)) {
                    if (infiniteLoop == 0 && loop >= pHeader->loopCount) {
                        frame = span;
                    } else {
                        frame = tick - loop * span;
                    }
                } else if (infiniteLoop == 0 && loop >= pHeader->loopCount) {
                    if (pHeader->loopCount % 2 == 0) {
                        frame = 0;
                    } else {
                        frame = span;
                    }
                } else if ((loop & 1) == 0) {
                    frame = tick - loop * span;
                } else {
                    frame = span * (loop + 1) - tick;
                }
            } else {
                u32 last = life - 1;
                if (tick >= last) {
                    if (!(flags & AnimCurveHeader::PROC_FLAG_TURN) || pHeader->loopCount % 2 != 0) {
                        frame = count - 1;
                    } else {
                        frame = 0;
                    }
                } else {
                    s32 span = count - 1;
                    f32 rate = static_cast<f32>(pHeader->loopCount) * (static_cast<f32>(span) / static_cast<f32>(last));
                    u32 loop = static_cast<u32>(static_cast<f32>(tick) * rate / static_cast<f32>(span));
                    f32 value = static_cast<f32>(tick) * rate - static_cast<f32>(loop * span);
                    if (!(flags & AnimCurveHeader::PROC_FLAG_TURN)) {
                        frame = value;
                    } else if ((loop & 1) == 0) {
                        frame = value;
                    } else {
                        frame = static_cast<f32>(span) - value;
                    }
                }
            }
        }
        const f32* pSource = reinterpret_cast<const f32*>(pKey) + numChannels * frame;
        for (u16 bit = 1; bit <= lastChannel; bit = static_cast<u16>(bit << 1)) {
            if (mask & bit) {
                *pTarget = *pSource++;
            }
            pTarget++;
        }
    }
}

}
}
