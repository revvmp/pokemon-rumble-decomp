#include <game/CStageCamera.h>
#include <game/BWorld.h>
#include <game/IUIInput.h>
#include <game/MathUtil.h>
#include <game/RenderMode.h>
#include <nw4r/g3d/g3d_light.h>
#include <nw4r/math/math_triangular.h>

CStageCamera::CStageCamera(u32 type) : BWorldCamera(type | TYPE), mReserved(0) {}

CStageCamera::~CStageCamera() {}

void CStageCamera::LoadScript(int mode, const std::tr1::shared_ptr<NPpu::CEnvironment>& rEnv) {
    mEngine.reset(new CPpuEngine);
    mEngine->Load(this, "CStageCamera.pkc", mode, rEnv);
}

f32 CStageCamera::GetStickWorldAngle(const IUIInput* pInput) {
    if (pInput == nullptr || !pInput->IsConnected() || !(0.0f < pInput->GetStickLength())) {
        return 0.0f;
    }

    nw4r::math::MTX34 viewMtx;
    GetG3dCamera().GetCameraMtx(&viewMtx);

    nw4r::math::MTX34 invViewMtx;
    if (!nw4r::math::MTX34Inv(&invViewMtx, &viewMtx)) {
        return 0.0f;
    }

    nw4r::math::VEC3 eye(0.0f, 0.0f, 0.0f);
    eye = Transform(invViewMtx, eye);

    f32 sin;
    f32 cos;
    nw4r::math::SinCosFIdx(&sin, &cos, (256.0f / 360.0f) * (90.0f + pInput->GetStickAngle()));

    nw4r::math::VEC3 dir(sin, 0.0f, cos);
    dir = Transform(invViewMtx, dir) - eye;

    return NormalizeSignedAngle(nw4r::math::Atan2Deg(dir.x, dir.z));
}

void CStageCamera::UpdateLightPosition() {
    nw4r::g3d::LightSetting& rSetting = GetWorld()->GetScnRoot()->GetLightSetting();

    for (int i = 0; i < static_cast<int>(rSetting.GetNumLightSet()); i++) {
        nw4r::g3d::LightSet lightSet = rSetting.GetLightSet(i);

        for (u32 j = 0;; j++) {
            nw4r::g3d::LightObj* pLight = lightSet.GetLightObj(j);
            if (pLight == nullptr) {
                break;
            }

            if (pLight->IsSpotLight()) {
                nw4r::math::VEC3 pos;
                pLight->GetLightPos(&pos);
                pos.x = mPosition.x;
                pos.z = mPosition.z;
                pLight->InitLightPos(pos.x, pos.y, pos.z);
            }
        }
    }
}

void CStageCamera::Calc() {
    BWorldCamera::Calc();
    UpdateLightPosition();

    if (mEngine) {
        mEngine->Calc();
    }
}

void CStageCamera::UpdateIndex(int prevIndex) {
    if (GetIndex() < 0) {
        return;
    }

    nw4r::g3d::Camera camera = GetG3dCamera();
    camera.SetPosition(mPosition);

    nw4r::g3d::Camera::PostureInfo posture;
    posture.tp = nw4r::g3d::Camera::POSTURE_ROTATE;
    posture.cameraRotate = nw4r::math::VEC3(-45.0f, 0.0f, 0.0f);
    camera.SetPosture(posture);

    camera.SetPerspective(20.2561665f, GetDisplayAspectRatio(), 0.01f, 1000.0f);
}

void CStageCamera::OnLeaveWorld(BWorld* pWorld) {
    BWorldCamera::OnLeaveWorld(pWorld);

    if (mEngine) {
        mEngine->Reset();
    }
}
