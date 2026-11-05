#define CMODEL_THUNK_EMISSION_LAYOUT
#pragma RTTI off

#include <memory>
namespace nw4r { namespace g3d { class AnmObj; } }
enum { kModelAnimationPointerSize = sizeof(std::tr1::shared_ptr<nw4r::g3d::AnmObj>) };
#include <game/graphics/CModel.h>
#include <game/graphics/ScnMdlSimpleDataInline.h>
#include <game/core/CFiber.h>
#include <game/io/CFileData.h>
#include <game/animation/AnmObjChrRootScale.h>
#include <game/core/FrameStep.h>
#include <game/core/Memory.h>
#include <nw4r/g3d/res/g3d_resfile.h>
#include <nw4r/g3d/res/g3d_resmdl.h>
#include <nw4r/g3d/res/g3d_resnode.h>
#include <algorithm>
#include <math.h>

inline CModel::~CModel() { Unload(); }
inline void CModel::ExecCallbackA(nw4r::g3d::ChrAnmResult* pResult, nw4r::g3d::ResMdl mdl, nw4r::g3d::FuncObjCalcWorld* pFuncObj) {
    if (mAim.mpModel != nullptr) {
        if (mAnimationControlsAim) {
            nw4r::math::MTX34 rotTrans;
            pResult->GetRotTrans(&rotTrans);
            nw4r::math::MTX34 inverse;
            if (nw4r::math::MTX34Inv(&inverse, &mAim.mBindTransform)) {
                nw4r::math::MTX34 combined;
                PSMTXConcat(inverse, rotTrans, combined);
                nw4r::math::VEC3 direction;
                nw4r::math::VEC3 forward;
                forward.x = 0.0f;
                forward.y = 0.0f;
                forward.z = 1.0f;
                nw4r::math::VEC3Transform(&direction, &combined, &forward);
                if (direction.x != 0.0f || direction.z != 0.0f) {
                    mAim.mYaw = nw4r::math::Atan2Rad(direction.x, direction.z);
                    mAim.mPitch = asin(direction.y);
                }
            }
        } else {
            if (mAimTargetPending) {
                mAimTargetPending = false;
                mAim.AimAtPoint(mAimTarget, 1000);
            }
            nw4r::math::MTX34 rotation;
            nw4r::math::MTX34RotXYZRad(&rotation, -mAim.mPitch, mAim.mYaw, 0.0f);
            PSMTXConcat(mAim.mBindTransform, rotation, rotation);
            pResult->SetRotTrans(&rotation);
        }
    }
}

#pragma dont_inline on
void RequestModelThunks(CModel* pModel, const nw4r::g3d::ResMdl& rModel) {
    pModel->CModel::ExecCallbackA(nullptr, rModel, nullptr);
    pModel->CModel::~CModel();
}
#pragma dont_inline off

namespace std {
template <> vector<tr1::shared_ptr<nw4r::g3d::AnmObj> >::~vector();
}
