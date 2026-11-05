#define PARTICLE_MANAGER_EMISSION_LAYOUT
#include <nw4r/ef/drawstrategy/ef_drawstrategybuilder.h>
#include <nw4r/ef/ef_drawinfo.h>
#include <nw4r/ef/ef_drawstrategy.h>
#include <nw4r/ef/ef_effect.h>
#include <nw4r/ef/ef_effectsystem.h>
#include <nw4r/ef/ef_emitter.h>
#include <nw4r/ef/ef_memorymanager.h>
#include <nw4r/ef/ef_particle.h>
#include <nw4r/ef/ef_particlemanager.h>
#include <nw4r/ef/ef_particlemanagerfield.h>
#include <nw4r/ef/ef_resource.h>
#include <nw4r/ef/ef_util.h>
#include <nw4r/math/math_arithmetic.h>
#include <nw4r/math/math_constant.h>
#include <MSL_C/math.h>
#include <string.h>

namespace nw4r {
namespace ef {

enum CurveOffset {
    CURVE_OFFSET_MIN = 0,
    CURVE_OFFSET_MAX = 0xFFFFFFFF
};

inline AnimCurveNameTable* GetPostFieldNames(AnimCurveHeader* pHeader,
                                            CurveOffset& rangeSize,
                                            CurveOffset& randomSize) {
    u8* pCursor = reinterpret_cast<u8*>(pHeader);
    pCursor += pHeader->keyTable;
    pCursor = reinterpret_cast<u8*>(reinterpret_cast<u32>(pCursor) + sizeof(AnimCurveHeader));
    rangeSize = static_cast<CurveOffset>(pHeader->rangeTable);
    pCursor += rangeSize;
    randomSize = static_cast<CurveOffset>(pHeader->randomTable);
    return reinterpret_cast<AnimCurveNameTable*>(reinterpret_cast<u32>(pCursor) + randomSize);
}

inline void GetPostFieldNameSize(AnimCurveHeader* pHeader, CurveOffset& nameSize) {
    nameSize = static_cast<CurveOffset>(pHeader->nameTable);
}

math::MTX34 ParticleManager::smDrawMtxPMtoEM;

s32 ParticleManager::smMtxInvId = 0;

ParticleManager::ParticleManager()
    : mActivityList(offsetof(Particle, mActivityLink)) {}

void ParticleManager::SendClosing() {
    mManagerEM->Closing(this);
}

void ParticleManager::DestroyFunc() {}

bool ParticleManager::Closing(Particle* pParticle) {
    pParticle->mParticleManager->UnRef();

    mActivityList.ToClosing(pParticle);
    pParticle->mLifeStatus = NW4R_EF_LS_CLOSING;

    return true;
}

int ParticleManager::RetireParticle(Particle* pParticle) {
    if (pParticle->mLifeStatus != NW4R_EF_LS_ACTIVE) {
        return 0;
    }

    mActivityList.ToWait(pParticle);
    pParticle->Destroy();
    return 1;
}

int ParticleManager::RetireParticleAll() {
    int num = 0;

    Particle* pIt;
    Particle* pNext;

    for (pIt = static_cast<Particle*>(mActivityList.mActiveList.headObject);
         pIt != NULL; pIt = pNext) {
        pNext = static_cast<Particle*>(
            NW4R_UT_LIST_GET_LINK(mActivityList.mActiveList, pIt)->nextObject);

        if (pIt->mLifeStatus == NW4R_EF_LS_ACTIVE) {
            num += RetireParticle(pIt);
        }
    }

    return num;
}

bool ParticleManager::Initialize(Emitter* pParent, EmitterResource* pResource) {
    ReferencedObject::Initialize();

    mActivityList.Initialize();

    mModifier.mScale.x = 1.0f;
    mModifier.mScale.y = 1.0f;

    mModifier.mRotate.x = 0.0f;
    mModifier.mRotate.y = 0.0f;
    mModifier.mRotate.z = 0.0f;

    mManagerEM = pParent;
    pParent->Ref();

    mResource = pResource;
    mFlag = 0;
    SetMtxDirty();

    EmitterDesc* pDesc = mResource->GetEmitterDesc();

    mDrawStrategy =
        mManagerEM->mManagerEF->mManagerES->mDrawStrategyBuilder->Create(
            pDesc->drawSetting.ptcltype);

    mLastCalced = NULL;

    Modifier_SetSimpleLightParameter(pDesc->drawSetting);

    return true;
}

ParticleManager::~ParticleManager() {}

Particle*
ParticleManager::CreateParticle(u16 life, math::VEC3 pos, math::VEC3 vel,
                                const math::MTX34* pSpace, f32 momentum,
                                const EmitterInheritSetting* pSetting,
                                Particle* pReferencePtcl, u16 calcRemain) {
    Particle* pParticle =
        mManagerEM->mManagerEF->mManagerES->GetMemoryManager()->AllocParticle();

    if (pParticle == NULL) {
        return NULL;
    }

    if (!pParticle->Initialize(life, pos, vel, this, pSpace, momentum, pSetting,
                               pReferencePtcl)) {
        return NULL;
    }

    pParticle->mCalcRemain += calcRemain;
    mActivityList.ToActive(pParticle);
    pParticle->mLifeStatus = NW4R_EF_LS_ACTIVE;

    return pParticle;
}

void ParticleManager::Calc() {
    Particle* pFirst = static_cast<Particle*>(
        ut::List_GetNext(&mActivityList.mActiveList, mLastCalced));

    if (pFirst == NULL) {
        return;
    }

    Particle* pIt = pFirst;

    if (mManagerEM->mManagerEF->mCallBack.mPrevPtclCalc != NULL) {
        mManagerEM->mManagerEF->mCallBack.mPrevPtclCalc(
            this, &mActivityList.mActiveList, pFirst);
    }

    math::MTX34 mtxLocToGlb;
    CalcGlobalMtx(&mtxLocToGlb);

    math::MTX34 mtxGlbToLoc;
    math::MTX34Inv(&mtxGlbToLoc, &mtxLocToGlb);

    math::MTX34 mtxEmToGlb;
    mManagerEM->CalcGlobalMtx(&mtxEmToGlb);

    math::VEC3 axisSum(
        (mtxEmToGlb._00 + mtxEmToGlb._01) + mtxEmToGlb._02,
        (mtxEmToGlb._10 + mtxEmToGlb._11) + mtxEmToGlb._12,
        (mtxEmToGlb._20 + mtxEmToGlb._21) + mtxEmToGlb._22);

    f32 emitterScale = 0.57735026f * math::VEC3Len(&axisSum);

    math::MTX34 mtxEmToLoc;
    math::MTX34Mult(&mtxEmToLoc, &mtxGlbToLoc, &mtxEmToGlb);

    math::MTX34 mtxLocToEm;
    math::MTX34Inv(&mtxLocToEm, &mtxEmToLoc);

    math::MTX34 mtxGlbToLocNoTrans = mtxGlbToLoc;
    mtxGlbToLocNoTrans._03 = 0.0f;
    mtxGlbToLocNoTrans._13 = 0.0f;
    mtxGlbToLocNoTrans._23 = 0.0f;

    math::MTX34 mtxLocToGlbNoTrans;
    math::MTX34Inv(&mtxLocToGlbNoTrans, &mtxGlbToLocNoTrans);

    math::MTX34 mtxLocToEmNoTrans = mtxLocToEm;
    mtxLocToEmNoTrans._03 = 0.0f;
    mtxLocToEmNoTrans._13 = 0.0f;
    mtxLocToEmNoTrans._23 = 0.0f;

    math::MTX34 mtxEmToLocNoTrans = mtxEmToLoc;
    mtxEmToLocNoTrans._03 = 0.0f;
    mtxEmToLocNoTrans._13 = 0.0f;
    mtxEmToLocNoTrans._23 = 0.0f;

    Particle* pNext;

    for (; pIt != NULL; pIt = pNext) {
        pNext = static_cast<Particle*>(
            NW4R_UT_LIST_GET_LINK(mActivityList.mActiveList, pIt)->nextObject);

        if (pIt->mLifeStatus != NW4R_EF_LS_ACTIVE) {
            continue;
        }

        if (pIt->mEvalStatus != NW4R_EF_ES_WAIT) {
            continue;
        }

        pIt->mEvalStatus = NW4R_EF_ES_DONE;

        if (pIt->mCalcRemain != 0) {
            mManagerEM->mManagerEF->SetFlagExistCalcRemain(true);
        }

        math::VEC3 prevPos = pIt->mParameter.mPosition;
        math::VEC3 prevVel = pIt->mParameter.mVelocity;

        math::VEC3 prevDir;
        pIt->GetMoveDir(&prevDir);

        pIt->mParameter.mPrevPosition = pIt->mParameter.mPosition;

        if (pIt->mLife <= pIt->mTick) {
            RetireParticle(pIt);
            continue;
        }

        math::VEC3 addVel(0.0f, 0.0f, 0.0f);
        math::VEC3 addPos(0.0f, 0.0f, 0.0f);
        BOOL hasPostField = false;
        detail::ParticlePostFieldTransform postTransform;
        u16 i;
        PostFieldInfo* pPostField;
        EmitterResource* pPostResource = nullptr;

        for (i = pIt->mTick == 0 ? 0 : mResource->NumPtclInitTrack();
             i < mResource->NumPtclTrack(); i++) {
            u8* pTrack = mResource->GetPtclTrack(i);
            AnimCurveHeader* pHeader = reinterpret_cast<AnimCurveHeader*>(pTrack);
            if (pHeader->processFlag & AnimCurveHeader::PROC_FLAG_STOP) {
                continue;
            }

            u32 tick;
            u32 life;
            u16 seed;
            if (pHeader->processFlag & AnimCurveHeader::PROC_FLAG_TIMING) {
                Emitter* pEmitter = pIt->mParticleManager->mManagerEM;
                tick = pEmitter->mTick;
                if (pEmitter->mParameter.mComFlags & EmitterDesc::CMN_FLAG_MAX_LIFE) {
                    life = 0xFFFFFFFF;
                } else {
                    life = pEmitter->mParameter.mEmitSpan;
                }
                seed = pEmitter->mRandSeed;
            } else {
                tick = pIt->mTick;
                life = pIt->mLife;
                seed = pIt->mRandSeed;
            }

            if (static_cast<u8>(pHeader->magic + 0x55) > 1) {
                continue;
            }
            const u8 kind = pHeader->kindType;
            switch (pHeader->curveFlag) {
            case AC_TYPE_PARTICLE_TEXTURE:
                AnimCurveExecuteTexture(pTrack, pIt, tick, seed, life);
                break;
            case AC_TYPE_CHILD:
                AnimCurveExecuteChild(pTrack, pIt, tick, seed, life);
                break;
            case AC_TYPE_PARTICLE_ROTATE:
                AnimCurveExecuteRotate(pTrack, &pIt->mParameter.mRotate.x,
                                       tick, seed, life);
                break;
            case AC_TYPE_PARTICLE_U8:
                AnimCurveExecuteU8(pTrack,
                    reinterpret_cast<u8*>(&pIt->mParameter) + kind,
                    tick, seed, life);
                break;
            case AC_TYPE_PARTICLE_F32:
                AnimCurveExecuteF32(pTrack,
                    reinterpret_cast<f32*>(reinterpret_cast<u8*>(&pIt->mParameter) + kind),
                    tick, seed, life);
                break;
            case detail::ParticleField::CURVE_TYPE: {
                detail::ParticleField field;
                memcpy(&field, pTrack + pHeader->keyTable + sizeof(AnimCurveHeader) +
                    pHeader->rangeTable +
                    pHeader->randomTable + pHeader->nameTable, sizeof(field));
                if (pHeader->keyTable != 0) {
                    AnimCurveExecuteF32(pTrack, &field.mPower, tick, seed, life);
                }
                math::VEC3 relativePosition;
                if (kind == detail::ParticleField::SPIN ||
                    static_cast<u8>(kind + 0xFE) <= 2) {
                    relativePosition = detail::CalcFieldPosition(field.mSpace == 1,
                        &mtxLocToEm, &mtxLocToGlb, &mtxEmToGlb, &prevPos);
                }

                math::VEC3 affect(0.0f, 0.0f, 0.0f);
                switch (kind) {
                case detail::ParticleField::SPEED:
                    detail::CalcFieldSpeed(&affect, &field, &prevVel);
                    break;
                case detail::ParticleField::GRAVITY:
                    detail::CalcFieldGravity(&affect, &field);
                    break;
                case detail::ParticleField::RANDOM:
                    detail::CalcFieldRandom(&affect, &field, pHeader, pIt, tick, seed, &prevVel, &prevDir, &mtxLocToEmNoTrans);
                    break;
                case detail::ParticleField::SPIN:
                    detail::CalcFieldSpin(&affect, &field, &relativePosition);
                    break;
                case detail::ParticleField::MAGNET:
                    detail::CalcFieldMagnet(&affect, &field, &relativePosition);
                    break;
                case detail::ParticleField::NEWTON:
                    detail::CalcFieldNewton(&affect, &field, &relativePosition);
                    break;
                case detail::ParticleField::VORTEX:
                    detail::CalcFieldVortex(&affect, &field, &relativePosition);
                    break;
                case detail::ParticleField::TAIL:
                    if (__fpclassifyf(mManagerEM->mPrevGlobalTranslate.x) != FP_NAN) {
                        math::MTX34 emitterMtx;
                        mManagerEM->CalcGlobalMtx(&emitterMtx);
                        math::VEC3 origin(emitterMtx._03, emitterMtx._13, emitterMtx._23);
                        math::VEC3Sub(&affect, &origin, &mManagerEM->mPrevGlobalTranslate);
                        math::VEC3Scale(&affect, &affect, field.mPower);
                    }
                    break;
                }

                switch (field.mSpace) {
                case 1:
                    math::VEC3Transform(&affect, &mtxEmToLocNoTrans, &affect);
                    break;
                case 0:
                    math::VEC3Transform(&affect, &mtxGlbToLocNoTrans, &affect);
                    break;
                case 3:
                    affect *= emitterScale;
                    math::VEC3Transform(&affect, &mtxGlbToLocNoTrans, &affect);
                    break;
                }
                switch (field.mAddTo) {
                case 0:
                    addVel += affect;
                    break;
                case 1:
                    addPos += affect;
                    break;
                }
                break;
            }
            case detail::POST_FIELD_CURVE:
                if (pHeader->infoTable != 0) {
                    CurveOffset nameSize;
                    CurveOffset rangeSize;
                    CurveOffset randomSize;
                    AnimCurveNameTable* pNames =
                        GetPostFieldNames(pHeader, rangeSize, randomSize);
                    GetPostFieldNameSize(pHeader, nameSize);
                    pPostField = reinterpret_cast<PostFieldInfo*>(
                        reinterpret_cast<u8*>(pNames) + nameSize);
                    memcpy(&postTransform, pPostField, sizeof(postTransform));
                    if (pHeader->nameTable > 4) {
                        pPostResource = reinterpret_cast<EmitterResource*>(pNames->datas[pPostField->mEffectIndex].work);
                    }
                }
                if (pHeader->keyTable != 0) {
                    AnimCurveExecuteF32(pTrack,
                        reinterpret_cast<f32*>(reinterpret_cast<u8*>(&postTransform) + kind),
                        tick, seed, life);
                }
                hasPostField = true;
                break;
            }
        }

        if (hasPostField) {
            math::VEC3 velocity = pIt->mParameter.mVelocity;
            velocity += addVel;
            bool killed = false;
            int moved = CalcPostFieldParticle(pIt, &postTransform,
                pPostField, pPostResource,
                &mtxEmToGlb, &mtxGlbToLoc, &prevPos, addPos, &velocity, &killed);
            if (killed) {
                RetireParticle(pIt);
                continue;
            }
            if (moved) {
                pIt->mParameter.mVelocity = velocity;
                pIt->AddPosition(&addPos);
                pIt->AddPosition(&pIt->mParameter.mVelocity);
            }
            if (pPostField->mWrapFlags & 1) {
                math::MTX34 region;
                math::MTX34Identity(&region);
                MTX34ScaleNonZero(&region, &pPostField->mWrapScale, &region);
                math::MTX34 rotation;
                PSMTX34RotXYZRad(&rotation, pPostField->mWrapRotate.x,
                    pPostField->mWrapRotate.y, pPostField->mWrapRotate.z);
                math::MTX34Mult(&region, &rotation, &region);
                math::MTX34Trans(&region, &pPostField->mWrapTranslate, &region);
                if (pPostField->mWrapFlags & 2) {
                    math::VEC3 origin(mtxEmToGlb._03, mtxEmToGlb._13, mtxEmToGlb._23);
                    math::MTX34Trans(&region, &origin, &region);
                }
                math::MTX34Mult(&region, &mtxGlbToLoc, &region);
                math::MTX34 inverse;
                math::MTX34Inv(&inverse, &region);
                math::VEC3 local;
                math::VEC3Transform(&local, &inverse, &pIt->mParameter.mPosition);
                BOOL wrapped = false;
                if (local.x > 1.0f) {
                    local.x = math::FMod(1.0f + local.x, 2.0f) - 1.0f;
                    wrapped = true;
                } else if (local.x < -1.0f) {
                    local.x = 1.0f + math::FMod(local.x - 1.0f, 2.0f);
                    wrapped = true;
                }
                if (local.y > 1.0f) {
                    local.y = math::FMod(1.0f + local.y, 2.0f) - 1.0f;
                    wrapped = true;
                } else if (local.y < -1.0f) {
                    local.y = 1.0f + math::FMod(local.y - 1.0f, 2.0f);
                    wrapped = true;
                }
                if (local.z > 1.0f) {
                    local.z = math::FMod(1.0f + local.z, 2.0f) - 1.0f;
                    wrapped = true;
                } else if (local.z < -1.0f) {
                    local.z = 1.0f + math::FMod(local.z - 1.0f, 2.0f);
                    wrapped = true;
                }
                if (wrapped) {
                    math::VEC3Transform(&pIt->mParameter.mPosition, &region, &local);
                }
            }
        } else {
            pIt->mParameter.mVelocity += addVel;
            pIt->AddPosition(&addPos);
            pIt->AddPosition(&pIt->mParameter.mVelocity);
        }

        pIt->mTick++;
    }

    mLastCalced =
        static_cast<Particle*>(ut::List_GetLast(&mActivityList.mActiveList));

    if (mManagerEM->mManagerEF->mCallBack.mPostPtclCalc != NULL) {
        mManagerEM->mManagerEF->mCallBack.mPostPtclCalc(
            this, &mActivityList.mActiveList, pFirst);
    }
}

void ParticleManager::Draw(const DrawInfo& rInfo) {
    const EmitterDesc* pDesc = mResource->GetEmitterDesc();

    if ((pDesc->drawSetting.mFlags & EmitterDrawSetting::FLAG_HIDDEN) ||
        (mManagerEM->mParameter.mComFlags &
         EmitterDesc::CMN_FLAG_DISABLE_DRAW)) {
        return;
    }

    DrawInfo info(rInfo);

    f32 zOffset = pDesc->drawSetting.zOffset;
    if (zOffset != 0.0f) {
        math::MTX34 mtx;
        mManagerEM->CalcGlobalMtx(&mtx);

        math::VEC3 pos(mtx._03, mtx._13, mtx._23);
        info.SetZOffset(zOffset, pos);
    }

    mDrawStrategy->Draw(info, this);
}

math::MTX34* ParticleManager::CalcGlobalMtx(math::MTX34* pResult) {
    if (mMtxDirty) {
        math::MTX34 orig;
        mManagerEM->CalcGlobalMtx(&orig);

        mManagerEM->RestructMatrix(&mMtx, &orig, mFlag & FLAG_MTX_INHERIT_SCALE,
                                   mFlag & FLAG_MTX_INHERIT_ROT,
                                   mInheritTranslate,
                                   mFlag & FLAG_MTX_INHERIT_PIVOT);

        mMtxDirty = false;
    }

    *pResult = mMtx;
    return pResult;
}

void ParticleManager::BeginCalc(bool onlyIfRemain) {
    mLastCalced = NULL;

    Particle* pIt =
        static_cast<Particle*>(mActivityList.mActiveList.headObject);

    for (; pIt != NULL; pIt = static_cast<Particle*>(
            NW4R_UT_LIST_GET_LINK(mActivityList.mActiveList, pIt)->nextObject))

    {
        if (!onlyIfRemain || pIt->mCalcRemain != 0) {
            if (pIt->mCalcRemain != 0) {
                pIt->mCalcRemain--;
            }

            if (pIt->GetLifeStatus() == NW4R_EF_LS_ACTIVE &&
                pIt->mEvalStatus == NW4R_EF_ES_DONE) {
                pIt->mEvalStatus = NW4R_EF_ES_WAIT;
            }
        }
    }
}

void ParticleManager::EndCalc() {
    Particle* pIt =
        static_cast<Particle*>(mActivityList.mActiveList.headObject);

    for (; pIt != NULL; pIt = static_cast<Particle*>(
            NW4R_UT_LIST_GET_LINK(mActivityList.mActiveList, pIt)->nextObject))

    {
        if (pIt->GetLifeStatus() == NW4R_EF_LS_ACTIVE &&
            pIt->mEvalStatus == NW4R_EF_ES_SKIP) {
            pIt->mEvalStatus = NW4R_EF_ES_DONE;
        }
    }
}

void ParticleManager::BeginDraw() {
    math::MTX34 emMtx;
    math::MTX34 pmMtx;

    mManagerEM->CalcGlobalMtx(&emMtx);
    CalcGlobalMtx(&pmMtx);

    math::MTX34Inv(&emMtx, &emMtx);
    math::MTX34Mult(&smDrawMtxPMtoEM, &emMtx, &pmMtx);
}

const math::MTX34* ParticleManager::Draw_GetMtxPMtoEM() const {
    return &smDrawMtxPMtoEM;
}

void ParticleManager::EndDraw() {}

void ParticleManager::Draw_ModifyColor(Particle* pParticle, GXColor* pColorPri,
                                       GXColor* pColorSec) {
    switch (mModifier.mLight.mType) {
    case ParticleModifier::SIMPLELIGHT_AMBIENT: {
        pColorPri->r = (pColorPri->r * mModifier.mLight.mAmbient.r + 128) >> 8;
        pColorPri->g = (pColorPri->g * mModifier.mLight.mAmbient.g + 128) >> 8;
        pColorPri->b = (pColorPri->b * mModifier.mLight.mAmbient.b + 128) >> 8;
        pColorPri->a = (pColorPri->a * mModifier.mLight.mAmbient.a + 128) >> 8;

        pColorSec->r = (pColorSec->r * mModifier.mLight.mAmbient.r + 128) >> 8;
        pColorSec->g = (pColorSec->g * mModifier.mLight.mAmbient.g + 128) >> 8;
        pColorSec->b = (pColorSec->b * mModifier.mLight.mAmbient.b + 128) >> 8;
        pColorSec->a = (pColorSec->a * mModifier.mLight.mAmbient.a + 128) >> 8;
        break;
    }

    case ParticleModifier::SIMPLELIGHT_DIFFUSE: {
        if (mModifier.mLight.mRadius < NW4R_MATH_FLT_EPSILON) {
            pColorPri->r = (pColorPri->r * mModifier.mLight.mAmbient.r + 128) >> 8;
            pColorPri->g = (pColorPri->g * mModifier.mLight.mAmbient.g + 128) >> 8;
            pColorPri->b = (pColorPri->b * mModifier.mLight.mAmbient.b + 128) >> 8;
            pColorPri->a = (pColorPri->a * mModifier.mLight.mAmbient.a + 128) >> 8;

            pColorSec->r = (pColorSec->r * mModifier.mLight.mAmbient.r + 128) >> 8;
            pColorSec->g = (pColorSec->g * mModifier.mLight.mAmbient.g + 128) >> 8;
            pColorSec->b = (pColorSec->b * mModifier.mLight.mAmbient.b + 128) >> 8;
            pColorSec->a = (pColorSec->a * mModifier.mLight.mAmbient.a + 128) >> 8;
        } else {
            const math::MTX34* pMtxPMtoEM = Draw_GetMtxPMtoEM();

            math::VEC3 pos;
            math::VEC3Transform(&pos, pMtxPMtoEM,
                                &pParticle->mParameter.mPosition);

            math::VEC3Sub(&pos, &pos, &mModifier.mLight.mPosition);
            f32 dist = math::VEC3Len(&pos);

            if (dist > mModifier.mLight.mRadius) {
                pColorPri->r = (pColorPri->r * mModifier.mLight.mAmbient.r + 128) >> 8;
                pColorPri->g = (pColorPri->g * mModifier.mLight.mAmbient.g + 128) >> 8;
                pColorPri->b = (pColorPri->b * mModifier.mLight.mAmbient.b + 128) >> 8;
                pColorPri->a = (pColorPri->a * mModifier.mLight.mAmbient.a + 128) >> 8;

                pColorSec->r = (pColorSec->r * mModifier.mLight.mAmbient.r + 128) >> 8;
                pColorSec->g = (pColorSec->g * mModifier.mLight.mAmbient.g + 128) >> 8;
                pColorSec->b = (pColorSec->b * mModifier.mLight.mAmbient.b + 128) >> 8;
                pColorSec->a = (pColorSec->a * mModifier.mLight.mAmbient.a + 128) >> 8;
            } else {
                s32 attn = (256 * dist) / mModifier.mLight.mRadius;

                u16 lr = mModifier.mLight.mDiffuse.r * 256 +
                         attn * (mModifier.mLight.mAmbient.r -
                                 mModifier.mLight.mDiffuse.r);

                u16 lg = mModifier.mLight.mDiffuse.g * 256 +
                         attn * (mModifier.mLight.mAmbient.g -
                                 mModifier.mLight.mDiffuse.g);

                u16 lb = mModifier.mLight.mDiffuse.b * 256 +
                         attn * (mModifier.mLight.mAmbient.b -
                                 mModifier.mLight.mDiffuse.b);

                u16 la = mModifier.mLight.mDiffuse.a * 256 +
                         attn * (mModifier.mLight.mAmbient.a -
                                 mModifier.mLight.mDiffuse.a);

                pColorPri->r = (pColorPri->r * lr + 128) >> 16;
                pColorPri->g = (pColorPri->g * lg + 128) >> 16;
                pColorPri->b = (pColorPri->b * lb + 128) >> 16;
                pColorPri->a = (pColorPri->a * la + 128) >> 16;

                pColorSec->r = (pColorSec->r * lr + 128) >> 16;
                pColorSec->g = (pColorSec->g * lg + 128) >> 16;
                pColorSec->b = (pColorSec->b * lb + 128) >> 16;
                pColorSec->a = (pColorSec->a * la + 128) >> 16;
            }
        }
        break;
    }

    default: {
        break;
    }
    }
}
}
}
