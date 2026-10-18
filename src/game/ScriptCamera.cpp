#include <game/CStageCamera.h>
#include <game/BWorld.h>
#include <game/CPpuEngine.h>
#include <memory>

#pragma dont_inline on

static CStageCamera* PopStageCamera(CPpuEngine* pEngine) {
    return DynamicCast<CStageCamera>(pEngine->PopWorldObj());
}

int NewStageCamera(NPpu::CEngine* pEngine) {
    CPpuEngine* pPpuEngine = static_cast<CPpuEngine*>(pEngine);
    std::tr1::shared_ptr<CStageCamera> camera(new CStageCamera(CStageCamera::TYPE));
    camera->SetOwner(pPpuEngine->GetOwner());

    BWorld* pWorld = pPpuEngine->GetWorld();
    if (pWorld->Add(camera)) {
        pWorld->SetCurrentCamera(camera);
        camera->LoadScript(std::tr1::shared_ptr<NPpu::CEnvironment>());
        pEngine->PushInt(camera->GetId());
    } else {
        pEngine->PushInt(0);
    }

    return 0;
}

int NewWorldCamera(NPpu::CEngine* pEngine) {
    CPpuEngine* pPpuEngine = static_cast<CPpuEngine*>(pEngine);
    std::tr1::shared_ptr<BWorldCamera> camera(new BWorldCamera(BWorldCamera::TYPE));
    camera->SetOwner(pPpuEngine->GetOwner());

    BWorld* pWorld = pPpuEngine->GetWorld();
    if (pWorld->Add(camera)) {
        pWorld->SetCurrentCamera(camera);
        pEngine->PushInt(camera->GetId());
    } else {
        pEngine->PushInt(0);
    }

    return 0;
}

int GetCurrentCamera(NPpu::CEngine* pEngine) {
    BWorldCamera* pCamera = static_cast<CPpuEngine*>(pEngine)->GetWorld()->GetCurrentCamera();
    if (pCamera != nullptr) {
        pEngine->PushInt(pCamera->GetId());
    } else {
        pEngine->PushInt(0);
    }

    return 0;
}

int method_camSetLookAt(NPpu::CEngine* pEngine) {
    int argCount = pEngine->FetchArgCount();
    nw4r::g3d::Camera::PostureInfo info;
    info.cameraUp.z = argCount >= 7 ? pEngine->PopFloat() : 0.0f;
    info.cameraUp.y = argCount >= 6 ? pEngine->PopFloat() : 1.0f;
    info.cameraUp.x = argCount >= 5 ? pEngine->PopFloat() : 0.0f;
    info.cameraTarget.z = pEngine->PopFloat();
    info.cameraTarget.y = pEngine->PopFloat();
    info.cameraTarget.x = pEngine->PopFloat();

    CStageCamera* pCamera = PopStageCamera(static_cast<CPpuEngine*>(pEngine));
    if (pCamera != nullptr) {
        nw4r::g3d::Camera camera = pCamera->GetG3dCamera();
        info.tp = nw4r::g3d::Camera::POSTURE_LOOKAT;
        camera.SetPosture(info);
    }

    pEngine->PushNull();
    return 0;
}

int method_camSetRotate(NPpu::CEngine* pEngine) {
    nw4r::g3d::Camera::PostureInfo info;
    info.cameraRotate.z = pEngine->PopFloat();
    info.cameraRotate.y = pEngine->PopFloat();
    info.cameraRotate.x = pEngine->PopFloat();

    CStageCamera* pCamera = PopStageCamera(static_cast<CPpuEngine*>(pEngine));
    if (pCamera != nullptr) {
        nw4r::g3d::Camera camera = pCamera->GetG3dCamera();
        info.tp = nw4r::g3d::Camera::POSTURE_ROTATE;
        camera.SetPosture(info);
    }

    pEngine->PushNull();
    return 0;
}

int method_camSetAim(NPpu::CEngine* pEngine) {
    int argCount = pEngine->FetchArgCount();
    nw4r::g3d::Camera::PostureInfo info;
    info.cameraTwist = argCount >= 5 ? pEngine->PopFloat() : 0.0f;
    info.cameraTarget.z = pEngine->PopFloat();
    info.cameraTarget.y = pEngine->PopFloat();
    info.cameraTarget.x = pEngine->PopFloat();

    CStageCamera* pCamera = PopStageCamera(static_cast<CPpuEngine*>(pEngine));
    if (pCamera != nullptr) {
        nw4r::g3d::Camera camera = pCamera->GetG3dCamera();
        info.tp = nw4r::g3d::Camera::POSTURE_AIM;
        camera.SetPosture(info);
    }

    pEngine->PushNull();
    return 0;
}
