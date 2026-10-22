#ifndef GAME_MODEL_AIM_H
#define GAME_MODEL_AIM_H
#include <nw4r/g3d/g3d_scnmdlsmpl.h>
#include <nw4r/g3d/res/g3d_resnode.h>
#include <nw4r/math/math_types.h>
class ModelAim {
public:
    struct Parameters {
        f32 maxPitchStep;
        f32 maxYawStep;
        f32 pitchFollowRate;
        f32 yawFollowRate;
        f32 pitchInnerMin;
        f32 pitchInnerMax;
        f32 pitchOuterMin;
        f32 pitchOuterMax;
        f32 yawInnerMin;
        f32 yawInnerMax;
        f32 yawOuterMin;
        f32 yawOuterMax;
    };
    ModelAim();
    ~ModelAim();
    bool Initialize(nw4r::g3d::ScnMdlSimple* pModel, nw4r::g3d::ResNode node);
    void SetParameters(const Parameters& rParameters);
    void AimAtPoint(const nw4r::math::VEC3& rTarget, int steps);
    void AimAtMatrixOrigin(const nw4r::math::MTX34& rMatrix, int steps);
    const Parameters& GetParameters() const { return *mpParameters; }
    static Parameters sDefaultParameters;
private:
    void Update(const nw4r::math::VEC3& rLocalTarget, int steps);

    nw4r::g3d::ScnMdlSimple* mpModel;
    u32 mParentNodeId;
    nw4r::math::MTX34 mBindTransform;
    f32 mYaw;
    f32 mPitch;
    bool mLimitEnabled;
    Parameters* mpParameters;
};
#endif
