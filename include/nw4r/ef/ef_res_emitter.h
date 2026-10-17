#ifndef NW4R_EF_RES_EMITTER_H
#define NW4R_EF_RES_EMITTER_H
#include <nw4r/types_nw4r.h>

#include <nw4r/ef/ef_res_animcurve.h>
#include <nw4r/ef/ef_types.h>
#include <nw4r/ef/emform/ef_emform.h>
#include <nw4r/math/math_types.h>

#include <revolution/GX.h>

namespace nw4r {
namespace ef {

struct ParticleParameterDesc;

struct TevStageColor {
    u8 mA;
    u8 mB;
    u8 mC;
    u8 mD;
};

struct TevStageColorOp {
    u8 mOp;
    u8 mBias;
    u8 mScale;
    u8 mClamp;
    u8 mOutReg;
};

struct TevStageAlpha {
    u8 mA;
    u8 mB;
    u8 mC;
    u8 mD;
};

struct BlendMode {
    u8 mType;
    u8 mSrcFactor;
    u8 mDstFactor;
    u8 mOp;
};

struct ColorInput {
    enum TevColor {
        TEVCOLOR_NULL,
        TEVCOLOR1_1,
        TEVCOLOR1_2,
        TEVCOLOR2_3,
        TEVCOLOR2_2,
        TEVCOLOR1_MULT,
        TEVCOLOR2_MULT
    };

    enum RasColor {
        RASCOLOR_NULL,
        RASCOLOR_LIGHTING,
    };

    u8 mRasColor;
    u8 mTevColor[GX_MAX_TEVREG - 1];
    u8 mTevKColor[GX_MAX_KCOLOR];
};

#pragma pack(push, 1)
struct Lighting {
    enum Mode {
        LIGHTING_MODE_OFF,
        LIGHTING_MODE_SIMPLE,
        LIGHTING_MODE_HARDWARE
    };

    enum Type {
        LIGHTING_TYPE_NONE,
        LIGHTING_TYPE_AMBIENT,
        LIGHTING_TYPE_POINT
    };

    u8 mMode;
    u8 mType;
    GXColor mAmbient;
    GXColor mDiffuse;
    f32 mRadius;
    math::VEC3 mPosition;
};
#pragma pack(pop)

struct EmitterDrawSetting {
    enum Flag {
        FLAG_ZCOMP_ENABLE_TEST = (1 << 0),
        FLAG_ZCOMP_ENABLE_UPDATE = (1 << 1),
        FLAG_ZCOMP_BEFORE_TEX = (1 << 2),
        FLAG_CLIP_DISABLE = (1 << 3),
        FLAG_TEX1_ENABLE = (1 << 4),
        FLAG_TEX2_ENABLE = (1 << 5),
        FLAG_TEXIND_ENABLE = (1 << 6),
        FLAG_TEX1_PROJ = (1 << 7),
        FLAG_TEX2_PROJ = (1 << 8),
        FLAG_TEXIND_PROJ = (1 << 9),
        FLAG_HIDDEN = (1 << 10),
        FLAG_DRAW_ORDER = (1 << 11),
        FLAG_FOG_ENABLE = (1 << 12),
        FLAG_XY_SAME_SIZE = (1 << 13),
        FLAG_XY_SAME_SCALE = (1 << 14),
    };

    enum AlphaFlickType {
        ALPHAFLICK_NONE,
        ALPHAFLICK_TRIANGLE,
        ALPHAFLICK_SAWTOOTH1,
        ALPHAFLICK_SAWTOOTH2,
        ALPHAFLICK_SQUARE,
        ALPHAFLICK_SINE
    };

    enum Assist {
        ASSIST_CMN_NORMAL = 0,
        ASSIST_CMN_CROSS,

        ASSIST_BB_NORMAL = 0,
        ASSIST_BB_Y,
        ASSIST_BB_DIRECTIONAL,
        ASSIST_BB_NOROLL,

        ASSIST_ST_NORMAL = 0,
        ASSIST_ST_CROSS,
        ASSIST_ST_BILLBOARD,
        ASSIST_ST_TUBE,
    };

    enum Ahead {
        AHEAD_CMN_NODESIGN = 5,

        AHEAD_BB_SPEED = 0,
        AHEAD_BB_EMITTER_CENTER,
        AHEAD_BB_EMITTER_DESIGN,
        AHEAD_BB_PARTICLE,
        AHEAD_BB_PARTICLE_BOTH,

        AHEAD_ST_SPEED = 0,
        AHEAD_ST_EMITTER_CENTER,
        AHEAD_ST_EMITTER_DESIGN,
        AHEAD_ST_PARTICLE,
        AHEAD_ST_PARTICLE_BOTH = 6,
    };

