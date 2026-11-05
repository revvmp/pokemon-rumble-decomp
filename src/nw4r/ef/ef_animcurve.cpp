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

inline u32 HashByteCurveSeed(u16 particleSeed, u16 curveSeed, u16 randomIndex, u32 loop) {
    detail::CurveRandomSeed h;
    h.mWord = (((((u32)particleSeed * 0x3F81F635 + (u32)curveSeed * 0x30A74193) +
                (u32)randomIndex * 0x371097E7) + (u32)loop * 0x7B929) + 0x4BF53);
    h.mByte[2] ^= h.mByte[3];
    h.mByte[1] ^= h.mByte[2];
    h.mByte[0] ^= h.mByte[1];
    return h.mWord;
}

struct ByteCurveValue {
    f32 mValue;

    inline ByteCurveValue(f32 value) : mValue(value) {}
    inline ByteCurveValue(const ByteCurveValue& rOther) : mValue(rOther.mValue) {}
    inline operator f32() const { return mValue; }
};

inline u8 InterpolateByteCurveInternal(u32 tick, u8 start, u8 end, u16 type) {
    u8 s = start;
    u8 e = end;

    if (s == e) {
        return static_cast<u8>(start);
    }
    switch (type & 3) {
    case 0:
        return (u8)(s + ((tick * (e - s)) >> 16));
    case 1: {
        f32 t = (f32)tick;
        t = t / 65536.0f;
        if (type == 1) {
            f32 s0 = (f32)s;
            ByteCurveValue w = 3.0f - 2.0f * t;
            return s0 + t * (t * (w.mValue * (f32)(int)(e - s)));
        } else {
            f32 c1 = ((type >> 2) & 1) == 0 ? 0.0f : 1.5f;
            f32 c0 = ((type >> 3) & 1) == 0 ? 0.0f : 1.5f;
            f32 s0 = (f32)s;
            ByteCurveValue d = t * (f32)(int)(e - s);
            ByteCurveValue u = t * (t * (c0 + c1 - 2.0f));
            ByteCurveValue v = t * (3.0f + (-2.0f * c0 - c1));
            f32 r = s0 + d * (c0 + (u + v));
            if (r < 0.0f) {
                return 0;
            }
            if (r > 255.0f) {
                return 0xFF;
            }
            return r;
        }
    }
    case 2:
        return static_cast<u8>(start);
    default:
        return 0;
    }
}

inline u8 InterpolateByteCurve(u32 tick, u8 start, u8 end, u16 type) {
    return InterpolateByteCurveInternal(tick, start, end, type);
}

inline void FindByteCurveKeys(u32* pIndex, BOOL* pExact, u16* pLeftFrame,
                              u16* pRightFrame, s32 frame, f32 frameF32,
                              const u16* pKeys, u32 stride, s32 lower, s32 hi) {
    s32 lo = lower;
    s32 mid = (lo + hi) / 2;
    BOOL atTarget = nw4r::ef::FAbs((f32)frame - frameF32) < 1.1920929e-07f;

    u16 keyLo = detail::ReadCurveKeyFrame(pKeys, stride, lo);
    *pLeftFrame = keyLo;
    if (frame < keyLo) {
        *pIndex = lo;
        *pExact = 1;
        return;
    }
    if (frame == keyLo) {
        *pIndex = lo;
        if (lo == hi) {
            *pExact = 1;
            return;
        }
        *pExact = atTarget;
        if (atTarget == 0) {
            *pRightFrame = detail::ReadCurveKeyFrame(pKeys, stride, lo + 1);
        }
        return;
    }

    u16 keyHi = detail::ReadCurveKeyFrame(pKeys, stride, hi);
    *pRightFrame = keyHi;
    if (keyHi <= frame) {
        *pIndex = hi;
        *pExact = 1;
        return;
    }

    s32 key = detail::ReadCurveKeyFrame(pKeys, stride, mid);
    while (lo < mid) {
        if (frame == key) {
            *pIndex = mid;
            *pExact = atTarget;
            if (atTarget != 0) {
                return;
            }
            *pLeftFrame = key;
            *pRightFrame = detail::ReadCurveKeyFrame(pKeys, stride, *pIndex + 1);
            return;
        } else if (key < frame) {
            lo = mid;
            *pLeftFrame = key;
        } else {
            hi = mid;
            *pRightFrame = key;
        }
        mid = (lo + hi) / 2;
        key = detail::ReadCurveKeyFrame(pKeys, stride, mid);
    }
    *pIndex = lo;
    *pExact = 0;
}

static inline u16 GetByteCurveType(const u8* pValues, const u8* pType, u16& rStart) {
    rStart = *pValues;
    return *pType;
}

inline u16 GetRandomByteCurveType(s32 value, const u8* pType, u32& rStart) {
    rStart = static_cast<u32>(value);
    return *pType;
}

static inline u8 GetByteCurveEnd(const u8* pStart, const u8* pEnd, const u8* pType,
                                 u16& rStart, u8& rType) {
    rStart = *pStart;
    rType = *pType;
    rType = static_cast<u8>(rType);
    return *pEnd;
}

