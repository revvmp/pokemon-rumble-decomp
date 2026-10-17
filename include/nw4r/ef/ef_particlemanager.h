#ifndef NW4R_EF_PARTICLE_MANAGER_H
#define NW4R_EF_PARTICLE_MANAGER_H
#include <nw4r/types_nw4r.h>

#include <nw4r/ef/ef_activitylist.h>
#include <nw4r/ef/ef_referencedobject.h>
#include <nw4r/ef/ef_res_emitter.h>

#include <nw4r/math/math_types.h>
#include <nw4r/ut/ut_list.h>

namespace nw4r {
namespace ef {

class DrawInfo;
class DrawStrategy;
class Emitter;
struct EmitterInheritSetting;
struct EmitterResource;
class Particle;

class ParticleManager : public ReferencedObject {
public:
    enum Flag {
        FLAG_MTX_INHERIT_SCALE = (1 << 0),
        FLAG_MTX_INHERIT_ROT = (1 << 1),
    };

    struct ParticleModifier {
        enum SimpleLightType {
            SIMPLELIGHT_OFF,
            SIMPLELIGHT_AMBIENT,
            SIMPLELIGHT_DIFFUSE
        };

        struct SimpleLight {
            u8 mType;
            GXColor mAmbient;
            GXColor mDiffuse;
            f32 mRadius;
            math::VEC3 mPosition;
        };

        math::VEC2 mScale;
        math::VEC3 mRotate;
        SimpleLight mLight;
    };

public:
    Emitter* mManagerEM;
    EmitterResource* mResource;
    u32 mFlag;
    DrawStrategy* mDrawStrategy;
    ut::Link mDrawOrderLink;
    ActivityList mActivityList;
    Particle* mLastCalced;
    ParticleModifier mModifier;
    s8 mInheritTranslate;
    u8 mWeight;

protected:
    bool mMtxDirty;

public:
    math::MTX34 mMtx;

    static math::MTX34 smDrawMtxPMtoEM;
    static math::MTX34 smMtxInv;
    static s32 smMtxInvId;

public:
    ParticleManager();
    ~ParticleManager();

    virtual void SendClosing();
    virtual void DestroyFunc();

    virtual bool Initialize(Emitter* pParent,
                            EmitterResource* pResource);

    virtual Particle* CreateParticle(u16 life, math::VEC3 pos, math::VEC3 vel,
                                     const math::MTX34* pSpace, f32 momentum,
                                     const EmitterInheritSetting* pSetting,
                                     Particle* pReferencePtcl,
                                     u16 calcRemain);

    virtual void Calc();
    virtual void Draw(const DrawInfo& rInfo);

    bool Closing(Particle* pParticle);

    int RetireParticle(Particle* pParticle);
    int RetireParticleAll();

    math::MTX34* CalcGlobalMtx(math::MTX34* pResult);

    void BeginCalc(bool remainOnly);
    void EndCalc();

    void BeginDraw();
    void EndDraw();

    const math::MTX34* Draw_GetMtxPMtoEM() const;

    void Draw_ModifyColor(Particle* pParticle, GXColor* pColorPri,
                          GXColor* pColorSec);

    f32 Draw_ModifyScaleX(Particle*  , f32 sx) {
        return sx * mModifier.mScale.x;
    }
    f32 Draw_ModifyScaleY(Particle*  , f32 sy) {
        return sy * mModifier.mScale.y;
    }

    void Draw_ModifyRotate(Particle*  , math::VEC3* pRot) {
        pRot->x += mModifier.mRotate.x;
        pRot->y += mModifier.mRotate.y;
        pRot->z += mModifier.mRotate.z;
    }

    void Modifier_SetSimpleLightParameter(const EmitterDrawSetting& rSetting) {
        Modifier_SetSimpleLightParameter(
            rSetting.mLighting.mType, rSetting.mLighting.mAmbient,
            rSetting.mLighting.mDiffuse, rSetting.mLighting.mRadius,
            rSetting.mLighting.mPosition);
    }

    void Modifier_SetSimpleLightParameter(u8 type, const GXColor& rAmbient,
                                          const GXColor& rDiffuse, f32 radius,
                                          const math::VEC3& rPos) {
        mModifier.mLight.mType = type;
        mModifier.mLight.mAmbient = rAmbient;
        mModifier.mLight.mDiffuse = rDiffuse;
        mModifier.mLight.mRadius = radius;
        mModifier.mLight.mPosition = rPos;
    }

    ActivityList* GetParticleList() {
        return &mActivityList;
    }

    void ParticleToFree(Particle* pParticle) {
        mActivityList.ToFree(pParticle);
    }

    void SetMtxDirty() {
        mMtxDirty = true;
    }

    static void ModifierTravFunc_SetSimpleLightType(void* pObject,
                                                    ForEachParam param) {

        static_cast<ParticleManager*>(pObject)->mModifier.mLight.mType =
            static_cast<u8>(param);
    }

    static void ModifierTravFunc_SetSimpleLightAmbient(void* pObject,
                                                       ForEachParam param) {

        static_cast<ParticleManager*>(pObject)->mModifier.mLight.mAmbient =
            *reinterpret_cast<GXColor*>(param);
    }

    static void ModifierTravFunc_SetScale(void* pObject, ForEachParam param) {
        static_cast<ParticleManager*>(pObject)->mModifier.mScale =
            *reinterpret_cast<math::VEC2*>(param);
    }

    static void ModifierTravFunc_SetRotate(void* pObject, ForEachParam param) {
        static_cast<ParticleManager*>(pObject)->mModifier.mRotate =
            *reinterpret_cast<math::VEC3*>(param);
    }
};

}
}

#endif