    enum OptionTypeCommon { TYPE_CMN_NORMAL, TYPE_CMN_CROSS };

    enum DirType {
        DIR_NO_DESIGN = 5,
        DIR_7 = 7,
    };

    u16 mFlags;
    u8 mACmpComp0;
    u8 mACmpComp1;
    u8 mACmpOp;
    u8 mNumTevs;
    u8 mFlagClamp;
    u8 mIndirectTargetStage;
    u8 mTevTexture[4];
    TevStageColor mTevColor[4];
    TevStageColorOp mTevColorOp[4];
    TevStageAlpha mTevAlpha[4];
    TevStageColorOp mTevAlphaOp[4];
    u8 mTevKColorSel[4];
    u8 mTevKAlphaSel[4];
    BlendMode mBlendMode;
    ColorInput mColorInput;
    ColorInput mAlphaInput;
    u8 mZCompareFunc;
    u8 mAlphaFlickType;
    u16 mAlphaFlickCycle;
    u8 mAlphaFlickRandom;
    u8 mAlphaFlickAmplitude;
    Lighting mLighting;
    f32 mIndTexOffsetMtx[2][3];
    s8 mIndTexScaleExp;
    s8 pivotX;
    s8 pivotY;
    u8 PADDING_0xAB;
    u8 ptcltype;
    u8 typeOption;
    u8 typeDir;
    u8 typeAxis;
    u8 typeOption0;
    u8 typeOption1;
    u8 typeOption2;
    u8 PADDING_0xB3;
    f32 zOffset;
};

struct EmitterDesc {
    enum CommonFlag {
        CMN_FLAG_SYNC_LIFE = (1 << 0),
        CMN_FLAG_DISABLE_DRAW = (1 << 1),
        CMN_FLAG_MAX_LIFE = (1 << 2),

        CMN_FLAG_INHERIT_PTCL_SCALE = (1 << 5),
        CMN_FLAG_INHERIT_PTCL_ROT = (1 << 6),

        CMN_FLAG_EMIT_INHERIT_SCALE = (1 << 7),
        CMN_FLAG_EMIT_INHERIT_ROT = (1 << 8),

        CMN_FLAG_DISABLE_CALC = (1 << 9),

        CMN_FLAG_INHERIT_PTCL_PIVOT = (1 << 10),
        CMN_FLAG_EMIT_INHERIT_PIVOT = (1 << 11),

        CMN_FLAG_PTCL_EMIT_INHERIT_SCALE = (1 << 12),
        CMN_FLAG_PTCL_EMIT_INHERIT_ROT = (1 << 13),

        CMN_FLAG_RELOCATED = (1 << 30),
    };

    enum EmitFlag {
        EMIT_FLAG_8 = (1 << 8),
        EMIT_FLAG_15 = (1 << 15),
        EMIT_FLAG_16 = (1 << 16),
        EMIT_FLAG_17 = (1 << 17),
        EMIT_FLAG_18 = (1 << 18),
        EMIT_FLAG_24 = (1 << 24),
        EMIT_FLAG_XYZ_SAME_SIZE = (1 << 25),
        EMIT_FLAG_26 = (1 << 26),
    };

    u32 commonFlag;
    u32 emitFlag;
    u16 emitLife;
    u16 ptclLife;
    s8 ptclLifeRandom;
    s8 inheritChildPtclTranslate;
    s8 emitEmitIntarvalRandom;
    s8 emitEmitRandom;
    f32 emitEmit;
    u16 emitEmitStart;
    u16 emitEmitPast;
    u16 emitEmitInterval;
    s8 inheritPtclTranslate;
    s8 inheritChildEmitTranslate;
    f32 commonParam[NUM_PARAMS];
    u16 emitEmitDiv;
    s8 velInitVelocityRandom;
    s8 velMomentumRandom;
    f32 velPowerRadiationDir;
    f32 velPowerYAxis;
    f32 velPowerRandomDir;
    f32 velPowerNormalDir;
    f32 velDiffusionEmitterNormal;
    f32 velPowerSpecDir;
    f32 velDiffusionSpecDir;
    math::VEC3 velSpecDir;
    math::VEC3 scale;
    math::VEC3 rotate;
    math::VEC3 translate;
    u8 lodNear;
    u8 lodFar;
    u8 lodMinEmit;
    u8 lodAlpha;
    u32 randomSeed;
    u8 userdata[8];
    EmitterDrawSetting drawSetting;

