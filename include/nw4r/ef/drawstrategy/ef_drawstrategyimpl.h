#ifndef NW4R_EF_DRAW_STRATEGY_DRAW_STRATEGY_IMPL_H
#define NW4R_EF_DRAW_STRATEGY_DRAW_STRATEGY_IMPL_H
#include <nw4r/types_nw4r.h>

#include <nw4r/ef/ef_drawstrategy.h>
#include <nw4r/ef/ef_types.h>

#include <nw4r/ef/ef_particle.h>
#include <nw4r/ef/ef_particlemanager.h>
#include <nw4r/math/math_arithmetic.h>
#include <nw4r/math/math_constant.h>
#include <nw4r/math/math_types.h>

#include <revolution/GX.h>

namespace nw4r {
namespace ef {

struct TextureData;

class DrawStrategyImpl : public DrawStrategy {
public:
    typedef Particle* (*GetFirstDrawParticleFunc)(ParticleManager* pManager);
    typedef Particle* (*GetNextDrawParticleFunc)(ParticleManager* pManager,
                                                 Particle* pParticle);

    enum ParticleDrawOrder { DRAWORDER_YOUNGERS_FIRST, DRAWORDER_ELDERS_FIRST };

    struct AheadContext {
        struct {
            ParticleManager* mParticleManager;
            const math::MTX34* mViewMtx;
            math::MTX34 mEmitterMtx;
            math::MTX34 mParticleManagerMtx;
            math::MTX34 mParticleManagerMtxInv;
            math::VEC3 mEmitterAxisY;
            math::VEC3 mEmitterCenter;
        } mCommon;

        struct {
            math::_VEC3 mWorldYAxis;
        } mNoDesign;

        AheadContext(const math::MTX34& rViewMtx, ParticleManager* pManager);
    };

    typedef void (*CalcAheadFunc)(math::VEC3* pAxisY, AheadContext* pContext,
                                  Particle* pParticle);

    struct PrevTexture {
        const TextureData* data;
        f32 scaleS;
        f32 scaleT;
        f32 transS;
        f32 transT;
        GXTexWrapMode wrapS;
        GXTexWrapMode wrapT;
        math::VEC2 scale;
        f32 rotate;
        math::VEC2 translate;

        PrevTexture() {
            data = nullptr;
            scaleS = 1.0f;
            scaleT = 1.0f;
            transS = 1.0f;
            transT = 1.0f;
            wrapS = GX_CLAMP;
            wrapT = GX_CLAMP;
            scale.x = 1.0f;
            scale.y = 1.0f;
            rotate = 0.0f;
            translate.x = 0.0f;
            translate.y = 0.0f;
        }

        bool IsEqual(const TextureData* _pData, f32 _scaleS, f32 _scaleT,
                     f32 _transS, f32 _transT, GXTexWrapMode _wrapS,
                     GXTexWrapMode _wrapT, math::VEC2 _scale, f32 _rotate,
                     math::VEC2 _translate) {

            if (data != _pData || scaleS != _scaleS || scaleT != _scaleT ||
                transS != _transS || transT != _transT || wrapS != _wrapS ||
                wrapT != _wrapT || scale.x != _scale.x || scale.y != _scale.y ||
                rotate != _rotate || translate.x != _translate.x ||
                translate.y != _translate.y) {
                return false;
            }

            return true;
        }

        void Set(const TextureData* _pData, f32 _scaleS, f32 _scaleT,
                 f32 _transS, f32 _transT, GXTexWrapMode _wrapS,
                 GXTexWrapMode _wrapT, math::VEC2 _scale, f32 _rotate,
                 math::VEC2 _translate) {

            data = _pData;
            scaleS = _scaleS;
            scaleT = _scaleT;
            transS = _transS;
            transT = _transT;
            wrapS = _wrapS;
            wrapT = _wrapT;
            scale.x = _scale.x;
            scale.y = _scale.y;
            rotate = _rotate;
            translate.x = _translate.x;
            translate.y = _translate.y;
        }
    };

public:
    DrawStrategyImpl();

    virtual GetFirstDrawParticleFunc
    GetGetFirstDrawParticleFunc(int drawOrder);

    virtual GetNextDrawParticleFunc
    GetGetNextDrawParticleFunc(int drawOrder);

protected:
    void Initialize(ParticleManager* pManager,
                    const EmitterDrawSetting& rSetting, const DrawInfo& rInfo);

    void SetupGP(Particle* pParticle, const EmitterDrawSetting& rSetting,
                 const DrawInfo& rInfo, bool first, bool xfDirty);

