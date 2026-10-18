#ifndef NW4R_EF_PARTICLE_MANAGER_FIELD_H
#define NW4R_EF_PARTICLE_MANAGER_FIELD_H

#include <nw4r/ef/ef_res_animcurve.h>
#include <nw4r/ef/ef_particle.h>
#include <nw4r/ef/ef_util.h>
#include <nw4r/math/math_types.h>

namespace nw4r {
namespace ef {

class Particle;
struct EmitterResource;
struct PostFieldInfo;

namespace detail {

struct ParticleField {
    enum { CURVE_TYPE = 7 };

    enum Type {
        GRAVITY,
        SPEED,
        MAGNET,
        NEWTON,
        VORTEX,
        SPIN = 6,
        RANDOM,
        TAIL
    };

    u8 mSpace;
    u8 mAddTo;
    u8 mReserved;
    u8 mFlags;
    f32 mPower;

    union Parameters {
        math::_VEC3 mRotation;
        math::_VEC3 mPosition;
        struct Newton {
            f32 mDistance;
            math::_VEC3 mPosition;
        } mNewton;
        struct Vortex {
            f32 mOuterPower;
            f32 mDistance;
            math::_VEC3 mRotation;
        } mVortex;
        struct Random {
            f32 mDiffusion;
            u16 mInterval;
            u8 mReserved[14];
        } mRandom;
    } mParams;
};

struct ParticlePostFieldTransform {
    math::VEC3 mScale;
    math::VEC3 mRotate;
    math::VEC3 mTranslate;
};

struct ParticlePostField {
    enum { CURVE_TYPE = 2 };

