#ifndef NW4R_EF_PARTICLE_H
#define NW4R_EF_PARTICLE_H
#include <nw4r/types_nw4r.h>

#include <nw4r/ef/ef_particlemanager.h>
#include <nw4r/ef/ef_referencedobject.h>
#include <nw4r/ef/ef_res_emitter.h>
#include <nw4r/ef/ef_types.h>
#include <nw4r/math/math_types.h>

#include <revolution/GX.h>

namespace nw4r {
namespace ef {

struct EmitterInheritSetting;
struct TextureData;

struct ParticleParameterDesc {
    GXColor mColor[COLOR_LAYER_MAX][COLOR_IDX_MAX];
    math::VEC2 size;
    math::VEC2 scale;
    math::VEC3 rotate;
    math::VEC2 textureScale[TEX_LAYER_MAX];
    f32 textureRotate[TEX_LAYER_MAX];
    math::VEC2 textureTranslate[TEX_LAYER_MAX];
    TextureData* mTexture[TEX_LAYER_MAX];
    u16 textureWrap;
    u8 textureReverse;
    u8 mACmpRef0;
    u8 mACmpRef1;
    u8 rotateOffsetRandom[AXIS_MAX];
    f32 rotateOffset[AXIS_MAX];
    u8 textureNames[1];
};

class ParticleParameter {
public:
    GXColor mColor[COLOR_LAYER_MAX][COLOR_IDX_MAX];
    math::VEC2 mSize;
    math::VEC2 mScale;
    math::VEC3 mRotate;
    math::VEC2 mTextureScale[TEX_LAYER_MAX];
    f32 mTextureRotate[TEX_LAYER_MAX];
    math::VEC2 mTextureTranslate[TEX_LAYER_MAX];
    TextureData* mTexture[TEX_LAYER_MAX];
    u16 mTextureWrap;
    u8 mTextureReverse;
    u8 mACmpRef0;
    u8 mACmpRef1;
    s8 mAlphaFlickRnd;
    u8 mRotateOffset[AXIS_MAX];
    u8 mCollisionStatus;
    math::VEC3 mVelocity;
    math::VEC3 mPosition;
    math::VEC3 mPrevPosition;
    f32 mMomentum;

public:
    void Initialize(ParticleParameterDesc* pDesc, ParticleManager* pManager);
};

class Particle : public ReferencedObject {
    friend class ParticleManager;

public:
    ParticleParameter mParameter;
    ParticleManager* mParticleManager;
    math::VEC3 mPrevAxis;
    EvaluateStatus mEvalStatus;
    u16 mTick;
    u16 mRandSeed;
    u16 mLife;
    u16 mCalcRemain;

public:
    Particle();
    ~Particle();

    virtual void SendClosing();
    virtual void DestroyFunc();
    virtual bool Initialize(u16 life, math::VEC3 pos, math::VEC3 vel,
                            ParticleManager* pManager,
                            const math::MTX34* pSpace, float momentum,
                            const EmitterInheritSetting* pSetting,
                            Particle* pReferencePtcl);

    ParticleParameter* GetParticleParameter() {
        return &mParameter;
    }

    void Draw_GetColor(int layer, GXColor* pColorPri, GXColor* pColorSec);

    void GetColor(int layer, int index, GXColor* pColor) {
        pColor->r = mParameter.mColor[layer][index].r;
        pColor->g = mParameter.mColor[layer][index].g;
        pColor->b = mParameter.mColor[layer][index].b;
        pColor->a = mParameter.mColor[layer][index].a;
    }

    f32 Draw_GetSizeX() {
        f32 sx = mParameter.mSize.x * mParameter.mScale.x;
        return mParticleManager->Draw_ModifyScaleX(this, sx);
    }
    f32 Draw_GetSizeY() {
        f32 sy;

        switch (mParticleManager->mResource->GetEmitterDrawSetting()->mFlags &
                (EmitterDrawSetting::FLAG_XY_SAME_SIZE |
                 EmitterDrawSetting::FLAG_XY_SAME_SCALE)) {

        case EmitterDrawSetting::FLAG_XY_SAME_SCALE: {
            sy = mParameter.mSize.y * mParameter.mScale.x;
            break;
        }

        case EmitterDrawSetting::FLAG_XY_SAME_SIZE: {
            sy = mParameter.mSize.x * mParameter.mScale.y;
            break;
        }

        case (EmitterDrawSetting::FLAG_XY_SAME_SIZE |
              EmitterDrawSetting::FLAG_XY_SAME_SCALE): {
            sy = mParameter.mSize.x * mParameter.mScale.x;
            break;
        }

        default: {
            sy = mParameter.mSize.y * mParameter.mScale.y;
            break;
        }
        }

        return mParticleManager->Draw_ModifyScaleY(this, sy);
    }