    static void CalcAhead_Speed(math::VEC3* pAxisY, AheadContext* pContext,
                                Particle* pParticle) {

        pParticle->GetMoveDir(pAxisY);

        if (!Normalize(pAxisY)) {
            *pAxisY = pContext->mCommon.mEmitterAxisY;
        }
    }

    static void CalcAhead_EmitterCenter(math::VEC3* pAxisY,
                                        AheadContext* pContext,
                                        Particle* pParticle) {

        math::VEC3Sub(pAxisY, &pParticle->mParameter.mPosition,
                      &pContext->mCommon.mEmitterCenter);

        if (!Normalize(pAxisY)) {
            *pAxisY = pContext->mCommon.mEmitterAxisY;
        }
    }

    static void CalcAhead_EmitterDesign(math::VEC3* pAxisY,
                                        AheadContext* pContext,
                                        Particle* pParticle) {

        *pAxisY = pContext->mCommon.mEmitterAxisY;
    }

    static void CalcAhead_Particle(math::VEC3* pAxisY, AheadContext* pContext,
                                   Particle* pParticle) {

        Particle* pElder =
            GetElderDrawParticle(pContext->mCommon.mParticleManager, pParticle);

        if (pElder != nullptr) {
            math::VEC3Sub(pAxisY, &pElder->mParameter.mPosition,
                          &pParticle->mParameter.mPosition);
        } else {
            math::VEC3Sub(pAxisY, &pParticle->mParameter.mPosition,
                          &pContext->mCommon.mEmitterCenter);
        }

        if (!Normalize(pAxisY)) {
            *pAxisY = pContext->mCommon.mEmitterAxisY;
        }
    }

    static void CalcAhead_NoDesign(math::VEC3* pAxisY, AheadContext* pContext,
                                   Particle* pParticle) {
        *pAxisY = math::VEC3(pContext->mNoDesign.mWorldYAxis);
    }

    static void CalcAhead_ParticleBoth(math::VEC3* pAxisY,
                                       AheadContext* pContext,
                                       Particle* pParticle) {

        Particle* pElder =
            GetElderDrawParticle(pContext->mCommon.mParticleManager, pParticle);

        Particle* pYounger = GetYoungerDrawParticle(
            pContext->mCommon.mParticleManager, pParticle);

        math::VEC3 elderPos(0.0f, 0.0f, 0.0f);

        if (pElder != nullptr) {
            math::VEC3Sub(&elderPos, &pElder->mParameter.mPosition,
                          &pParticle->mParameter.mPosition);

            if (!Normalize(&elderPos)) {
                elderPos = math::VEC3(0.0f, 0.0f, 0.0f);
            }
        }

        math::VEC3 youngerPos(0.0f, 0.0f, 0.0f);

        if (pYounger != nullptr) {
            math::VEC3Sub(&youngerPos, &pYounger->mParameter.mPosition,
                          &pParticle->mParameter.mPosition);

            if (!Normalize(&youngerPos)) {
                youngerPos = math::VEC3(0.0f, 0.0f, 0.0f);
            }
        }

        math::VEC3Sub(pAxisY, &elderPos, &youngerPos);

        if (!Normalize(pAxisY)) {
            *pAxisY = pContext->mCommon.mEmitterAxisY;
        }
    }

    int GetNumDrawParticle(ParticleManager* pManager) {
        return pManager->mActivityList.GetNumActive();
    }

    static Particle* GetElderParticle(ParticleManager* pManager,
                                      Particle* pParticle) {

        return static_cast<Particle*>(
            NW4R_UT_LIST_GET_LINK(pManager->mActivityList.mActiveList,
                                  pParticle)
                ->prevObject);
    }
    static Particle* GetYoungerParticle(ParticleManager* pManager,
                                        Particle* pParticle) {

        return static_cast<Particle*>(
            NW4R_UT_LIST_GET_LINK(pManager->mActivityList.mActiveList,
                                  pParticle)
                ->nextObject);
    }

    static Particle* GetElderParticle(const ParticleManager* pManager,
                                      Particle* pParticle) {

        return static_cast<Particle*>(
            NW4R_UT_LIST_GET_LINK(pManager->mActivityList.mActiveList,
                                  pParticle)
                ->prevObject);
    }
    static Particle* GetYoungerParticle(const ParticleManager* pManager,
                                        Particle* pParticle) {

        return static_cast<Particle*>(
            NW4R_UT_LIST_GET_LINK(pManager->mActivityList.mActiveList,
                                  pParticle)
                ->nextObject);
    }