    EmitFormType GetFormType() {
        return static_cast<EmitFormType>(emitFlag & 0xFF);
    }
};

struct EmitterResource {
private:
    char* name;
    u32 headersize;

public:
    EmitterDesc* GetEmitterDesc() {
        return reinterpret_cast<EmitterDesc*>(reinterpret_cast<u8*>(this) +
                                              sizeof(EmitterResource));
    }
    u8* SkipEmitterDesc() {
        u8* pPtr = reinterpret_cast<u8*>(this);
        pPtr += headersize + sizeof(EmitterResource);
        return pPtr;
    }

    const char* GetName() const {
        return name;
    }

    EmitterDrawSetting* GetEmitterDrawSetting() {
        return &GetEmitterDesc()->drawSetting;
    }

    ParticleParameterDesc* GetParticleParameterDesc() {
        u8* pPtr = SkipEmitterDesc();
        pPtr += 4;
        return reinterpret_cast<ParticleParameterDesc*>(pPtr);
    }
    u8* SkipParticleParameterDesc() {
        u8* pPtr = SkipEmitterDesc();
        pPtr += *reinterpret_cast<u32*>(pPtr) + sizeof(u32);
        return pPtr;
    }

    u16 NumPtclTrack() {
        const u8* pPtr = SkipParticleParameterDesc();
        u16 numPtclTrack = *(reinterpret_cast<const u16*>(pPtr) + 0);
        return numPtclTrack;
    }
    u16 NumPtclInitTrack() {
        const u8* pPtr = SkipParticleParameterDesc();
        u16 numPtclTrack = *(reinterpret_cast<const u16*>(pPtr) + 1);
        return numPtclTrack;
    }

    u16 NumEmitTrack() {
        const u8* pPtr = SkipParticleParameterDesc();
        u16 numPtclTrack = *reinterpret_cast<const u16*>(pPtr);

        pPtr += numPtclTrack * 8 + 4;
        u16 numEmitTrack = *(reinterpret_cast<const u16*>(pPtr) + 0);

        return numEmitTrack;
    }
    u16 NumEmitInitTrack() {
        const u8* pPtr = SkipParticleParameterDesc();
        u16 numPtclTrack = *reinterpret_cast<const u16*>(pPtr);

        pPtr += numPtclTrack * 8 + 4;
        u16 numEmitTrack = *(reinterpret_cast<const u16*>(pPtr) + 1);

        return numEmitTrack;
    }

    u8** GetPtclTrackTbl() {
        u8* pPtr = SkipParticleParameterDesc();
        u8** ppTbl = reinterpret_cast<u8**>(pPtr + 4);
        return ppTbl;
    }
    u8** GetEmitTrackTbl() {
        u8* pPtr = SkipParticleParameterDesc();
        u16 numPtclTrack = *reinterpret_cast<const u16*>(pPtr);

        pPtr += numPtclTrack * 8 + 4;
        u8** ppTbl = reinterpret_cast<u8**>(pPtr + 4);
        return ppTbl;
    }

    u8* GetPtclTrack(u16 idx) {
        return GetPtclTrackTbl()[idx];
    }
    u8* GetEmitTrack(u16 idx) {
        return GetEmitTrackTbl()[idx];
    }

    void LocateTracks() {
        u8* pTail = SkipParticleParameterDesc();
        pTail += NumPtclTrack() * 8 + 4;
        pTail += NumEmitTrack() * 8 + 4;

        u32* pPtclPtrTbl = reinterpret_cast<u32*>(GetPtclTrackTbl());
        u32* pEmitPtrTbl = reinterpret_cast<u32*>(GetEmitTrackTbl());

        u32* pPtclSizeTbl =
            reinterpret_cast<u32*>(GetPtclTrackTbl()) + NumPtclTrack();
        u32* pEmitSizeTbl =
            reinterpret_cast<u32*>(GetEmitTrackTbl()) + NumEmitTrack();

        int i;
        for (i = 0; i < NumPtclTrack(); i++) {
            pPtclPtrTbl[i] = reinterpret_cast<u32>(pTail);
            pTail += pPtclSizeTbl[i];
        }

        for (i = 0; i < NumEmitTrack(); i++) {
            pEmitPtrTbl[i] = reinterpret_cast<u32>(pTail);
            pTail += pEmitSizeTbl[i];
        }
    }
};

class ResEmitter {
private:
    EmitterResource* mData;

public:
    explicit ResEmitter(EmitterResource* pData = NULL) : mData(pData) {}
    ResEmitter(const ResEmitter& rOther) : mData(rOther.mData) {}

    bool IsValid() const {
        return mData != NULL;
    }

    EmitterResource* ptr() {
        return mData;
    }

    ResAnimCurve GetPtclTrack(u16 idx);
};

}
}

#endif