    void Draw_GetRotate(math::VEC3* pRot) {
        *pRot = mParameter.mRotate;

        if (mParameter.mRotateOffset[AXIS_X] > 0) {
            pRot->x += NW4R_MATH_FIDX_TO_RAD(mParameter.mRotateOffset[AXIS_X]);
        }

        if (mParameter.mRotateOffset[AXIS_Y] > 0) {
            pRot->y += NW4R_MATH_FIDX_TO_RAD(mParameter.mRotateOffset[AXIS_Y]);
        }

        if (mParameter.mRotateOffset[AXIS_Z] > 0) {
            pRot->z += NW4R_MATH_FIDX_TO_RAD(mParameter.mRotateOffset[AXIS_Z]);
        }

        mParticleManager->Draw_ModifyRotate(this, pRot);
    }

    int Draw_GetTextureScaleS(int layer) const {
        int scale = 1;

        if (GetTextureWrapS(layer) == GX_MIRROR) {
            scale = 2;
        }

        u8 reverse =
            (mParameter.mTextureReverse >> (layer * NUM_OF_TEX_REVERSE) &
             TEX_REVERSE_ST);

        if (static_cast<u32>(reverse) == TEX_REVERSE_S ||
            static_cast<u32>(reverse) == TEX_REVERSE_ST) {
            scale -= scale * 2;
        }

        return scale;
    }
    int Draw_GetTextureScaleT(int layer) const {
        int scale = 1;

        if (GetTextureWrapT(layer) == GX_MIRROR) {
            scale = 2;
        }

        u8 reverse =
            (mParameter.mTextureReverse >> (layer * NUM_OF_TEX_REVERSE) &
             TEX_REVERSE_ST);

        if (static_cast<u32>(reverse) == TEX_REVERSE_T ||
            static_cast<u32>(reverse) == TEX_REVERSE_ST) {
            scale -= scale * 2;
        }

        return scale;
    }

    int Draw_GetTextureTransS(int layer) const {
        int trans = 0;

        u8 reverse =
            (mParameter.mTextureReverse >> (layer * NUM_OF_TEX_REVERSE) &
             TEX_REVERSE_ST);

        if (static_cast<u32>(reverse) == TEX_REVERSE_S ||
            static_cast<u32>(reverse) == TEX_REVERSE_ST) {
            trans = 1;
        }

        if (GetTextureWrapS(layer) == GX_MIRROR) {
            trans *= 2;
        }

        return trans;
    }
    int Draw_GetTextureTransT(int layer) const {
        int trans = 0;

        u8 reverse =
            (mParameter.mTextureReverse >> (layer * NUM_OF_TEX_REVERSE) &
             TEX_REVERSE_ST);

        if (static_cast<u32>(reverse) == TEX_REVERSE_T ||
            static_cast<u32>(reverse) == TEX_REVERSE_ST) {
            trans = 1;
        }

        if (GetTextureWrapT(layer) == GX_MIRROR) {
            trans *= 2;
        }

        return trans;
    }

    GXTexWrapMode GetTextureWrapS(int layer) const {
        return static_cast<GXTexWrapMode>(mParameter.mTextureWrap >>
                (layer * NUM_OF_TEX_WRAP_ST + 0) & TEX_WRAP_MASK);
    }
    GXTexWrapMode GetTextureWrapT(int layer) const {
        return static_cast<GXTexWrapMode>(mParameter.mTextureWrap >>
                (layer * NUM_OF_TEX_WRAP_ST + NUM_OF_TEX_WRAP_S) & TEX_WRAP_MASK);
    }

    math::VEC3& GetMoveDir(math::VEC3* pDir) {
        *pDir = mParameter.mPosition - mParameter.mPrevPosition;
        return *pDir;
    }

    math::VEC3& AddPosition(const math::VEC3* pDelta) {
        mParameter.mPosition.x += pDelta->x * mParameter.mMomentum;
        mParameter.mPosition.y += pDelta->y * mParameter.mMomentum;
        mParameter.mPosition.z += pDelta->z * mParameter.mMomentum;
        return mParameter.mPosition;
    }

private:
    enum TextureReverseFlag {
        TEX_REVERSE_S = (1 << 0),
        TEX_REVERSE_T = (1 << 1),
        TEX_REVERSE_ST = TEX_REVERSE_S | TEX_REVERSE_T,

        NUM_OF_TEX_REVERSE = 2
    };

    enum TextureWrapFlag {
        TEX_WRAP_MASK = (1 << 0) | (1 << 1),

        NUM_OF_TEX_WRAP_S = 2,
        NUM_OF_TEX_WRAP_T = 2,

        NUM_OF_TEX_WRAP_ST = 4
    };
};

}
}

#endif