    static Particle* GetOldestParticle(ParticleManager* pManager) {
        return static_cast<Particle*>(
            pManager->mActivityList.mActiveList.headObject);
    }
    static Particle* GetYoungestParticle(ParticleManager* pManager) {
        return static_cast<Particle*>(
            pManager->mActivityList.mActiveList.tailObject);
    }

    static Particle* GetElderDrawParticle(const ParticleManager* pManager,
                                          Particle* pParticle) {

        Particle* pIt = GetElderParticle(pManager, pParticle);

        while (pIt != nullptr &&
               pIt->GetLifeStatus() != ReferencedObject::NW4R_EF_LS_ACTIVE) {

            pIt = GetElderParticle(pManager, pIt);
        }

        return pIt;
    }
    static Particle* GetYoungerDrawParticle(const ParticleManager* pManager,
                                            Particle* pParticle) {

        Particle* pIt = GetYoungerParticle(pManager, pParticle);

        while (pIt != nullptr &&
               pIt->GetLifeStatus() != ReferencedObject::NW4R_EF_LS_ACTIVE) {

            pIt = GetYoungerParticle(pManager, pIt);
        }

        return pIt;
    }

    static Particle* GetOldestDrawParticle(ParticleManager* pManager) {
        Particle* pIt = GetOldestParticle(pManager);

        while (pIt != nullptr &&
               pIt->GetLifeStatus() != ReferencedObject::NW4R_EF_LS_ACTIVE) {

            pIt = GetYoungerParticle(pManager, pIt);
        }

        return pIt;
    }
    static Particle* GetYoungestDrawParticle(ParticleManager* pManager) {
        Particle* pIt = GetYoungestParticle(pManager);

        while (pIt != nullptr &&
               pIt->GetLifeStatus() != ReferencedObject::NW4R_EF_LS_ACTIVE) {

            pIt = GetElderDrawParticle(pManager, pIt);
        }

        return pIt;
    }

    static bool Normalize(math::VEC3* pVec) {
        f32 sqMag = math::VEC3Dot(pVec, pVec);

        if (sqMag < NW4R_MATH_FLT_EPSILON) {
            return false;
        }

        *pVec *= math::FrSqrt(sqMag);
        return true;
    }

    static const math::VEC3& GetXUnitVec() {
        return mXUnitVec;
    }
    static const math::VEC3& GetYUnitVec() {
        return mYUnitVec;
    }
    static const math::VEC3& GetZUnitVec() {
        return mZUnitVec;
    }

private:
    void _InitTexture(const EmitterDrawSetting& rSetting);
    void _InitTev(const EmitterDrawSetting& rSetting, const DrawInfo& rInfo);
    void _InitColor(ParticleManager* pManager,
                    const EmitterDrawSetting& rSetting, const DrawInfo& rInfo);

    bool _SetupACmp(Particle* pParticle, const EmitterDrawSetting& rSetting,
                    bool first);
    bool _SetupTevReg(Particle* pParticle, const EmitterDrawSetting& rSetting,
                      bool first);
    bool _SetupTexture(Particle* pParticle, const EmitterDrawSetting& rSetting,
                       const DrawInfo& rInfo, bool first);

    static Particle*
    GetFirstDrawParticle_EldersFirst(ParticleManager* pManager);

    static Particle*
    GetFirstDrawParticle_YoungersFirst(ParticleManager* pManager);

    static Particle* GetNextDrawParticle_EldersFirst(ParticleManager* pManager,
                                                     Particle* pParticle);

    static Particle*
    GetNextDrawParticle_YoungersFirst(ParticleManager* pManager,
                                      Particle* pParticle);

protected:
    PrevTexture mPrevTexture[TEX_LAYER_MAX];
    GXColor mPrevTevColor[GX_MAX_TEVREG - 1];
    GXColor mPrevTevKColor[GX_MAX_KCOLOR];
    int mPrevARef0;
    int mPrevARef1;
    bool mUseColor[COLOR_LAYER_MAX][COLOR_IDX_MAX];
    bool mUseAlpha[COLOR_LAYER_MAX][COLOR_IDX_MAX];
    GXColor mColor[COLOR_LAYER_MAX][COLOR_IDX_MAX];
    u8 mNumTexmap;
    int mTexmapMap[TEX_LAYER_MAX];

    static const math::VEC3 mXUnitVec;
    static const math::VEC3 mYUnitVec;
    static const math::VEC3 mZUnitVec;

    static const math::VEC3 mZeroVec;
    static const math::MTX34 mIdentityMtx;
};

}
}

#endif