void AnimCurveExecuteU8(u8* pCmdList, u8* pTarget, u32 tick, u16 seed, u32 life) {
    u32 rstride;
    u32 flag;
    u32 fittingPeriod;
    u8 mask;
    u8 end;
    u32 stride;
    const u8* pKeys;
    const u8* pRanges;
    const u8* pRandomTable;
    u32 divL;
    u32 divH;
    const u8* pKeyData;
    u32 idx;
    u16 keyLo;
    u16 keyHi;

    if (pCmdList[0] == 0xAB) {
        AnimCurveExecuteBakedU8(pCmdList, pTarget, tick, life);
        return;
    }

    mask = pCmdList[3];
    const AnimCurveHeader* pHeader;
    const u8* pNameTable;
    f32 frameF32;
    u16 frame;
    BOOL exact;

    end = sHighestChannel[mask];
    u32 count = sChannelCount[mask];
    if (count == 0) {
        return;
    }

    stride = (count + 0xD) & ~1;
    detail::SplitCurveResource(pCmdList, &pHeader, &pKeys, &pRanges, &pRandomTable, &pNameTable);
    rstride = count * 2;

    u16 frameLength = pHeader->frameLength;
    divL = 0;

    if (frameLength <= 1) {
        frameF32 = 0.0f;
        divL = tick;
        frame = 0;
    } else {
        flag = pHeader->processFlag;

        if (!(flag & AnimCurveHeader::PROC_FLAG_INFLOOP) &&
            pHeader->loopCount <= 1) {

            if (!(flag & AnimCurveHeader::PROC_FLAG_FITTING)) {
                frame = tick;
                if (tick >= frameLength - 1) {
                    frame = frameLength - 1;
                }
                frameF32 = frame;
            } else {
                if (life == 1) {
                    frameF32 = frameLength - 1;
                } else {
                    frameF32 = tick * (static_cast<f32>(frameLength - 1) / (life - 1));
                    if (frameF32 > static_cast<f32>(frameLength - 1)) {
                        frameF32 = static_cast<f32>(frameLength - 1);
                    }
                }

                frame = frameF32;
            }
        } else if (!(flag & AnimCurveHeader::PROC_FLAG_FITTING)) {
            u32 period = frameLength - 1;
            divL = tick / period;

            if (!(flag & AnimCurveHeader::PROC_FLAG_TURN)) {
                if (!(flag & AnimCurveHeader::PROC_FLAG_INFLOOP) &&
                    divL >= pHeader->loopCount) {
                    frame = period;
                    divL = static_cast<u8>(pHeader->loopCount - 1);
                } else {
                    frame = tick - divL * period;
                }
            } else {
                if (!(flag & AnimCurveHeader::PROC_FLAG_INFLOOP) &&
                    divL >= pHeader->loopCount) {
                    if (pHeader->loopCount % 2 == 0) {
                        frame = 0;
                    } else {
                        frame = period;
                    }
                    divL = static_cast<u8>(pHeader->loopCount - 1);
                } else if (divL % 2 == 0) {
                    frame = tick - divL * period;
                } else {
                    frame = period * (divL + 1) - tick;
                }
            }

            frameF32 = frame;
        } else if (tick >= life - 1) {
            if (!(flag & AnimCurveHeader::PROC_FLAG_TURN) ||
                pHeader->loopCount % 2 != 0) {
                frame = static_cast<u16>(frameLength - 1);
            } else {
                frame = 0;
            }

            frameF32 = frame;
            divL = static_cast<u8>(pHeader->loopCount - 1);
        } else {
            fittingPeriod = pHeader->frameLength - 1;
            f32 fittingScale = pHeader->loopCount *
                               (static_cast<f32>(static_cast<int>(fittingPeriod)) / (life - 1));
            u32 fittingLoop = tick * fittingScale / static_cast<int>(fittingPeriod);
            divL = fittingLoop;
            f32 fittingTime = tick * fittingScale - fittingLoop * fittingPeriod;
            if (!(flag & AnimCurveHeader::PROC_FLAG_TURN)) {
                frameF32 = fittingTime;
            } else if (fittingLoop % 2 == 0) {
                frameF32 = fittingTime;
            } else {
                frameF32 = static_cast<f32>(static_cast<int>(fittingPeriod)) - fittingTime;
            }
            frame = frameF32;
        }
    }

    divH = divL;

    pKeyData = pKeys + 4;
    FindByteCurveKeys(&idx, &exact, &keyLo, &keyHi, frame, frameF32, (const u16*)pKeyData, stride, 0,
                ((const AnimCurveKey*)pKeys)->count - 1);

    if (exact) {
        const u8* pValues = pKeyData + idx * stride + 12;
        if (*(pValues - 10) == 0) {
            for (u16 bit = 1; bit <= end; bit = (u16)(bit << 1)) {
                if ((mask & bit) != 0) {
                    *pTarget = *pValues++;
                }
                pTarget++;
            }
            return;
        }

        u16 r = *(const u16*)pValues;
        u32 hash = HashByteCurveSeed(seed, pHeader->randomSeed, r, divL);
        u8 scale;
        u8 base;
        const detail::CurveRandomByte* pRange;
        if ((*(pValues - 10) & 2) == 0) {
            const u8* rangeAddress = pRanges + r * rstride;
            rangeAddress = static_cast<const u8*>(rangeAddress);
            pRange = reinterpret_cast<const detail::CurveRandomByte*>(rangeAddress + 4);
        } else {
            const u8* rangeAddress = pRandomTable + rstride * ((hash >> 16) % *(const u16*)pRandomTable);
            rangeAddress = static_cast<const u8*>(rangeAddress);
            pRange = reinterpret_cast<const detail::CurveRandomByte*>(rangeAddress + 4);
            hash = detail::NextCurveSeed(hash);
        }
        for (u16 bit = 1; bit <= end; bit = (u16)(bit << 1)) {
            if ((mask & bit) != 0) {
                base = pRange->mBase;
                scale = pRange->mRange;
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
    const u8* pValuesA;
    u32 t = (u32)(65536.0f * (frameF32 - (f32)keyLo)) / (keyHi - keyLo);
    const detail::ValueCurveKey* pRecordA = (const detail::ValueCurveKey*)(pKeyData + idx * stride);
    pValuesA = pRecordA->mData.mByte;
    const detail::ValueCurveKey* pRecordB = (const detail::ValueCurveKey*)(pKeyData + (idx + 1) * stride);
    const u8* pValuesB = pRecordB->mData.mByte;
    const u8* pInterpolation = pRecordA->mType;
    BOOL randA = *(reinterpret_cast<const u8*>(pValuesA) - 10) != 0;
    BOOL randB = *(reinterpret_cast<const u8*>(pValuesB) - 10) != 0;

    if (!randA && !randB) {
        for (u16 bit = 1; bit <= end; bit = (u16)(bit << 1)) {
            if ((mask & bit) != 0) {
                u16 startValue;
                u8 endValue;
                u8 interpolationType;
                endValue = GetByteCurveEnd(pValuesA, pValuesB, pInterpolation,
                                          startValue, interpolationType);
                *pTarget = InterpolateByteCurve(t, startValue, endValue, interpolationType);
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
        u32 hashA = HashByteCurveSeed(seed, pHeader->randomSeed, rA, divL);
        const detail::CurveRandomByte* pRangeA;
        if ((*(reinterpret_cast<const u8*>(pValuesA) - 10) & 2) == 0) {
            pRangeA = (const detail::CurveRandomByte*)(pRanges + rA * rstride + 4);
        } else {
            pRangeA = (const detail::CurveRandomByte*)(pRandomTable +
                                              rstride * ((hashA >> 16) % *(const u16*)pRandomTable) + 4);
            hashA = detail::NextCurveSeed(hashA);
        }
        for (u16 bit = 1; bit <= end; bit = (u16)(bit << 1)) {
            if ((mask & bit) != 0) {
                u8 scaleA;
                u8 baseA = pRangeA->mBase;
                scaleA = pRangeA->mRange;
                pRangeA++;
                s32 vA = baseA + scaleA * (s16)(hashA >> 16) / 32768;
                if (vA < 0) {
                    vA = 0;
                }
                if (vA > 0xFF) {
                    vA = 0xFF;
                }
                u32 preservedStart;
                u16 interpolationMode = GetRandomByteCurveType(vA, pInterpolation, preservedStart);
                *pTarget = InterpolateByteCurve(t,
                    static_cast<u8>(static_cast<u64>(static_cast<u32>(preservedStart))),
                    *pValuesB, interpolationMode);
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
        u32 hashB = HashByteCurveSeed(seed, pHeader->randomSeed, rB, divH);
        const detail::CurveRandomByte* pRangeB;
        if ((*(reinterpret_cast<const u8*>(pValuesB) - 10) & 2) == 0) {
            pRangeB = (const detail::CurveRandomByte*)(pRanges + rB * rstride + 4);
        } else {
            pRangeB = (const detail::CurveRandomByte*)(pRandomTable +
                                              rstride * ((hashB >> 16) % *(const u16*)pRandomTable) + 4);
            hashB = detail::NextCurveSeed(hashB);
        }
        for (u16 bit = 1; bit <= end; bit = (u16)(bit << 1)) {
            if ((mask & bit) != 0) {
                u8 scaleB;
                u8 baseB = pRangeB->mBase;
                scaleB = pRangeB->mRange;
                pRangeB++;
                s32 vB = baseB + scaleB * (s16)(hashB >> 16) / 32768;
                if (vB < 0) {
                    vB = 0;
                }
                if (vB > 0xFF) {
                    vB = 0xFF;
                }
                u16 startValue;
                u16 interpolationType;
                interpolationType = GetByteCurveType(pValuesA, pInterpolation, startValue);
                *pTarget = InterpolateByteCurve(t, startValue,
                    static_cast<u8>(static_cast<u64>(static_cast<u32>(vB))), interpolationType);
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

    u16 rA;
    u32 hashA;
    u16 rB;
    u32 hashB;
    const detail::CurveRandomByte* pRangeB;
    const detail::CurveRandomByte* pRangeA;
    rA = *(const u16*)pValuesA;
    hashA = HashByteCurveSeed(seed, pHeader->randomSeed, rA, divL);
    if ((*(reinterpret_cast<const u8*>(pValuesA) - 10) & 2) == 0) {
        const u8* rangeAddress = pRanges + rA * rstride;
        rangeAddress = static_cast<const u8*>(rangeAddress);
        pRangeA = reinterpret_cast<const detail::CurveRandomByte*>(rangeAddress + 4);
    } else {
        const u8* rangeAddress = pRandomTable + rstride * ((hashA >> 16) % *(const u16*)pRandomTable);
        rangeAddress = static_cast<const u8*>(rangeAddress);
        pRangeA = reinterpret_cast<const detail::CurveRandomByte*>(rangeAddress + 4);
        hashA = detail::NextCurveSeed(hashA);
    }
    rB = *(const u16*)pValuesB;
    hashB = HashByteCurveSeed(seed, pHeader->randomSeed, rB, divH);
    if ((*(reinterpret_cast<const u8*>(pValuesB) - 10) & 2) == 0) {
        const u8* rangeAddress = pRanges + rB * rstride;
        rangeAddress = static_cast<const u8*>(rangeAddress);
        pRangeB = reinterpret_cast<const detail::CurveRandomByte*>(rangeAddress + 4);
    } else {
        pRangeB = (const detail::CurveRandomByte*)(pRandomTable +
                                          rstride * ((hashB >> 16) % *(const u16*)pRandomTable) + 4);
        hashB = detail::NextCurveSeed(hashB);
    }
    s32 vA;
    for (u16 bit = 1; bit <= end; bit = (u16)(bit << 1)) {
        if ((mask & bit) != 0) {
            u8 scaleA;
            u8 baseA = pRangeA->mBase;
            scaleA = pRangeA->mRange;
            pRangeA++;
            vA = baseA + scaleA * (s16)(hashA >> 16) / 32768;
            if (vA < 0) {
                vA = 0;
            }
            if (vA > 0xFF) {
                vA = 0xFF;
            }
            u8 scaleB;
            u8 baseB;
            u32 randomMode;
            randomMode = pHeader->processFlag & 4;
            if (randomMode == 0) {
                hashA = detail::NextCurveSeed(hashA);
            }
            baseB = pRangeB->mBase;
            scaleB = pRangeB->mRange;
            pRangeB++;
            s32 vB = baseB + scaleB * (s16)(hashB >> 16) / 32768;
            if (vB < 0) {
                vB = 0;
            }
            if (vB > 0xFF) {
                vB = 0xFF;
            }
            if (randomMode == 0) {
                hashB = detail::NextCurveSeed(hashB);
            }
            u32 preservedStart;
            u16 interpolationMode = GetRandomByteCurveType(vA, pInterpolation, preservedStart);
            *pTarget = InterpolateByteCurve(t,
                static_cast<u8>(static_cast<u64>(static_cast<u32>(preservedStart))),
                static_cast<u8>(static_cast<u64>(static_cast<u32>(vB))), interpolationMode);
        } else if ((pHeader->processFlag & 4) == 0) {
            hashA = detail::NextCurveSeed(hashA);
            hashB = detail::NextCurveSeed(hashB);
        }
        pInterpolation++;
        pTarget++;
    }
}

static inline f32 CalcFittingFloatTime(const AnimCurveHeader* pHeader, u8 flag,
                                       u32 tick, u32 life, u32& rLoop, u32& periodProduct,
                                       u32& fittedLoopCount, u32& fittingPeriod, u32 frameLength) {
    fittingPeriod = frameLength - 1;
    fittedLoopCount = pHeader->loopCount;
    fittedLoopCount = static_cast<u32>(fittedLoopCount);
    f32 scale = fittedLoopCount *
                (static_cast<f32>(static_cast<int>(fittingPeriod)) / (life - 1));

    u32 loop = tick * scale / static_cast<int>(fittingPeriod);
    rLoop = loop;

    periodProduct = loop;
    periodProduct *= fittingPeriod;
    f32 time = tick * scale - periodProduct;

    if (!(flag & AnimCurveHeader::PROC_FLAG_TURN)) {
        return time;
    }

    if (loop % 2 == 0) {
        return time;
    }

    return static_cast<f32>(static_cast<int>(fittingPeriod)) - time;
}

static inline f32 CalcFloatCurveTime(const AnimCurveHeader* pHeader, u32 tick, u32 life,
                                    u16& rFrame, u32& rLoop, u32& periodProduct,
                                    u32& fittedLoopCount, u32& fittingPeriod, u32& frameLength) {
    frameLength = pHeader->frameLength;
    rLoop = 0;
    f32 time;

    if (frameLength <= 1) {
        time = 0.0f;
        rLoop = tick;
        rFrame = 0;
    } else {
        u32 rFlag = pHeader->processFlag;

        if (!(rFlag & AnimCurveHeader::PROC_FLAG_INFLOOP) &&
            pHeader->loopCount <= 1) {

            if (!(rFlag & AnimCurveHeader::PROC_FLAG_FITTING)) {
                rFrame = tick;
                if (tick >= frameLength - 1) {
                    rFrame = frameLength - 1;
                }
                time = rFrame;
            } else {
                if (life == 1) {
                    time = static_cast<s32>(frameLength - 1);
                } else {
                    time = tick * (static_cast<f32>(static_cast<s32>(frameLength - 1)) / (life - 1));
                    if (time > static_cast<f32>(static_cast<s32>(frameLength - 1))) {
                        time = static_cast<f32>(static_cast<s32>(frameLength - 1));
                    }
                }

                rFrame = time;
            }
        } else if (!(rFlag & AnimCurveHeader::PROC_FLAG_FITTING)) {
            u32 firstPeriod = frameLength - 1;
            rLoop = tick / firstPeriod;

            if (!(rFlag & AnimCurveHeader::PROC_FLAG_TURN)) {
                if (!(rFlag & AnimCurveHeader::PROC_FLAG_INFLOOP) &&
                    rLoop >= pHeader->loopCount) {
                    rFrame = firstPeriod;
                    rLoop = static_cast<u8>(pHeader->loopCount - 1);
                } else {
                    rFrame = tick - rLoop * firstPeriod;
                }
            } else {
                if (!(rFlag & AnimCurveHeader::PROC_FLAG_INFLOOP) &&
                    rLoop >= pHeader->loopCount) {
                    if (pHeader->loopCount % 2 == 0) {
                        rFrame = 0;
                    } else {
                        rFrame = firstPeriod;
                    }
                    rLoop = static_cast<u8>(pHeader->loopCount - 1);
                } else if (rLoop % 2 == 0) {
                    rFrame = tick - rLoop * firstPeriod;
                } else {
                    rFrame = firstPeriod * (rLoop + 1) - tick;
                }
            }

            time = rFrame;
        } else if (tick >= life - 1) {
            if (!(rFlag & AnimCurveHeader::PROC_FLAG_TURN) ||
                pHeader->loopCount % 2 != 0) {
                rFrame = static_cast<u16>(frameLength - 1);
            } else {
                rFrame = 0;
            }

            time = rFrame;
            rLoop = static_cast<u8>(pHeader->loopCount - 1);
        } else {
            time = CalcFittingFloatTime(pHeader, rFlag, tick, life, rLoop, periodProduct,
                                        fittedLoopCount, fittingPeriod, frameLength);
            rFrame = time;
        }
    }

    return time;
}

struct FloatCurveKeyMatch {
    BOOL mValue;

    operator BOOL() const {
        return mValue;
    }
};

static inline u32 HashFloatComposite(u32 seedPart, u16 randomIndex, u32 loop) {
    detail::CurveRandomSeed h;
    h.mWord = (u32)loop * 0x7B929 + ((u32)randomIndex * 0x371097E7 + seedPart) + 0x4BF53;
    h.mByte[2] ^= h.mByte[3];
    h.mByte[1] ^= h.mByte[2];
    h.mByte[0] ^= h.mByte[1];
    return h.mWord;
}

static inline void FindFloatCurveKeys(u32* pIndex, FloatCurveKeyMatch* pExact,
                                      u16* pLeftFrame, u16* pRightFrame, s32 frame,
                                      f32 frameF32, const u16* pKeys, u32 stride,
                                      s32& lo, s32 hi) {
    lo = 0;
    s32 mid = (lo + hi) / 2;
    BOOL atTarget = nw4r::ef::FAbs((f32)frame - frameF32) < 1.1920929e-07f;

    u16 keyLo = detail::ReadCurveKeyFrame(pKeys, stride, lo);
    *pLeftFrame = keyLo;
    if (frame < keyLo) {
        *pIndex = lo;
        pExact->mValue = 1;
        return;
    }
    if (frame == keyLo) {
        *pIndex = lo;
        if (lo == hi) {
            pExact->mValue = 1;
            return;
        }
        pExact->mValue = atTarget;
        if (atTarget == 0) {
            *pRightFrame = detail::ReadCurveKeyFrame(pKeys, stride, lo + 1);
        }
        return;
    }

    u16 keyHi = detail::ReadCurveKeyFrame(pKeys, stride, hi);
    *pRightFrame = keyHi;
    if (keyHi <= frame) {
        *pIndex = hi;
        pExact->mValue = 1;
        return;
    }

    s32 key = detail::ReadCurveKeyFrame(pKeys, stride, mid);
    while (lo < mid) {
        if (frame == key) {
            *pIndex = mid;
            pExact->mValue = atTarget;
            if (atTarget != 0) {
                return;
            }
            *pLeftFrame = key;
            *pRightFrame = detail::ReadCurveKeyFrame(pKeys, stride, *pIndex + 1);
            return;
        } else if (key < frame) {
            lo = mid;
            *pLeftFrame = key;
        } else {
            hi = mid;
            *pRightFrame = key;
        }
        mid = (lo + hi) / 2;
        key = detail::ReadCurveKeyFrame(pKeys, stride, mid);
    }
    *pIndex = lo;
    pExact->mValue = 0;
}

struct FloatCurveValue {
    f32 mValue;

    FloatCurveValue(f32 value) : mValue(value) {}

    operator f32() const {
        return mValue;
    }
};

static inline f32 InterpolateFloatCurve(f32 t, f32 a, f32 b, u8 type) {
    if (a == b) {
        return a;
    }
    switch (type & 3) {
    case 0:
        return a + t * (b - a);
    case 1:
        if (type == 1) {
            FloatCurveValue w = 3.0f - 2.0f * t;
            return a + t * (t * (w.mValue * (b - a)));
        } else {
            f32 c0 = ((type >> 3) & 1) == 0 ? 0.0f : 1.5f;
            f32 c1 = ((type >> 2) & 1) == 0 ? 0.0f : 1.5f;
            FloatCurveValue s = t * (b - a);
            FloatCurveValue u = t * (t * (c0 + c1 - 2.0f));
            FloatCurveValue v = t * (3.0f + (-2.0f * c0 - c1));
            return a + s * (c0 + (u + v));
        }
    case 2:
        return a;
    default:
        return 0.0f;
    }
}

static inline const f32* GetFloatCurveValues(const f32* pValues, u8 flags, BOOL& rRandom) {
    rRandom = flags != 0;
    return pValues;
}

void AnimCurveExecuteF32(u8* pCmdList, f32* pTarget, u32 inputTick, u16 seed, u32 life) {
    u32 periodProduct;
    u32 fittedLoopCount;
    u32 fittingPeriod;
    u32 frameLength;
    u8 mask;
    const AnimCurveHeader* pHeader;
    const u8* pKeys;
    const u8* pRanges;
    const u8* pRandomTable;
    u32 tick;
    const u8* pNameTable;
    f32 frameF32;

    s32 searchLo;
    u32 rstride;
    u32 stride;
    const u8* pKeyData;
    u32 idx;
    u32 divL;
    u32 divH;
    u8 end;
    u32 count;
    u16 keyLo;
    u16 keyHi;
    u16 frame;
    FloatCurveKeyMatch exact;
    tick = reinterpret_cast<u32>(reinterpret_cast<void*>(inputTick));

    if (pCmdList[0] == 0xAB) {
        AnimCurveExecuteBakedF32(pCmdList, pTarget, tick, life);
        return;
    }

    mask = pCmdList[3];

    detail::SplitCurveResource(pCmdList, &pHeader, &pKeys, &pRanges, &pRandomTable, &pNameTable);
    frameF32 = CalcFloatCurveTime(pHeader, tick, life, frame, divL, periodProduct,
                                 fittedLoopCount, fittingPeriod, frameLength);
    divH = divL;

    end = sHighestChannel[mask];
    count = sChannelCount[mask];
    if (count == 0) {
        return;
    }

    stride = count * 4 + 0xC;
    rstride = count * 8;
    pKeyData = pKeys + 4;
    FindFloatCurveKeys(&idx, &exact, &keyLo, &keyHi, frame, frameF32,
                      (const u16*)pKeyData, stride, searchLo,
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
            const void* pRangeAddress;
            pRangeAddress = pRanges + r * rstride;
            pRangeAddress = static_cast<const void*>(pRangeAddress);
            pRange = reinterpret_cast<const detail::CurveRandomRange*>(
                static_cast<const u8*>(pRangeAddress) + 4);
        } else {
            const void* pRangeAddress;
            pRangeAddress = pRandomTable +
                            rstride * ((hash >> 16) % *(const u16*)pRandomTable);
            pRangeAddress = static_cast<const void*>(pRangeAddress);
            pRange = reinterpret_cast<const detail::CurveRandomRange*>(
                static_cast<const u8*>(pRangeAddress) + 4);
            hash = detail::NextCurveSeed(hash);
        }
        for (u16 bit = 1; bit <= end; bit = (u16)(bit << 1)) {
            if ((mask & bit) != 0) {
                f32 scale;
                f32 base = pRange->mBase;
                scale = pRange->mRange;
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
    u8 flagsA;
    u8 flagsB;
    FloatCurveValue t((frameF32 - (f32)keyLo) / (f32)(keyHi - keyLo));
    const detail::ValueCurveKey* pRecordA = (const detail::ValueCurveKey*)(pKeyData + idx * stride);
    const f32* pValuesA = pRecordA->mData.mValue;
    const detail::ValueCurveKey* pRecordB = (const detail::ValueCurveKey*)(pKeyData + (idx + 1) * stride);
    const f32* pValuesB = pRecordB->mData.mValue;
    const u8* pInterpolation = pRecordA->mType;
    flagsA = pRecordA->mFlags;
    flagsB = *(reinterpret_cast<const u8*>(pValuesB) - 10);
    BOOL randA = flagsA != 0;
    BOOL randB;
    pValuesB = GetFloatCurveValues(pValuesB, flagsB, randB);

    if (!randA && !randB) {
        for (u16 bit = 1; bit <= end; bit = (u16)(bit << 1)) {
            if ((mask & bit) != 0) {
                *pTarget = InterpolateFloatCurve(t, *pValuesA, *pValuesB, *pInterpolation);
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
        u16 interpolationType;
        const detail::CurveRandomRange* pRange;
        if ((flagsA & 2) == 0) {
            const void* pRangeAddress;
            pRangeAddress = pRanges + r * rstride;
            pRangeAddress = static_cast<const void*>(pRangeAddress);
            pRange = reinterpret_cast<const detail::CurveRandomRange*>(
                static_cast<const u8*>(pRangeAddress) + 4);
        } else {
            const void* pRangeAddress;
            pRangeAddress = pRandomTable +
                            rstride * ((hash >> 16) % *(const u16*)pRandomTable);
            pRangeAddress = static_cast<const void*>(pRangeAddress);
            pRange = reinterpret_cast<const detail::CurveRandomRange*>(
                static_cast<const u8*>(pRangeAddress) + 4);
            hash = detail::NextCurveSeed(hash);
        }
        for (u16 bit = 1; bit <= end; bit = (u16)(bit << 1)) {
            if ((mask & bit) != 0) {
                f32 scale;
                f32 base = pRange->mBase;
                scale = pRange->mRange;
                pRange++;
                f32 a = base + scale * (f32)(hash >> 16);
                if ((pHeader->processFlag & 4) == 0) {
                    hash = detail::NextCurveSeed(hash);
                }
                interpolationType = *pInterpolation;
                interpolationType = static_cast<u16>(interpolationType);
                *pTarget = InterpolateFloatCurve(t, a, *pValuesB, interpolationType);
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
        if ((flagsB & 2) == 0) {
            const void* pRangeAddress;
            pRangeAddress = pRanges + r * rstride;
            pRangeAddress = static_cast<const void*>(pRangeAddress);
            pRange = reinterpret_cast<const detail::CurveRandomRange*>(
                static_cast<const u8*>(pRangeAddress) + 4);
        } else {
            const void* pRangeAddress;
            pRangeAddress = pRandomTable +
                            rstride * ((hash >> 16) % *(const u16*)pRandomTable);
            pRangeAddress = static_cast<const void*>(pRangeAddress);
            pRange = reinterpret_cast<const detail::CurveRandomRange*>(
                static_cast<const u8*>(pRangeAddress) + 4);
            hash = detail::NextCurveSeed(hash);
        }
        for (u16 bit = 1; bit <= end; bit = (u16)(bit << 1)) {
            if ((mask & bit) != 0) {
                f32 scale;
                f32 base = pRange->mBase;
                scale = pRange->mRange;
                pRange++;
                f32 b = base + scale * (f32)(hash >> 16);
                if ((pHeader->processFlag & 4) == 0) {
                    hash = detail::NextCurveSeed(hash);
                }
                *pTarget = InterpolateFloatCurve(t, *pValuesA, b, *pInterpolation);
                pValuesA++;
            }
            pInterpolation++;
            pTarget++;
        }
        return;
    }

    u32 seedPart;
    u16 rA;
    u32 hashA;
    u32 rB;
    u32 hashB;
    const detail::CurveRandomRange* pRangeB;
    const detail::CurveRandomRange* pRangeA;
    seedPart = (u32)seed * 0x3F81F635 + (u32)pHeader->randomSeed * 0x30A74193;
    rA = *(const u16*)pValuesA;
    hashA = HashFloatComposite(seedPart, rA, divL);
    if ((flagsA & 2) == 0) {
        const void* pRangeAddress;
        pRangeAddress = pRanges + rA * rstride;
        pRangeAddress = static_cast<const void*>(pRangeAddress);
        pRangeA = reinterpret_cast<const detail::CurveRandomRange*>(
            static_cast<const u8*>(pRangeAddress) + 4);
    } else {
        const void* pRangeAddress;
        pRangeAddress = pRandomTable +
                        rstride * ((hashA >> 16) % *(const u16*)pRandomTable);
        pRangeAddress = static_cast<const void*>(pRangeAddress);
        pRangeA = reinterpret_cast<const detail::CurveRandomRange*>(
            static_cast<const u8*>(pRangeAddress) + 4);
        hashA = detail::NextCurveSeed(hashA);
    }
    rB = *(const u16*)pValuesB;
    hashB = HashFloatComposite(seedPart, rB, divH);
    if ((flagsB & 2) == 0) {
        const void* pRangeAddress;
        pRangeAddress = pRanges + rB * rstride;
        pRangeAddress = static_cast<const void*>(pRangeAddress);
        pRangeB = reinterpret_cast<const detail::CurveRandomRange*>(
            static_cast<const u8*>(pRangeAddress) + 4);
    } else {
        const void* pRangeAddress;
        pRangeAddress = pRandomTable +
                        rstride * ((hashB >> 16) % *(const u16*)pRandomTable);
        pRangeAddress = static_cast<const void*>(pRangeAddress);
        pRangeB = reinterpret_cast<const detail::CurveRandomRange*>(
            static_cast<const u8*>(pRangeAddress) + 4);
        hashB = detail::NextCurveSeed(hashB);
    }
    for (u16 bit = 1; bit <= end; bit = (u16)(bit << 1)) {
        if ((mask & bit) != 0) {
            f32 scaleA;
            f32 baseA = pRangeA->mBase;
            scaleA = pRangeA->mRange;
            pRangeA++;
            f32 scaleB;
            f32 baseB;
            f32 b;
            f32 a = baseA + scaleA * (f32)(hashA >> 16);
            if ((pHeader->processFlag & 4) == 0) {
                hashA = detail::NextCurveSeed(hashA);
            }
            baseB = pRangeB->mBase;
            scaleB = pRangeB->mRange;
            pRangeB++;
            b = baseB + scaleB * (f32)(hashB >> 16);
            if ((pHeader->processFlag & 4) == 0) {
                hashB = detail::NextCurveSeed(hashB);
            }
            *pTarget = InterpolateFloatCurve(t, a, b, *pInterpolation);
        }
        pInterpolation++;
        pTarget++;
    }
}

static inline u32 HashRotationCurveSeed(u16 particleSeed, u16 curveSeed, u16 randomIndex, u32 loop) {
    detail::CurveRandomSeed h;
    h.mWord = (u32)particleSeed * 0x3F81F635 + (u32)curveSeed * 0x30A74193 +
              (u32)randomIndex * 0x371097E7 + (u32)loop * 0x7B929 + 0x4BF53;
    h.mByte[2] ^= h.mByte[3];
    h.mByte[1] ^= h.mByte[2];
    h.mByte[0] ^= h.mByte[1];
    return h.mWord;
}

static inline f32 CalcRotationFittingTime(const AnimCurveHeader* pHeader, u8 flag,
                                      u32 tick, u32 life, u32& rLoop, u32& rPeriod,
                                      u32& rLoopPeriod, u32& rFittingCount) {
    rPeriod = pHeader->frameLength - 1;
    rFittingCount = pHeader->loopCount;
    rFittingCount = static_cast<u32>(rFittingCount);
    f32 scale = rFittingCount *
                (static_cast<f32>(static_cast<int>(rPeriod)) / (life - 1));

    u32 loop = tick * scale / static_cast<int>(rPeriod);
    rLoop = loop;

    rLoopPeriod = loop;
    rLoopPeriod *= rPeriod;
    f32 time = tick * scale - rLoopPeriod;

    if (!(flag & AnimCurveHeader::PROC_FLAG_TURN)) {
        return time;
    }

    if (loop % 2 == 0) {
        return time;
    }

    return static_cast<f32>(static_cast<int>(rPeriod)) - time;
}

static inline f32 CalcRotationCurveTime(const AnimCurveHeader* pHeader, u32 tick, u32 life,
                                        u16& rFrame, u32& rLoop, u32& rFittingPeriod,
                                        u32& rLoopPeriod, u32& rFittingCount) {
    u16 frameLength = pHeader->frameLength;
    rLoop = 0;
    f32 time;

    if (frameLength <= 1) {
        time = 0.0f;
        rLoop = tick;
        rFrame = 0;
    } else {
        u8 timeFlag = pHeader->processFlag;

        if (!(timeFlag & AnimCurveHeader::PROC_FLAG_INFLOOP) &&
            pHeader->loopCount <= 1) {

            if (!(timeFlag & AnimCurveHeader::PROC_FLAG_FITTING)) {
                rFrame = tick;
                if (tick >= frameLength - 1) {
                    rFrame = frameLength - 1;
                }
                time = rFrame;
            } else {
                if (life == 1) {
                    time = frameLength - 1;
                } else {
                    time = tick * (static_cast<f32>(frameLength - 1) / (life - 1));
                    if (time > static_cast<f32>(frameLength - 1)) {
                        time = static_cast<f32>(frameLength - 1);
                    }
                }

                rFrame = time;
            }
        } else if (!(timeFlag & AnimCurveHeader::PROC_FLAG_FITTING)) {
            u32 period = frameLength - 1;
            rLoop = tick / period;

            if (!(timeFlag & AnimCurveHeader::PROC_FLAG_TURN)) {
                if (!(timeFlag & AnimCurveHeader::PROC_FLAG_INFLOOP) &&
                    rLoop >= pHeader->loopCount) {
                    rFrame = period;
                    rLoop = static_cast<u8>(pHeader->loopCount - 1);
                } else {
                    rFrame = tick - rLoop * period;
                }
            } else {
                if (!(timeFlag & AnimCurveHeader::PROC_FLAG_INFLOOP) &&
                    rLoop >= pHeader->loopCount) {
                    if (pHeader->loopCount % 2 == 0) {
                        rFrame = 0;
                    } else {
                        rFrame = period;
                    }
                    rLoop = static_cast<u8>(pHeader->loopCount - 1);
                } else if (rLoop % 2 == 0) {
                    rFrame = tick - rLoop * period;
                } else {
                    rFrame = period * (rLoop + 1) - tick;
                }
            }

            time = rFrame;
        } else if (tick >= life - 1) {
            if (!(timeFlag & AnimCurveHeader::PROC_FLAG_TURN) ||
                pHeader->loopCount % 2 != 0) {
                rFrame = static_cast<u16>(frameLength - 1);
            } else {
                rFrame = 0;
            }

            time = rFrame;
            rLoop = static_cast<u8>(pHeader->loopCount - 1);
        } else {
            time = CalcRotationFittingTime(pHeader, timeFlag, tick, life, rLoop, rFittingPeriod, rLoopPeriod, rFittingCount);
            rFrame = time;
        }
    }

    return time;
}

struct RotationCurveValue {
    f32 mValue;

    RotationCurveValue(f32 value) : mValue(value) {}
    RotationCurveValue(const RotationCurveValue& rOther) : mValue(rOther.mValue) {}

    operator f32() const {
        return mValue;
    }
};

static inline RotationCurveValue CalcRotationBlendTime(f32 time, u16 left, u16 right) {
    return RotationCurveValue((time - (f32)left) / (f32)(right - left));
}

static inline f32 InterpolateRotationCurve(f32 t, f32 a, f32 b, u8 type) {
    if (a == b) {
        return a;
    }
    switch (type & 3) {
    case 0:
        return a + t * (b - a);
    case 1:
        if (type == 1) {
            RotationCurveValue w = 3.0f - 2.0f * t;
            return a + t * (t * (w.mValue * (b - a)));
        } else {
            f32 c0 = ((type >> 3) & 1) == 0 ? 0.0f : 1.5f;
            f32 c1 = ((type >> 2) & 1) == 0 ? 0.0f : 1.5f;
            RotationCurveValue s = t * (b - a);
            RotationCurveValue u = t * (t * (c0 + c1 - 2.0f));
            RotationCurveValue v = t * (3.0f + (-2.0f * c0 - c1));
            return a + s * (c0 + (u + v));
        }
    case 2:
        return a;
    default:
        return 0.0f;
    }
}

void AnimCurveExecuteRotate(u8* pCommandList, f32* pTarget, u32 inputTick, u16 seed, u32 life) {
    u8* pCmdList = reinterpret_cast<u8*>(reinterpret_cast<u32>(pCommandList));

    u32 loopPeriod;
    u32 fittingCount;
    u32 fittingPeriod;
    u8 mask;
    const u8* pKeys;
    const u8* pRanges;
    const u8* pRandomTable;
    u32 rstride;
    u32 stride;
    const u8* pKeyData;
    u32 divL;
    u32 divH;
    u8 end;
    u32 pairBytes;
    u32 idx;
    u16 keyLo;
    u16 keyHi;
    u32 tick = inputTick;

    mask = pCmdList[3];
    const AnimCurveHeader* pHeader;
    const u8* pNameTable;
    f32 frameF32;
    u16 frame;
    BOOL exact;

    detail::SplitCurveResource(pCmdList, &pHeader, &pKeys, &pRanges, &pRandomTable, &pNameTable);
    frameF32 = CalcRotationCurveTime(pHeader, tick, life, frame, divL, fittingPeriod, loopPeriod, fittingCount);
    divH = divL;

    end = sHighestChannel[mask];
    u32 count = sChannelCount[mask];
    if (count == 0) {
        return;
    }

    stride = count * 4 + 0xC;
    pairBytes = count * 8;
    rstride = pairBytes + 4;
    pKeyData = pKeys + 4;
    detail::FindCurveKeys(&idx, &exact, &keyLo, &keyHi, frame, frameF32, (const u16*)pKeyData, stride, 0,
                ((const AnimCurveKey*)pKeys)->count - 1);

    if (exact) {
        const detail::ValueCurveKey* pRecord = (const detail::ValueCurveKey*)(pKeyData + idx * stride);
        const f32* pValues = pRecord->mData.mValue;
        if (*(reinterpret_cast<const u8*>(pValues) - 10) == 0) {
            for (u16 bit = 1; bit <= end; bit = (u16)(bit << 1)) {
                if ((mask & bit) != 0) {
                    *pTarget = *pValues++;
                }
                pTarget++;
            }
            return;
        }

        u16 r = *(const u16*)pValues;
        u32 hash = HashRotationCurveSeed(seed, pHeader->randomSeed, r, divL);
        const detail::CurveRandomRange* pRange;
        if ((*(reinterpret_cast<const u8*>(pValues) - 10) & 2) == 0) {
            const u8* pRangeData = pRanges + r * rstride;
            pRangeData = static_cast<const u8*>(pRangeData);
            pRange = reinterpret_cast<const detail::CurveRandomRange*>(pRangeData + 4);
        } else {
            const u8* pRangeData = pRandomTable +
                                               rstride * ((hash >> 16) % *(const u16*)pRandomTable);
                                               pRangeData = static_cast<const u8*>(pRangeData);
            pRange = reinterpret_cast<const detail::CurveRandomRange*>(pRangeData + 4);
            hash = detail::NextCurveSeed(hash);
        }
        BOOL negate = FALSE;
        if (*reinterpret_cast<const u8*>(reinterpret_cast<u32>(pRange) + pairBytes) != 0) {
            if ((hash & 0x10000) == 0) {
                negate = TRUE;
            }
            hash = detail::NextCurveSeed(hash);
        }
        for (u16 bit = 1; bit <= end; bit = (u16)(bit << 1)) {
            if ((mask & bit) != 0) {
                f32 scale;
                f32 base = pRange->mBase;
                scale = pRange->mRange;
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
    u8 flagsA;
    u8 flagsB;
    RotationCurveValue t = CalcRotationBlendTime(frameF32, keyLo, keyHi);
    const detail::ValueCurveKey* pRecordA = (const detail::ValueCurveKey*)(pKeyData + idx * stride);
    const f32* pValuesA = pRecordA->mData.mValue;
    const detail::ValueCurveKey* pRecordB = (const detail::ValueCurveKey*)(pKeyData + (idx + 1) * stride);
    const f32* pValuesB = pRecordB->mData.mValue;
    const u8* pInterpolation = pRecordA->mType;
    flagsA = pRecordA->mFlags;
    flagsB = *(reinterpret_cast<const u8*>(pValuesB) - 10);
    bool randA = flagsA != 0;
    BOOL randB = flagsB != 0;

    if (!randA && !randB) {
        for (u16 bit = 1; bit <= end; bit = (u16)(bit << 1)) {
            if ((mask & bit) != 0) {
                *pTarget = InterpolateRotationCurve(t.mValue, *pValuesA, *pValuesB, *pInterpolation);
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
        u32 hashA = HashRotationCurveSeed(seed, pHeader->randomSeed, rA, divL);
        const detail::CurveRandomRange* pRangeA;
        if ((flagsA & 2) == 0) {
            const u8* pRangeData = pRanges + rA * rstride;
            pRangeData = static_cast<const u8*>(pRangeData);
            pRangeA = reinterpret_cast<const detail::CurveRandomRange*>(pRangeData + 4);
        } else {
            const u8* pRangeData = pRandomTable +
                                               rstride * ((hashA >> 16) % *(const u16*)pRandomTable);
                                               pRangeData = static_cast<const u8*>(pRangeData);
            pRangeA = reinterpret_cast<const detail::CurveRandomRange*>(pRangeData + 4);
            hashA = detail::NextCurveSeed(hashA);
        }
        BOOL negateA = FALSE;
        if (*reinterpret_cast<const u8*>(reinterpret_cast<u32>(pRangeA) + pairBytes) != 0) {
            if ((hashA & 0x10000) == 0) {
                negateA = TRUE;
            }
            hashA = detail::NextCurveSeed(hashA);
        }
        for (u16 bit = 1; bit <= end; bit = (u16)(bit << 1)) {
            if ((mask & bit) != 0) {
                f32 scaleA;
                f32 baseA = pRangeA->mBase;
                scaleA = pRangeA->mRange;
                pRangeA++;
                f32 vA = baseA + scaleA * (f32)(hashA >> 16);
                if (negateA) {
                    vA = -vA;
                }
                *pTarget = InterpolateRotationCurve(t.mValue, vA, *pValuesB, *pInterpolation);
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
        u32 hashB = HashRotationCurveSeed(seed, pHeader->randomSeed, rB, divH);
        const detail::CurveRandomRange* pRangeB;
        if ((flagsB & 2) == 0) {
            const u8* pRangeData = pRanges + rB * rstride;
            pRangeData = static_cast<const u8*>(pRangeData);
            pRangeB = reinterpret_cast<const detail::CurveRandomRange*>(pRangeData + 4);
        } else {
            const u8* pRangeData = pRandomTable +
                                               rstride * ((hashB >> 16) % *(const u16*)pRandomTable);
                                               pRangeData = static_cast<const u8*>(pRangeData);
            pRangeB = reinterpret_cast<const detail::CurveRandomRange*>(pRangeData + 4);
            hashB = detail::NextCurveSeed(hashB);
        }
        BOOL negateB = FALSE;
        if (*reinterpret_cast<const u8*>(reinterpret_cast<u32>(pRangeB) + pairBytes) != 0) {
            if ((hashB & 0x10000) == 0) {
                negateB = TRUE;
            }
            hashB = detail::NextCurveSeed(hashB);
        }
        for (u16 bit = 1; bit <= end; bit = (u16)(bit << 1)) {
            if ((mask & bit) != 0) {
                f32 scaleB;
                f32 baseB = pRangeB->mBase;
                scaleB = pRangeB->mRange;
                pRangeB++;
                f32 vB = baseB + scaleB * (f32)(hashB >> 16);
                if (negateB) {
                    vB = -vB;
                }
                *pTarget = InterpolateRotationCurve(t.mValue, *pValuesA, vB, *pInterpolation);
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
    u32 hashA = HashRotationCurveSeed(seed, pHeader->randomSeed, rA, divL);
    const detail::CurveRandomRange* pRangeA;
    if ((flagsA & 2) == 0) {
        const u8* pRangeData = pRanges + rA * rstride;
        pRangeData = static_cast<const u8*>(pRangeData);
        pRangeA = reinterpret_cast<const detail::CurveRandomRange*>(pRangeData + 4);
    } else {
        const u8* pRangeData = pRandomTable +
                                           rstride * ((hashA >> 16) % *(const u16*)pRandomTable);
                                           pRangeData = static_cast<const u8*>(pRangeData);
        pRangeA = reinterpret_cast<const detail::CurveRandomRange*>(pRangeData + 4);
        hashA = detail::NextCurveSeed(hashA);
    }
    u16 rB = *(const u16*)pValuesB;
    u32 hashB = HashRotationCurveSeed(seed, pHeader->randomSeed, rB, divH);
    const detail::CurveRandomRange* pRangeB;
    if ((flagsB & 2) == 0) {
        const u8* pRangeData = pRanges + rB * rstride;
        pRangeData = static_cast<const u8*>(pRangeData);
        pRangeB = reinterpret_cast<const detail::CurveRandomRange*>(pRangeData + 4);
    } else {
        const u8* pRangeData = pRandomTable +
                                           rstride * ((hashB >> 16) % *(const u16*)pRandomTable);
                                           pRangeData = static_cast<const u8*>(pRangeData);
        pRangeB = reinterpret_cast<const detail::CurveRandomRange*>(pRangeData + 4);
        hashB = detail::NextCurveSeed(hashB);
    }
    BOOL negateA = FALSE;
    if (*reinterpret_cast<const u8*>(reinterpret_cast<u32>(pRangeA) + pairBytes) != 0) {
        if ((hashA & 0x10000) == 0) {
            negateA = TRUE;
        }
        hashA = detail::NextCurveSeed(hashA);
    }
    BOOL negateB = FALSE;
    if (*reinterpret_cast<const u8*>(reinterpret_cast<u32>(pRangeB) + pairBytes) != 0) {
        if ((hashB & 0x10000) == 0) {
            negateB = TRUE;
        }
        hashB = detail::NextCurveSeed(hashB);
    }
    for (u16 bit = 1; bit <= end; bit = (u16)(bit << 1)) {
        if ((mask & bit) != 0) {
            f32 scaleA;
            f32 baseA;
            f32 scaleB;
            f32 baseB;
            f32 vB;
            f32 vA;
            baseA = pRangeA->mBase;
            scaleA = pRangeA->mRange;
            pRangeA++;
            vA = baseA + scaleA * (f32)(hashA >> 16);
            if (negateA) {
                vA = -vA;
            }
            baseB = pRangeB->mBase;
            scaleB = pRangeB->mRange;
            pRangeB++;
            vB = baseB + scaleB * (f32)(hashB >> 16);
            if (negateB) {
                vB = -vB;
            }
            *pTarget = InterpolateRotationCurve(t.mValue, vA, vB, *pInterpolation);
        }
        pInterpolation++;
        pTarget++;
        if ((pHeader->processFlag & 4) == 0) {
            hashA = detail::NextCurveSeed(hashA);
            hashB = detail::NextCurveSeed(hashB);
        }
    }
}

void AnimCurveExecuteTexture(u8* pCommandList, Particle* pParticle, u32 tick, u16 seed, u32 life) {
    u8* pCmdList = reinterpret_cast<u8*>(reinterpret_cast<u32>(pCommandList));

    u32 resDiv;
    s32 idx;
    detail::TextureCurveKey keyValue;
    u16 frame;
    u32 ch;
    TextureData** pChannel;

    const AnimCurveHeader* pHeader = reinterpret_cast<const AnimCurveHeader*>(pCmdList);
    const u8* pKeys = pCmdList + sizeof(AnimCurveHeader);
    const u8* pRanges = pKeys + pHeader->keyTable;
    const u8* pRandomTable = pRanges + pHeader->rangeTable;
    const u8* pNameTable = pRandomTable + pHeader->randomTable;
    do {
        u32 flag = pHeader->processFlag;
        u32 fittingPeriod;
        u32 infiniteFlag = flag & AnimCurveHeader::PROC_FLAG_INFLOOP;
        resDiv = 0;

        if (!infiniteFlag &&
            pHeader->loopCount <= 1) {

            if (!(flag & AnimCurveHeader::PROC_FLAG_FITTING)) {
                if (tick >= pHeader->frameLength) {
                    frame = pHeader->frameLength;
                    break;
                }
                frame = tick;
                break;
            }

            frame = pHeader->frameLength * tick / life;
            break;
        }

        if (!(flag & AnimCurveHeader::PROC_FLAG_FITTING)) {
            u32 frameLength = pHeader->frameLength;

            if (frameLength <= 1 || !(flag & AnimCurveHeader::PROC_FLAG_TURN)) {
                resDiv = tick / frameLength;

                if (!infiniteFlag &&
                    resDiv >= pHeader->loopCount) {
                    resDiv = static_cast<u8>(pHeader->loopCount - 1);
                    frame = frameLength;
                    break;
                }

                frame = tick - resDiv * frameLength;
                break;
            }

            u32 period = frameLength - 1;
            resDiv = tick / period;

            if (!infiniteFlag &&
                resDiv >= pHeader->loopCount) {
                u16 frameResult;
                if (pHeader->loopCount % 2 == 0) {
                    frameResult = 0;
                } else {
                    frameResult = static_cast<u16>(period);
                }
                resDiv = static_cast<u8>(pHeader->loopCount - 1);
                frame = frameResult;
                break;
            }

            if (resDiv % 2 == 0) {
                frame = tick - resDiv * period;
                break;
            }
            frame = period * (resDiv + 1) - tick;
            break;
        }

        if (tick >= life) {
            u16 frameResult;
            if (!(flag & AnimCurveHeader::PROC_FLAG_TURN) ||
                pHeader->loopCount % 2 != 0) {
                frameResult = pHeader->frameLength;
            } else {
                frameResult = 0;
            }
            resDiv = static_cast<u8>(pHeader->loopCount - 1);
            frame = frameResult;
            break;
        }

        u32 frameLength = pHeader->frameLength;

        if (frameLength <= 1 || !(flag & AnimCurveHeader::PROC_FLAG_TURN)) {
            f32 scale = pHeader->loopCount *
                        (static_cast<f32>(frameLength) / static_cast<f32>(life));

            resDiv = tick * scale / frameLength;
            frame = tick * scale - resDiv * frameLength;
            break;
        }

        u8 loopCount;
        fittingPeriod = frameLength - 1;
        loopCount = pHeader->loopCount;

        f32 scale =
            (1.0f + static_cast<f32>(static_cast<int>(fittingPeriod)) * loopCount) / life;
        u32 pos = tick * scale;
        resDiv = pos / fittingPeriod;

        if (resDiv >= loopCount) {
            u16 frameResult;
            if (loopCount % 2 == 0) {
                frameResult = 0;
            } else {
                frameResult = static_cast<u16>(fittingPeriod);
            }
            resDiv = static_cast<u8>(loopCount - 1);
            frame = frameResult;
            break;
        }

        if (resDiv % 2 == 0) {
            frame = pos - resDiv * fittingPeriod;
            break;
        }
        frame = fittingPeriod * (resDiv + 1) - pos;
        break;
    } while (false);

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

static inline void CalcChildFrame(const AnimCurveHeader* pHeader, u32 tick, u32 life,
                                u32& rLoop, bool& rEnd, u16& rFrame) {
    u32 flag = pHeader->processFlag;
    rLoop = 0;
    rEnd = false;

    if (!(flag & AnimCurveHeader::PROC_FLAG_INFLOOP) &&
        pHeader->loopCount <= 1) {

        if (!(flag & AnimCurveHeader::PROC_FLAG_FITTING)) {
            if (tick >= pHeader->frameLength) {
                rFrame = pHeader->frameLength;
                return;
            }
            rFrame = tick;
            return;
        }

        rFrame = pHeader->frameLength * tick / life;
        return;
    }

    if (!(flag & AnimCurveHeader::PROC_FLAG_FITTING)) {
        u32 frameLength = pHeader->frameLength;

        if (frameLength <= 1 || !(flag & AnimCurveHeader::PROC_FLAG_TURN)) {
            rLoop = tick / frameLength;

            if (!(flag & AnimCurveHeader::PROC_FLAG_INFLOOP) &&
                rLoop >= pHeader->loopCount) {
                rLoop = static_cast<u8>(pHeader->loopCount - 1);
                rFrame = frameLength;
                return;
            }

            rFrame = tick - rLoop * frameLength;
            return;
        }

        u8 loopCount;
        u32 period = frameLength - 1;
        rLoop = tick / period;

        if (!(flag & AnimCurveHeader::PROC_FLAG_INFLOOP) &&
            rLoop >= (loopCount = pHeader->loopCount)) {
            u16 frame;
            if (loopCount % 2 == 0) {
                if (rLoop == loopCount &&
                    tick - rLoop * period == 0) {
                    rEnd = true;
                }
                frame = 0;
            } else {
                frame = static_cast<u16>(period);
            }
            rLoop = static_cast<u8>(loopCount - 1);
            rFrame = frame;
            return;
        }

        if (rLoop % 2 == 0) {
            rFrame = tick - rLoop * period;
            return;
        }
        rFrame = (period * (rLoop + 1) - 1) - (tick - 1);
        return;
    }

    if (tick >= life) {
        u16 frame;
        if (!(flag & AnimCurveHeader::PROC_FLAG_TURN) ||
            pHeader->loopCount % 2 != 0) {
            frame = pHeader->frameLength;
        } else {
            frame = 0;
        }
        rLoop = static_cast<u8>(pHeader->loopCount - 1);
        rFrame = frame;
        return;
    }

    u32 frameLength = pHeader->frameLength;

    if (frameLength <= 1 || !(flag & AnimCurveHeader::PROC_FLAG_TURN)) {
        f32 scale = pHeader->loopCount *
                    (static_cast<f32>(frameLength) / static_cast<f32>(life));

        rLoop = tick * scale / frameLength;
        rFrame = tick * scale - rLoop * frameLength;
        return;
    }

    u8 loopCount;
    u32 period = frameLength - 1;
    loopCount = pHeader->loopCount;

    f32 scale =
        (1.0f + static_cast<f32>(static_cast<int>(period)) * loopCount) / life;
    u32 pos = tick * scale;
    rLoop = pos / period;

    if (rLoop >= loopCount) {
        u16 frame;
        if (loopCount % 2 == 0) {
            if (tick == life - 1) {
                rEnd = true;
            }
            frame = 0;
        } else {
            frame = static_cast<u16>(period);
        }
        rLoop = static_cast<u8>(loopCount - 1);
        rFrame = frame;
        return;
    }

    if (rLoop % 2 == 0) {
        rFrame = pos - rLoop * period;
        return;
    }
    rFrame = period * (rLoop + 1) - pos;
    return;
}

static inline void FindChildCurveKeyIndex(s32* pIndex, s32 frame, const u16* pKeys, u32 stride, s32& lo, s32 hi, int flag) {

    lo = 0;
    s32 mid = (lo + hi) / 2;

    if (frame < detail::ReadCurveKeyFrame(pKeys, stride, lo) || (frame == detail::ReadCurveKeyFrame(pKeys, stride, lo) && flag != 0)) {
        *pIndex = lo;
        return;
    }
    if (detail::ReadCurveKeyFrame(pKeys, stride, hi) < frame || (frame == detail::ReadCurveKeyFrame(pKeys, stride, hi) && flag == 0)) {
        *pIndex = hi;
        return;
    }
    s32 key = detail::ReadCurveKeyFrame(pKeys, stride, mid);
    while (lo < mid) {
        if (frame == key) {
            if (flag != 0) {
                hi = mid;
            } else {
                lo = mid;
            }
        } else if (key < frame) {
            lo = mid;
        } else {
            hi = mid;
        }
        mid = (lo + hi) / 2;
        key = detail::ReadCurveKeyFrame(pKeys, stride, mid);
    }
    *pIndex = lo;
}

void AnimCurveExecuteChild(u8* pCommandList, Particle* pParticle, u32 tick, u16 seed, u32 life) {
    u8* pCmdList = reinterpret_cast<u8*>(reinterpret_cast<u32>(pCommandList));

    const AnimCurveHeader* pHeader;
    const u8* pKeys;
    const u8* pRanges;
    const u8* pRandomTable;
    const u8* pNameTable;
    u16 tickA;
    u32 divA;
    u16 tickB;
    u32 divB;
    s32 idx;
    bool finalA;
    bool finalB;
    const detail::ChildCurveKey* pEntry;

    detail::SplitCurveResource(pCmdList, &pHeader, &pKeys, &pRanges, &pRandomTable, &pNameTable);
    CalcChildFrame(const_cast<AnimCurveHeader*>(pHeader), tick, life, divA, finalA, tickA);
    f32 conversionPhase = static_cast<f32>(tick);
    CalcChildFrame(const_cast<AnimCurveHeader*>(pHeader), tick + 1, life, divB, finalB, tickB);

    s32 searchStart;
    u32 secondFrame = tickB;
    const detail::ChildCurveKey* pKeyData = ((const detail::ChildCurveKeys*)pKeys)->mEntry;
    u32 div;
    if (divA == divB) {
        if (tickA == secondFrame) {
            if (finalA == 0) {
                return;
            }
            divB = divA + 1;
            tickB = 1;
        }
        if (tickA < tickB) {
            FindChildCurveKeyIndex(&idx, tickA, &pKeyData->mFrame, sizeof(detail::ChildCurveKey), searchStart,
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
            FindChildCurveKeyIndex(&idx, tickA, &pKeyData->mFrame, sizeof(detail::ChildCurveKey), searchStart,
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
            FindChildCurveKeyIndex(&idx, tickA, &pKeyData->mFrame, sizeof(detail::ChildCurveKey), searchStart,
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
            FindChildCurveKeyIndex(&idx, tickA, &pKeyData->mFrame, sizeof(detail::ChildCurveKey), searchStart,
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

    FindChildCurveKeyIndex(&idx, tickA, &pKeyData->mFrame, sizeof(detail::ChildCurveKey), searchStart,
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
        u32 loop;
        u32 infiniteLoop;
        u32 count = pHeader->frameLength;
        if (count <= 1) {
            frame = 0;
        } else {
            u32 flags = pHeader->processFlag;
            infiniteLoop = flags & AnimCurveHeader::PROC_FLAG_INFLOOP;
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
                loop = tick / span;
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
                if (tick >= life - 1) {
                    if (!(flags & AnimCurveHeader::PROC_FLAG_TURN) || pHeader->loopCount % 2 != 0) {
                        frame = count - 1;
                    } else {
                        frame = 0;
                    }
                } else {
                    s32 span = count - 1;
                    f32 rate = static_cast<f32>(pHeader->loopCount) * (static_cast<f32>(span) / static_cast<f32>(life - 1));
                    loop = static_cast<u32>(static_cast<f32>(tick) * rate / static_cast<f32>(span));
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
        u32 loop;
        u32 infiniteLoop;
        u32 count = pHeader->frameLength;
        if (count <= 1) {
            frame = 0;
        } else {
            u32 flags = pHeader->processFlag;
            infiniteLoop = flags & AnimCurveHeader::PROC_FLAG_INFLOOP;
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
                loop = tick / span;
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
                if (tick >= life - 1) {
                    if (!(flags & AnimCurveHeader::PROC_FLAG_TURN) || pHeader->loopCount % 2 != 0) {
                        frame = count - 1;
                    } else {
                        frame = 0;
                    }
                } else {
                    s32 span = count - 1;
                    f32 rate = static_cast<f32>(pHeader->loopCount) * (static_cast<f32>(span) / static_cast<f32>(life - 1));
                    loop = static_cast<u32>(static_cast<f32>(tick) * rate / static_cast<f32>(span));
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
