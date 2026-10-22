#include <game/ModelAim.h>
#include <game/MathUtil.h>

#include <math.h>
#include <nw4r/math/math_arithmetic.h>
#include <nw4r/math/math_triangular.h>
#include <revolution/mtx.h>

ModelAim::Parameters ModelAim::sDefaultParameters;

namespace {
struct DefaultParametersInit {
    DefaultParametersInit() {
        ModelAim::Parameters& r = ModelAim::sDefaultParameters;
        r.maxPitchStep = 0.017453292f;
        r.maxYawStep = 0.017453292f;
        f32 keepRate = 0.9f;
        r.pitchFollowRate = 1.0f - keepRate;
        r.yawFollowRate = 1.0f - keepRate;
        r.pitchInnerMin = 1.0471976f;
        r.pitchInnerMax = -0.34906584f;
        r.pitchOuterMin = 1.5707964f;
        r.pitchOuterMax = -1.3962634f;
        r.yawInnerMin = 1.3962634f;
        r.yawInnerMax = -1.3962634f;
        r.yawOuterMin = 3.1415927f;
        r.yawOuterMax = -3.1415927f;
    }
} sDefaultParametersInit;
}

ModelAim::ModelAim() : mpModel(nullptr), mLimitEnabled(true), mpParameters(&sDefaultParameters) {}

ModelAim::~ModelAim() {
    if (mpParameters != &sDefaultParameters) {
        delete mpParameters;
    }
}

bool ModelAim::Initialize(nw4r::g3d::ScnMdlSimple* pModel, nw4r::g3d::ResNode node) {
    mpModel = nullptr;
    if (!node.IsValid()) {
        return false;
    }
    mpModel = pModel;
    nw4r::g3d::ResNode parent = node.GetParentNode();
    if (parent.IsValid()) {
        mParentNodeId = parent.GetMtxID();
    } else {
        mParentNodeId = -1;
    }
    mPitch = 0.0f;
    mYaw = 0.0f;
    nw4r::math::MTX34RotXYZDeg(&mBindTransform, node.ref().rot.x, node.ref().rot.y, node.ref().rot.z);
    PSMTXTransApply(mBindTransform, mBindTransform, node.ref().translate.x, node.ref().translate.y, node.ref().translate.z);
    return true;
}

void ModelAim::SetParameters(const Parameters& rParameters) {
    if (mpParameters == &sDefaultParameters) {
        mpParameters = new Parameters(rParameters);
    } else {
        *mpParameters = rParameters;
    }
}

void ModelAim::AimAtPoint(const nw4r::math::VEC3& rTarget, int steps) {
    bool skip = true;
    if (mpModel != nullptr) {
        bool idle = !mLimitEnabled && mYaw == 0.0f && mPitch == 0.0f;
        if (!idle) {
            skip = false;
        }
    }
    if (skip) {
        return;
    }
    nw4r::math::MTX34 transform;
    if (mParentNodeId == -1) {
        transform = mBindTransform;
    } else {
        PSMTXConcat(mpModel->GetWldMtxArray()[mParentNodeId], mBindTransform, transform);
    }
    nw4r::math::MTX34 inverse;
    if (PSMTXInverse(transform, inverse)) {
        nw4r::math::VEC3 local;
        nw4r::math::VEC3Transform(&local, &inverse, &rTarget);
        Update(local, steps);
    }
}

void ModelAim::AimAtMatrixOrigin(const nw4r::math::MTX34& rMatrix, int steps) {
    bool skip = true;
    if (mpModel != nullptr) {
        bool idle = !mLimitEnabled && mYaw == 0.0f && mPitch == 0.0f;
        if (!idle) {
            skip = false;
        }
    }
    if (skip) {
        return;
    }
    nw4r::math::MTX34 transform;
    nw4r::math::MTX34 combined;
    nw4r::math::MTX34 inverse;
    nw4r::math::VEC3 point;
    if (mParentNodeId == -1) {
        transform = mBindTransform;
    } else {
        PSMTXConcat(mpModel->GetWldMtxArray()[mParentNodeId], mBindTransform, transform);
    }
    PSMTXConcat(rMatrix, transform, combined);
    if (PSMTXInverse(combined, inverse)) {
        point.x = 0.0f;
        point.y = 0.0f;
        point.z = 0.0f;
        nw4r::math::VEC3Transform(&point, &inverse, &point);
        Update(point, steps);
    }
}

void ModelAim::Update(const nw4r::math::VEC3& rLocalTarget, int steps) {
    f32 yaw = nw4r::math::Atan2Rad(rLocalTarget.x, rLocalTarget.z);
    f32 pitch = nw4r::math::Atan2Rad(rLocalTarget.y, nw4r::math::FSqrt(rLocalTarget.x * rLocalTarget.x + rLocalTarget.z * rLocalTarget.z));
    if (mLimitEnabled && mpParameters->yawOuterMax <= yaw && yaw <= mpParameters->yawOuterMin && mpParameters->pitchOuterMax <= pitch && pitch <= mpParameters->pitchOuterMin) {
        if (mpParameters->yawOuterMax <= yaw && yaw < mpParameters->yawInnerMax) {
            yaw = mpParameters->yawInnerMax;
        } else if (mpParameters->yawInnerMin <= yaw && yaw <= mpParameters->yawOuterMin) {
            yaw = mpParameters->yawInnerMin;
        }
        if (mpParameters->pitchOuterMax <= pitch && pitch < mpParameters->pitchInnerMax) {
            pitch = mpParameters->pitchInnerMax;
        } else if (mpParameters->pitchInnerMin <= pitch && pitch <= mpParameters->pitchOuterMin) {
            pitch = mpParameters->pitchInnerMin;
        }
    } else {
        pitch = 0.0f;
        yaw = pitch;
    }
    f32 pitchDelta = pitch - mPitch;
    f32 pitchFollow;
    if (steps == 1) {
        pitchFollow = fabs(pitchDelta * mpParameters->pitchFollowRate);
    } else {
        pitchFollow = fabs(pitchDelta * IntegerPower(1.0f - mpParameters->pitchFollowRate, steps) - pitchDelta);
    }
    f32 pitchStep = Max(pitchFollow, mpParameters->maxPitchStep * steps);
    if (-pitchStep <= pitchDelta && pitchDelta <= pitchStep) {
        mPitch = pitch;
    } else if (0.0f < pitchDelta) {
        mPitch += pitchStep;
    } else {
        mPitch -= pitchStep;
    }
    f32 yawDelta = yaw - mYaw;
    f32 yawFollow;
    if (steps == 1) {
        yawFollow = fabs(yawDelta * mpParameters->yawFollowRate);
    } else {
        yawFollow = fabs(yawDelta * IntegerPower(1.0f - mpParameters->yawFollowRate, steps) - yawDelta);
    }
    f32 yawStep = Max(yawFollow, mpParameters->maxYawStep * steps);
    if (-yawStep <= yawDelta && yawDelta <= yawStep) {
        mYaw = yaw;
    } else if (0.0f < yawDelta) {
        mYaw += yawStep;
    } else {
        mYaw -= yawStep;
    }
}
