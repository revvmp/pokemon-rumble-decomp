#ifndef NW4R_EF_ANIM_CURVE_INTERNAL_H
#define NW4R_EF_ANIM_CURVE_INTERNAL_H

#include <nw4r/ef/ef_res_animcurve.h>
#include <nw4r/math/math_arithmetic.h>
#include <nw4r/ef/ef_util.h>
#include <nw4r/ef/ef_emitter.h>

namespace nw4r {
namespace ef {
namespace detail {

inline void SplitCurveResource(const u8* pData, const AnimCurveHeader** ppHeader,
                                const u8** ppKeys, const u8** ppRanges,
                                const u8** ppRandom, const u8** ppNames) {
    *ppHeader = reinterpret_cast<const AnimCurveHeader*>(pData);
    *ppKeys = pData + sizeof(AnimCurveHeader);
    *ppRanges = *ppKeys + (*ppHeader)->keyTable;
    *ppRandom = *ppRanges + (*ppHeader)->rangeTable;
    *ppNames = *ppRandom + (*ppHeader)->randomTable;
}

struct TextureCurveKey {
    u8 mValue;
    u8 mType;
    u16 mNameIndex;
};

struct TextureCurveRecord {
    u16 mFrame;
    u8 mFlags;
    u8 mReserved[9];
    union Value {
        TextureCurveKey mKey;
        u16 mRandomIndex;
    } mValue;
};

struct TextureCurveKeys {
    u16 mCount;
    u16 mReserved;
    TextureCurveRecord mEntry[1];
};

struct TextureCurveRandomEntry {
    TextureCurveKey mKey;
    u8 mMode;
    u8 mReserved[3];
};

struct TextureCurveRandom {
    u16 mCount;
    u16 mReserved;
    TextureCurveRandomEntry mEntry[1];
};

struct TextureCurveRandomTable {
    u16 mCount;
    u16 mReserved;
    TextureCurveKey mKey[1];
};

struct ChildCurveSetting {
    EmitterInheritSetting mSetting;
    u16 mNameIndex;
};

struct ChildCurveKey {
    u16 mFrame;
    u8 mFlags;
    u8 mReserved[9];
    union Value {
        ChildCurveSetting mSetting;
        u16 mRandomIndex;
    } mData;
};

struct ChildCurveKeys {
    u16 mCount;
    u16 mReserved;
    ChildCurveKey mEntry[1];
};

struct ChildCurveTable {
    u16 mCount;
    u16 mReserved;
    ChildCurveSetting mEntry[1];
};

struct ValueCurveKey {
    u16 mFrame;
    u8 mFlags;
    u8 mReserved;
    u8 mType[8];
    union Values {
        f32 mValue[1];
        u8 mByte[1];
        u16 mRandomIndex;
    } mData;
};

struct CurveRandomByte {
    u8 mBase;
    u8 mRange;
};

struct CurveRandomRange {
    f32 mBase;
    f32 mRange;
};

union CurveRandomSeed {
    u32 mWord;
    u8 mByte[4];
};

inline u16 ReadCurveKeyFrame(const u16* pKeys, u32 stride, s32 index) {
    return *reinterpret_cast<const u16*>(reinterpret_cast<const u8*>(pKeys) + index * stride);
}

inline void CalcCurveFrame(u32 life, const AnimCurveHeader* pHeader, u32 tick, u16* pFrame, f32* pFrameF32,
                 u32* pLoop) {

    *pLoop = 0;
    u16 count = pHeader->frameLength;
    if (count <= 1) {
        *pFrame = 0;
        *pFrameF32 = 0.0f;
        *pLoop = tick;
        return;
    }

    u8 flags = pHeader->processFlag;
    u32 infiniteLoop = flags & 0x20;
    if (infiniteLoop == 0 && pHeader->loopCount <= 1) {
        if ((flags & 0x80) == 0) {
            if (tick >= count - 1) {
                *pFrame = count - 1;
            } else {
                *pFrame = tick;
            }
            *pFrameF32 = *pFrame;
            return;
        }
        if (life == 1) {
            *pFrameF32 = count - 1;
        } else {
            f32 t = (f32)tick * ((f32)(count - 1) / (f32)(life - 1));
            *pFrameF32 = t;
            if (t > (f32)(count - 1)) {
                *pFrameF32 = count - 1;
            }
        }
        *pFrame = *pFrameF32;
        return;
    }

    if ((flags & 0x80) == 0) {
        u32 div = tick / (count - 1);
        *pLoop = div;
        if ((flags & 0x40) == 0) {
            if (infiniteLoop == 0 && div >= pHeader->loopCount) {
                *pFrame = count - 1;
                *pLoop = (u8)(pHeader->loopCount - 1);
            } else {
                *pFrame = tick - div * (count - 1);
            }
        } else if (infiniteLoop == 0 && div >= pHeader->loopCount) {
            if (pHeader->loopCount % 2 == 0) {
                *pFrame = 0;
            } else {
                *pFrame = count - 1;
            }
            *pLoop = (u8)(pHeader->loopCount - 1);
        } else if ((div & 1) == 0) {
            *pFrame = tick - div * (count - 1);
        } else {
            *pFrame = (count - 1) * (div + 1) - tick;
        }
        *pFrameF32 = *pFrame;
        return;
    }

    u32 last = life - 1;
    if (tick >= last) {
        if ((flags & 0x40) == 0 || pHeader->loopCount % 2 != 0) {
            *pFrame = count - 1;
        } else {
            *pFrame = 0;
        }
        *pFrameF32 = *pFrame;
        *pLoop = (u8)(pHeader->loopCount - 1);
        return;
    }

    s32 span = count - 1;
    f32 rate = (f32)pHeader->loopCount * ((f32)span / (f32)last);
    u32 div = (u32)((f32)tick * rate / (f32)span);
    *pLoop = div;
    s32 span2 = count - 1;
    f32 frac = (f32)tick * rate - (f32)(div * span2);
    if ((flags & 0x40) == 0) {
        *pFrameF32 = frac;
    } else if ((div & 1) == 0) {
        *pFrameF32 = frac;
    } else {
        *pFrameF32 = (f32)span2 - frac;
    }
    *pFrame = *pFrameF32;
}

inline void CalcCurveIndex(u32 life, const AnimCurveHeader* pHeader, u32 tick, u16* pFrame, f32* pFrameF32,
                 u32* pLoop, u8* pFinal) {

    *pLoop = 0;
    *pFinal = 0;

    u8 flags = pHeader->processFlag;
    u16 count = pHeader->frameLength;
    u32 bit5 = flags & 0x20;

    if (bit5 == 0 && pHeader->loopCount <= 1) {
        if ((flags & 0x80) == 0) {
            if (tick >= count) {
                *pFrame = count;
            } else {
                *pFrame = (u16)tick;
            }
        } else {
            *pFrame = (u16)(count * tick / life);
        }
    } else if ((flags & 0x80) == 0) {
        if (count <= 1 || (flags & 0x40) == 0) {
            u32 div = tick / count;
            if (bit5 == 0 && div >= pHeader->loopCount) {
                *pFrame = count;
                *pLoop = (u8)(pHeader->loopCount - 1);
            } else {
                *pFrame = (u16)(tick - div * count);
                *pLoop = div;
            }
        } else {
            u32 div = tick / (count - 1);
            *pLoop = div;
            if ((flags & 0x20) == 0 && div >= pHeader->loopCount) {
                if (pHeader->loopCount % 2 == 0) {
                    if (div == pHeader->loopCount && tick - div * (count - 1) == 0) {
                        *pFinal = 1;
                    }
                    *pFrame = 0;
                } else {
                    *pFrame = (u16)(count - 1);
                }
                *pLoop = (u8)(pHeader->loopCount - 1);
            } else if ((div & 1) == 0) {
                *pFrame = (u16)(tick - div * (count - 1));
            } else {
                *pFrame = (u16)((count - 1) * (div + 1) - tick);
            }
        }
    } else if (tick >= life) {
        if ((flags & 0x40) == 0 || pHeader->loopCount % 2 != 0) {
            *pFrame = count;
        } else {
            *pFrame = 0;
        }
        *pLoop = (u8)(pHeader->loopCount - 1);
    } else {
        if (count <= 1 || (flags & 0x40) == 0) {
            f32 rate = (f32)pHeader->loopCount * ((f32)count / (f32)life);
            u32 div = (u32)((f32)tick * rate / (f32)count);
            *pLoop = div;
            *pFrame = (f32)tick * rate - (f32)(div * count);
        } else {
            s32 span = count - 1;
            f32 rate = (1.0f + (f32)span * (f32)pHeader->loopCount) / (f32)life;
            u32 pos = (u32)((f32)tick * rate);
            u32 div = pos / span;
            *pLoop = div;
            if (div >= pHeader->loopCount) {
                if (pHeader->loopCount % 2 == 0) {
                    if (tick == life - 1) {
                        *pFinal = 1;
                    }
                    *pFrame = 0;
                } else {
                    *pFrame = (u16)(count - 1);
                }
                *pLoop = (u8)(pHeader->loopCount - 1);
            } else if ((div & 1) == 0) {
                *pFrame = (u16)(pos - div * (count - 1));
            } else {
                *pFrame = (u16)((count - 1) * (div + 1) - pos);
            }
        }
    }
    *pFrameF32 = (f32)*pFrame;
}

inline void AdvanceCurveLoops(const AnimCurveHeader* pHeader, const AnimCurveKey* pKeys, u32 keyIndex, u32* pLeftLoop,
                 u32* pRightLoop) {

    *pRightLoop = *pLeftLoop;

    u8 flags = pHeader->processFlag;
    if ((flags & 0x40) == 0) {
        return;
    }
    u32 bit5 = flags & 0x20;
    if (bit5 == 0 && pHeader->loopCount <= 1) {
        return;
    }
    u32 v = *pLeftLoop;
    if ((v & 1) == 0 && (s32)(keyIndex + 1) >= (s32)(pKeys->count - 1)) {
        if (bit5 != 0 || v < (u32)(pHeader->loopCount - 1)) {
            (*pRightLoop)++;
        }
    }

    u32 w = *pLeftLoop;
    if ((w & 1) != 0 && keyIndex == 0 && w != 0) {
        (*pLeftLoop)++;
    }
}

inline u32 NextCurveSeed(u32 seed) {
    return seed * 0x343FD + 0x269EC3;
}

inline u32 HashCurveSeed(u16 particleSeed, u16 curveSeed, u16 randomIndex, u32 loop) {
    CurveRandomSeed h;
    h.mWord = (u32)loop * 0x7B929 + (u32)randomIndex * 0x371097E7 + ((u32)particleSeed * 0x3F81F635 + (u32)curveSeed * 0x30A74193) + 0x4BF53;
    h.mByte[2] ^= h.mByte[3];
    h.mByte[1] ^= h.mByte[2];
    h.mByte[0] ^= h.mByte[1];
    return h.mWord;
}

inline void FindCurveKeys(u32* pIndex, BOOL* pExact, u16* pLeftFrame, u16* pRightFrame, s32 frame, f32 frameF32,
                 const u16* pKeys, u32 stride, s32 lo, s32 hi) {

    s32 mid = (lo + hi) / 2;
    BOOL atTarget = nw4r::ef::FAbs((f32)frame - frameF32) < 1.1920929e-07f;

    u16 keyLo = ReadCurveKeyFrame(pKeys, stride, lo);
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
            *pRightFrame = ReadCurveKeyFrame(pKeys, stride, lo + 1);
        }
        return;
    }