    ParticlePostFieldTransform mTransform;
    u8 mCollisionParameters[34];
    u16 mEffectIndex;
    u8 mWrapFlags;
    u8 mReserved[3];
    math::VEC3 mWrapScale;
    math::VEC3 mWrapRotate;
    math::VEC3 mWrapTranslate;
};

inline math::VEC3 CalcFieldPosition(bool emitterSpace,
                                    const math::MTX34* pLocalToEmitter,
                                    const math::MTX34* pLocalToGlobal,
                                    const math::MTX34* pEmitterToGlobal,
                                    const math::VEC3* pPosition) {
    math::VEC3 position;
    if (emitterSpace) {
        math::VEC3Transform(&position, pLocalToEmitter, pPosition);
    } else {
        math::VEC3Transform(&position, pLocalToGlobal, pPosition);
        math::VEC3 origin(0.0f, 0.0f, 0.0f);
        math::VEC3Transform(&origin, pEmitterToGlobal, &origin);
        position -= origin;
    }
    return position;
}

inline void CalcFieldSpeed(math::VEC3* pOut, const ParticleField* pField,
                           const math::VEC3* pVelocity) {
    math::VEC3Scale(pOut, pVelocity, pField->mPower - 1.0f);
}

inline void CalcFieldGravity(math::VEC3* pOut, const ParticleField* pField) {
    Rotation2VecY(math::VEC3(pField->mParams.mRotation), pOut);
    *pOut *= pField->mPower;
}

inline void CalcFieldRandom(math::VEC3* pOut, ParticleField* pField,
                            const AnimCurveHeader* pHeader,
                            const Particle* pParticle, u32 tick, u16 seed,
                            const math::VEC3* pVelocity,
                            const math::VEC3* pDirection,
                            const math::MTX34* pVelocityMtx) {
    u32 random = seed * 0x3F81F635 + pHeader->randomSeed * 0x30A74193 +
                 static_cast<u16>(tick) * 0x371097E7 + 0x4BF53;
    u8* pRandom = reinterpret_cast<u8*>(&random);
    pRandom[2] ^= pRandom[3];
    pRandom[1] ^= pRandom[2];
    pRandom[0] ^= pRandom[1];
    if (!(pHeader->processFlag & AnimCurveHeader::PROC_FLAG_TIMING)) {
        if (tick == 0 || tick % (pField->mParams.mRandom.mInterval + 1) != 0) {
            return;
        }
    } else if (pParticle->mTick != 0 &&
               tick % (pField->mParams.mRandom.mInterval + 1) != 0) {
        return;
    }
    if (pField->mFlags & 1) {
        math::VEC3 base;
        switch (pField->mAddTo) {
        case 0:
            base = *pVelocity;
            break;
        case 1:
            if (pParticle->mTick == 0) {
                base = *pVelocity;
            } else {
                base = *pDirection / pParticle->mParameter.mMomentum;
            }
            break;
        }
        math::VEC3Transform(&base, pVelocityMtx, &base);
        pField->mPower = math::VEC3Len(&base);
    }
    if (pField->mFlags & 2) {
        pOut->x = static_cast<f32>(static_cast<s16>(random >> 16)) / 32768.0f;
        random = random * 0x343FD + 0x269EC3;
        pOut->y = static_cast<f32>(static_cast<s16>(random >> 16)) / 32768.0f;
        random = random * 0x343FD + 0x269EC3;
        pOut->z = static_cast<f32>(static_cast<s16>(random >> 16)) / 32768.0f;
        random = random * 0x343FD + 0x269EC3;
        if ((pField->mFlags & 1) && !Normalize(pOut, pOut)) {
            pOut->y = 1.0f;
        }
        *pOut *= pField->mPower;
    } else {
        math::VEC3 direction;
        if (pParticle->mTick == 0) {
            direction = *pVelocity;
        } else {
            direction = *pDirection;
        }
        math::VEC3Transform(&direction, pVelocityMtx, &direction);
        if (!Normalize(&direction, &direction)) {
            direction.y = 1.0f;
        }
        math::MTX34 directionMtx;
        GetDirMtxY(&directionMtx, direction);
        if (0.0f != pField->mParams.mRandom.mDiffusion) {
            f32 tilt = (static_cast<f32>(random >> 16) / 65535.0f) *
                       pField->mParams.mRandom.mDiffusion;
            random = random * 0x343FD + 0x269EC3;
            f32 turn = 2.0f * (3.1415927f *
                       (static_cast<f32>(random >> 16) / 65535.0f));
            random = random * 0x343FD + 0x269EC3;
            _PSSinCosRad(&pOut->x, tilt);
            pOut->z = pOut->x;
            f32 sinTurn;
            f32 cosTurn;
            PSSinCosRad(&sinTurn, &cosTurn, turn);
            pOut->x *= sinTurn;
            pOut->z *= cosTurn;
            if (pField->mFlags & 1) {
                *pOut *= pField->mPower;
            } else {
                *pOut *= (static_cast<f32>(random >> 16) / 65535.0f) * pField->mPower;
            }
        } else {
            pOut->x = 0.0f;
            if (pField->mFlags & 1) {
                pOut->y = pField->mPower;
            } else {
                pOut->y = (static_cast<f32>(static_cast<s16>(random >> 16)) /
                            32768.0f) * pField->mPower;
            }
            pOut->z = 0.0f;
        }
        math::VEC3Transform(pOut, &directionMtx, pOut);
    }
    if (!(pField->mFlags & 4)) {
        pOut->x = 0.0f;
    }
    if (!(pField->mFlags & 8)) {
        pOut->y = 0.0f;
    }
    if (!(pField->mFlags & 16)) {
        pOut->z = 0.0f;
    }
}

inline void CalcFieldSpin(math::VEC3* pOut, const ParticleField* pField,
                            const math::VEC3* pPosition) {
    math::VEC3 axis;
    Rotation2VecY(math::VEC3(pField->mParams.mRotation), &axis);
    math::MTX34 rotation;
    math::MTX34RotAxisRad(&rotation, &axis, pField->mPower);
    math::VEC3Transform(pOut, &rotation, pPosition);
    *pOut -= *pPosition;
}

inline void CalcFieldMagnet(math::VEC3* pOut, const ParticleField* pField,
                            const math::VEC3* pPosition) {
    math::VEC3Sub(pOut, reinterpret_cast<const math::VEC3*>(&pField->mParams.mPosition), pPosition);
    Normalize(pOut, pOut);
    *pOut *= pField->mPower;
}

inline void CalcFieldNewton(math::VEC3* pOut, const ParticleField* pField,
                            const math::VEC3* pPosition) {
    math::VEC3Sub(pOut, reinterpret_cast<const math::VEC3*>(&pField->mParams.mNewton.mPosition), pPosition);
    f32 distanceSq = math::VEC3LenSq(pOut);
    Normalize(pOut, pOut);
    *pOut *= pField->mPower;
    f32 fieldDistanceSq = pField->mParams.mNewton.mDistance * pField->mParams.mNewton.mDistance;
    if (distanceSq > fieldDistanceSq) {
        *pOut *= fieldDistanceSq / distanceSq;
    }
}

inline void CalcFieldVortex(math::VEC3* pOut, ParticleField* pField,
                            const math::VEC3* pPosition) {
    math::VEC3 axis;
    Rotation2VecY(math::VEC3(pField->mParams.mVortex.mRotation), &axis);
    pField->mParams.mVortex.mDistance *= pField->mParams.mVortex.mDistance;
    math::VEC3 radial;
    math::VEC3Scale(&radial, &axis, math::VEC3Dot(&axis, pPosition));
    math::VEC3Sub(&radial, pPosition, &radial);
    f32 distanceSq = math::VEC3LenSq(&radial);
    if (0.0f == distanceSq) {
        return;
    }
    f32 power;
    if (distanceSq >= pField->mParams.mVortex.mDistance) {
        power = pField->mParams.mVortex.mOuterPower;
    } else {
        f32 blend = distanceSq / pField->mParams.mVortex.mDistance;
        power = (1.0f - blend) * pField->mPower +
                blend * pField->mParams.mVortex.mOuterPower;
    }
    Normalize(&radial, &radial);
    math::VEC3Cross(pOut, &radial, &axis);
    *pOut *= power;
}

}

int CalcPostFieldParticle(Particle* pParticle,
                          const detail::ParticlePostFieldTransform* pTransform,
                          PostFieldInfo* pInfo, EmitterResource* pResource,
                          const math::MTX34* pEmitterMtx,
                          const math::MTX34* pManagerInverseMtx,
                          const math::VEC3* pPrevPosition,
                          math::VEC3 positionOffset, math::VEC3* pVelocity,
                          bool* pKilled);

}
}

#endif