    u16 keyHi = ReadCurveKeyFrame(pKeys, stride, hi);
    *pRightFrame = keyHi;
    if (keyHi <= frame) {
        *pIndex = hi;
        *pExact = 1;
        return;
    }

    s32 key = ReadCurveKeyFrame(pKeys, stride, mid);
    while (lo < mid) {
        if (frame == key) {
            *pIndex = mid;
            *pExact = atTarget;
            if (atTarget != 0) {
                return;
            }
            *pLeftFrame = key;
            *pRightFrame = ReadCurveKeyFrame(pKeys, stride, *pIndex + 1);
            return;
        } else if (key < frame) {
            lo = mid;
            *pLeftFrame = key;
        } else {
            hi = mid;
            *pRightFrame = key;
        }
        mid = (lo + hi) / 2;
        key = ReadCurveKeyFrame(pKeys, stride, mid);
    }
    *pIndex = lo;
    *pExact = 0;
}

inline u8 InterpolateCurveU8(u32 tick, u8 start, u8 end, u8 type) {
    u8 s = start;
    u8 e = end;

    if (s == e) {
        return start;
    }
    switch (type & 3) {
    case 0:
        return (u8)(s + ((tick * (e - s)) >> 16));
    case 1: {
        f32 t = (f32)tick / 65536.0f;
        if (type == 1) {
            f32 s0 = (f32)s;
            f32 w = 3.0f - 2.0f * t;
            return s0 + t * (t * (w * (f32)(int)(e - s)));
        } else {
            f32 c1 = ((type >> 2) & 1) == 0 ? 0.0f : 1.5f;
            f32 c0 = ((type >> 3) & 1) == 0 ? 0.0f : 1.5f;
            f32 s0 = (f32)s;
            f32 d = t * (f32)(int)(e - s);
            f32 u = t * (t * (2.0f - (c0 + c1)));
            f32 v = t * (3.0f + (-2.0f * c0 - c1));
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
        return start;
    default:
        return 0;
    }
}

inline f32 InterpolateCurveF32(f32 t, f32 a, f32 b, u8 type) {
    if (a == b) {
        return a;
    }
    switch (type & 3) {
    case 0:
        return a + t * (b - a);
    case 1:
        if (type == 1) {
            f32 w = 3.0f - 2.0f * t;
            return a + t * (t * (w * (b - a)));
        } else {
            f32 c0 = ((type >> 3) & 1) == 0 ? 0.0f : 1.5f;
            f32 c1 = ((type >> 2) & 1) == 0 ? 0.0f : 1.5f;
            f32 s = t * (b - a);
            f32 u = t * (t * (c0 + c1 - 2.0f));
            f32 v = t * (3.0f + (-2.0f * c0 - c1));
            return a + s * (c0 + (u + v));
        }
    case 2:
        return a;
    default:
        return 0.0f;
    }
}
inline void CopyTextureCurveKey(TextureCurveKey* pDst, const TextureCurveKey* pSrc) {
    pDst->mValue = pSrc->mValue;
    pDst->mType = pSrc->mType;
    pDst->mNameIndex = pSrc->mNameIndex;
}

inline void FindCurveKeyIndex(s32* pIndex, s32 frame, const u16* pKeys, u32 stride, s32 lo, s32 hi, int flag) {

    s32 mid = (lo + hi) / 2;

    if (frame < ReadCurveKeyFrame(pKeys, stride, lo) || (frame == ReadCurveKeyFrame(pKeys, stride, lo) && flag != 0)) {
        *pIndex = lo;
        return;
    }
    if (ReadCurveKeyFrame(pKeys, stride, hi) < frame || (frame == ReadCurveKeyFrame(pKeys, stride, hi) && flag == 0)) {
        *pIndex = hi;
        return;
    }
    s32 key = ReadCurveKeyFrame(pKeys, stride, mid);
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
        key = ReadCurveKeyFrame(pKeys, stride, mid);
    }
    *pIndex = lo;
}


}
}
}

#endif
