#include <game/CDividedGround.h>
#include <game/CPlayerPii.h>
#include <game/CStageCamera.h>
#include <game/BWorld.h>
#include <game/CPlayer.h>
#include <game/CPpuEngine.h>
#include <game/IGameSession.h>
#include <game/MathUtil.h>
#include <game/RenderMode.h>
#include <memory>
#include <nw4r/g3d/g3d_state.h>
#include <nw4r/math/math_geometry.h>
#include <nw4r/math/math_triangular.h>
#include <revolution/sc.h>

#pragma dont_inline on

static CStageCamera* PopStageCamera(CPpuEngine* pEngine) {
    return DynamicCast<CStageCamera>(pEngine->PopWorldObj());
}

int NewStageCamera(NPpu::CEngine* pEngine) {
    CPpuEngine* pPpuEngine = static_cast<CPpuEngine*>(pEngine);
    std::tr1::shared_ptr<CStageCamera> camera(new CStageCamera(CStageCamera::TYPE));
    camera->SwapRegister(pPpuEngine->GetRegister());

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
    camera->SwapRegister(pPpuEngine->GetRegister());

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

int method_camSetFovy(NPpu::CEngine* pEngine) {
    f32 fovy = pEngine->PopFloat();

    CStageCamera* pCamera = PopStageCamera(static_cast<CPpuEngine*>(pEngine));
    if (pCamera != nullptr) {
        nw4r::g3d::Camera camera = pCamera->GetG3dCamera();
        nw4r::g3d::CameraData& rData = camera.ref();
        camera.SetPerspective(fovy, rData.projAspect, rData.projNear, rData.projFar);
    }

    pEngine->PushNull();
    return 0;
}

int method_camSetNear(NPpu::CEngine* pEngine) {
    f32 near = pEngine->PopFloat();

    CStageCamera* pCamera = PopStageCamera(static_cast<CPpuEngine*>(pEngine));
    if (pCamera != nullptr) {
        nw4r::g3d::Camera camera = pCamera->GetG3dCamera();
        nw4r::g3d::CameraData& rData = camera.ref();
        camera.SetPerspective(rData.projFovy, rData.projAspect, near, rData.projFar);
    }

    pEngine->PushNull();
    return 0;
}

int method_camSetFar(NPpu::CEngine* pEngine) {
    f32 far = pEngine->PopFloat();

    CStageCamera* pCamera = PopStageCamera(static_cast<CPpuEngine*>(pEngine));
    if (pCamera != nullptr) {
        nw4r::g3d::Camera camera = pCamera->GetG3dCamera();
        nw4r::g3d::CameraData& rData = camera.ref();
        camera.SetPerspective(rData.projFovy, rData.projAspect, rData.projNear, far);
    }

    pEngine->PushNull();
    return 0;
}

int method_camGetPostureType(NPpu::CEngine* pEngine) {
    CStageCamera* pCamera = PopStageCamera(static_cast<CPpuEngine*>(pEngine));
    if (pCamera != nullptr) {
        nw4r::g3d::Camera camera = pCamera->GetG3dCamera();
        nw4r::g3d::Camera::PostureInfo info;
        camera.GetPosture(&info);
        pEngine->PushInt(info.tp);
    } else {
        pEngine->PushInt(0);
    }

    return 0;
}

int method_camGetUpX(NPpu::CEngine* pEngine) {
    CStageCamera* pCamera = PopStageCamera(static_cast<CPpuEngine*>(pEngine));
    if (pCamera != nullptr) {
        nw4r::g3d::Camera camera = pCamera->GetG3dCamera();
        nw4r::g3d::Camera::PostureInfo info;
        camera.GetPosture(&info);
        pEngine->PushFloat(info.cameraUp.x);
    } else {
        pEngine->PushFloat(0.0f);
    }

    return 0;
}

int method_camGetUpY(NPpu::CEngine* pEngine) {
    CStageCamera* pCamera = PopStageCamera(static_cast<CPpuEngine*>(pEngine));
    if (pCamera != nullptr) {
        nw4r::g3d::Camera camera = pCamera->GetG3dCamera();
        nw4r::g3d::Camera::PostureInfo info;
        camera.GetPosture(&info);
        pEngine->PushFloat(info.cameraUp.y);
    } else {
        pEngine->PushFloat(1.0f);
    }

    return 0;
}

int method_camGetUpZ(NPpu::CEngine* pEngine) {
    CStageCamera* pCamera = PopStageCamera(static_cast<CPpuEngine*>(pEngine));
    if (pCamera != nullptr) {
        nw4r::g3d::Camera camera = pCamera->GetG3dCamera();
        nw4r::g3d::Camera::PostureInfo info;
        camera.GetPosture(&info);
        pEngine->PushFloat(info.cameraUp.z);
    } else {
        pEngine->PushFloat(0.0f);
    }

    return 0;
}

int method_camGetTargetX(NPpu::CEngine* pEngine) {
    CStageCamera* pCamera = PopStageCamera(static_cast<CPpuEngine*>(pEngine));
    if (pCamera != nullptr) {
        nw4r::g3d::Camera camera = pCamera->GetG3dCamera();
        nw4r::g3d::Camera::PostureInfo info;
        camera.GetPosture(&info);
        pEngine->PushFloat(info.cameraTarget.x);
    } else {
        pEngine->PushFloat(0.0f);
    }

    return 0;
}

int method_camGetTargetY(NPpu::CEngine* pEngine) {
    CStageCamera* pCamera = PopStageCamera(static_cast<CPpuEngine*>(pEngine));
    if (pCamera != nullptr) {
        nw4r::g3d::Camera camera = pCamera->GetG3dCamera();
        nw4r::g3d::Camera::PostureInfo info;
        camera.GetPosture(&info);
        pEngine->PushFloat(info.cameraTarget.y);
    } else {
        pEngine->PushFloat(0.0f);
    }

    return 0;
}

int method_camGetTargetZ(NPpu::CEngine* pEngine) {
    CStageCamera* pCamera = PopStageCamera(static_cast<CPpuEngine*>(pEngine));
    if (pCamera != nullptr) {
        nw4r::g3d::Camera camera = pCamera->GetG3dCamera();
        nw4r::g3d::Camera::PostureInfo info;
        camera.GetPosture(&info);
        pEngine->PushFloat(info.cameraTarget.z);
    } else {
        pEngine->PushFloat(0.0f);
    }

    return 0;
}

int method_camGetRotateX(NPpu::CEngine* pEngine) {
    CStageCamera* pCamera = PopStageCamera(static_cast<CPpuEngine*>(pEngine));
    if (pCamera != nullptr) {
        nw4r::g3d::Camera camera = pCamera->GetG3dCamera();
        nw4r::g3d::Camera::PostureInfo info;
        camera.GetPosture(&info);
        pEngine->PushFloat(info.cameraRotate.x);
    } else {
        pEngine->PushFloat(0.0f);
    }

    return 0;
}

int method_camGetRotateY(NPpu::CEngine* pEngine) {
    CStageCamera* pCamera = PopStageCamera(static_cast<CPpuEngine*>(pEngine));
    if (pCamera != nullptr) {
        nw4r::g3d::Camera camera = pCamera->GetG3dCamera();
        nw4r::g3d::Camera::PostureInfo info;
        camera.GetPosture(&info);
        pEngine->PushFloat(info.cameraRotate.y);
    } else {
        pEngine->PushFloat(0.0f);
    }

    return 0;
}

int method_camGetRotateZ(NPpu::CEngine* pEngine) {
    CStageCamera* pCamera = PopStageCamera(static_cast<CPpuEngine*>(pEngine));
    if (pCamera != nullptr) {
        nw4r::g3d::Camera camera = pCamera->GetG3dCamera();
        nw4r::g3d::Camera::PostureInfo info;
        camera.GetPosture(&info);
        pEngine->PushFloat(info.cameraRotate.z);
    } else {
        pEngine->PushFloat(0.0f);
    }

    return 0;
}

int method_camGetTwist(NPpu::CEngine* pEngine) {
    CStageCamera* pCamera = PopStageCamera(static_cast<CPpuEngine*>(pEngine));
    if (pCamera != nullptr) {
        nw4r::g3d::Camera camera = pCamera->GetG3dCamera();
        nw4r::g3d::Camera::PostureInfo info;
        camera.GetPosture(&info);
        pEngine->PushFloat(info.cameraTwist);
    } else {
        pEngine->PushFloat(0.0f);
    }

    return 0;
}

int method_camGetFovy(NPpu::CEngine* pEngine) {
    CStageCamera* pCamera = PopStageCamera(static_cast<CPpuEngine*>(pEngine));
    if (pCamera != nullptr) {
        nw4r::g3d::Camera camera = pCamera->GetG3dCamera();
        pEngine->PushFloat(camera.ref().projFovy);
    } else {
        pEngine->PushFloat(20.256166f);
    }

    return 0;
}

int method_camGetNear(NPpu::CEngine* pEngine) {
    CStageCamera* pCamera = PopStageCamera(static_cast<CPpuEngine*>(pEngine));
    if (pCamera != nullptr) {
        nw4r::g3d::Camera camera = pCamera->GetG3dCamera();
        pEngine->PushFloat(camera.ref().projNear);
    } else {
        pEngine->PushFloat(0.01f);
    }

    return 0;
}

int method_camGetFar(NPpu::CEngine* pEngine) {
    CStageCamera* pCamera = PopStageCamera(static_cast<CPpuEngine*>(pEngine));
    if (pCamera != nullptr) {
        nw4r::g3d::Camera camera = pCamera->GetG3dCamera();
        pEngine->PushFloat(camera.ref().projFovy);
    } else {
        pEngine->PushFloat(1000.0f);
    }

    return 0;
}

static bool GetSafeFrameGroundPos(NPpu::CValue* pResult, BWorldCamera* pCamera, u32 aspect, int x, int y) {
    const GXRenderModeObj* pMode = nw4r::g3d::G3DState::GetRenderModeObj();
    u16 width;
    u16 height;
    GetSafeDisplaySize(&width, &height, pMode->viTVmode, aspect);
    f32 pixelAspect = GetPixelAspectRatio(pMode->viTVmode, aspect);

    nw4r::g3d::CameraData& rData = pCamera->GetG3dCamera().ref();
    f32 bottom;
    f32 top;
    switch (rData.flags & (nw4r::g3d::CameraData::FLAG_PROJ_FRUSTUM | nw4r::g3d::CameraData::FLAG_PROJ_PERSP |
                           nw4r::g3d::CameraData::FLAG_PROJ_ORTHO)) {
    case nw4r::g3d::CameraData::FLAG_PROJ_FRUSTUM:
        top = rData.projTop;
        bottom = rData.projBottom;
        break;
    case nw4r::g3d::CameraData::FLAG_PROJ_PERSP:
        top = nw4r::math::TanDeg(0.5f * rData.projFovy);
        top *= rData.projNear;
        bottom = -top;
        break;
    }

    f32 scale = (top - bottom) / pMode->viHeight / 2.0f;
    f32 halfHeight = height * scale;
    f32 near = rData.projNear;
    f32 halfWidth = scale * (width * pixelAspect);

    nw4r::math::MTX34 viewMtx;
    pCamera->GetG3dCamera().GetCameraMtx(&viewMtx);

    nw4r::math::MTX34 invViewMtx;
    if (!Inverse(&invViewMtx, &viewMtx)) {
        return false;
    }

    nw4r::math::VEC3 nearViewPos(x * halfWidth, y * halfHeight, -near);
    nw4r::math::VEC3 nearPos = Transform(invViewMtx, nearViewPos);
    nw4r::math::VEC3 eye = Transform(invViewMtx, nw4r::math::VEC3(0.0f, 0.0f, 0.0f));
    nw4r::math::RAY3 ray(eye, nearPos - eye, false);
    nw4r::math::PLANE ground(0.0f, 1.0f, 0.0f, 0.0f, true);

    nw4r::math::VEC3 pos;
    if (nw4r::math::IntersectionRay3Plane(&ray, &ground, nullptr, &pos) != nw4r::math::INTERSECTION_1) {
        return false;
    }

    pResult[0].SetFloat(pos.x);
    pResult[1].SetFloat(pos.y);
    pResult[2].SetFloat(pos.z);
    return true;
}

int method_camGetXYZOf(NPpu::CEngine* pEngine) {
    NPpu::CValue* pResult = pEngine->PopReference();
    int corner = pEngine->PopInt();
    BWorldCamera* pCamera = DynamicCast<BWorldCamera>(static_cast<CPpuEngine*>(pEngine)->PopWorldObj());

    bool result;
    switch (corner) {
    case 0:
        result = GetSafeFrameGroundPos(pResult, pCamera, 0, 1, 1);
        break;
    case 1:
        result = GetSafeFrameGroundPos(pResult, pCamera, 0, -1, 1);
        break;
    case 2:
        result = GetSafeFrameGroundPos(pResult, pCamera, 0, -1, -1);
        break;
    case 3:
        result = GetSafeFrameGroundPos(pResult, pCamera, 0, 1, -1);
        break;
    case 4:
        result = GetSafeFrameGroundPos(pResult, pCamera, 1, 1, 1);
        break;
    case 5:
        result = GetSafeFrameGroundPos(pResult, pCamera, 1, -1, 1);
        break;
    case 6:
        result = GetSafeFrameGroundPos(pResult, pCamera, 1, -1, -1);
        break;
    case 7:
        result = GetSafeFrameGroundPos(pResult, pCamera, 1, 1, -1);
        break;
    default:
        result = false;
        break;
    }

    pEngine->PushInt(result);
    return 0;
}

int IsTV16x9(NPpu::CEngine* pEngine) {
    pEngine->PushInt(SCGetAspectRatio() == SC_ASPECT_RATIO_16x9);
    return 0;
}

struct SSafeFrame {
    f32 top;
    f32 bottom;
    f32 left;
    f32 right;
    f32 near;
    f32 far;
};

struct SSafeFrameCache {
    u8 aspect;
    u32 projFlags;
    f32 projTop;
    f32 projBottom;
    f32 projFovy;
    f32 projNear;
    nw4r::math::PLANE planes[4];
};

static SSafeFrameCache sSafeFrameCache = {0xFF};

static SSafeFrame GetSafeFrame(BWorldCamera* pCamera, u32 aspect) {
    const GXRenderModeObj* pMode = nw4r::g3d::G3DState::GetRenderModeObj();
    u16 width;
    u16 height;
    GetSafeDisplaySize(&width, &height, pMode->viTVmode, aspect);
    f32 pixelAspect = GetPixelAspectRatio(pMode->viTVmode, aspect);

    nw4r::g3d::CameraData& rData = pCamera->GetG3dCamera().ref();
    f32 bottom;
    f32 top;
    switch (rData.flags & (nw4r::g3d::CameraData::FLAG_PROJ_FRUSTUM | nw4r::g3d::CameraData::FLAG_PROJ_PERSP |
                           nw4r::g3d::CameraData::FLAG_PROJ_ORTHO)) {
    case nw4r::g3d::CameraData::FLAG_PROJ_FRUSTUM:
        top = rData.projTop;
        bottom = rData.projBottom;
        break;
    case nw4r::g3d::CameraData::FLAG_PROJ_PERSP:
        top = nw4r::math::TanDeg(0.5f * rData.projFovy);
        top *= rData.projNear;
        bottom = -top;
        break;
    }

    SSafeFrame frame;
    frame.near = rData.projNear;
    f32 scale = (top - bottom) / pMode->viHeight / 2.0f;
    frame.far = rData.projFar;
    f32 halfHeight = height * scale;
    f32 halfWidth = scale * (width * pixelAspect);
    frame.top = halfHeight;
    frame.bottom = -halfHeight;
    frame.left = -halfWidth;
    frame.right = halfWidth;
    return frame;
}

static bool MakePlane(nw4r::math::PLANE* pPlane, const nw4r::math::VEC3& rP0, const nw4r::math::VEC3& rP1,
                      const nw4r::math::VEC3& rP2) {
    nw4r::math::VEC3 normal = Cross(rP2 - rP0, rP1 - rP0);
    if (normal.x == 0.0f && normal.y == 0.0f && normal.z == 0.0f) {
        return false;
    }

    if (!SafeNormalize(&normal, &normal)) {
        return false;
    }

    f32 d = -normal.Dot(rP0);
    pPlane->n = normal;
    pPlane->d = d;
    return true;
}

static bool IsInFrontOfPlane(const nw4r::math::VEC3& rPos, const nw4r::math::PLANE& rPlane, f32 margin) {
    f32 offset = -rPlane.d;
    return -margin <= rPos.Dot(rPlane.n) - offset;
}

static int IsObjInSafeFrame(NPpu::CEngine* pEngine, u32 aspect) {
    f32 margin = pEngine->PopFloat();
    BWorldObj* pObj = static_cast<CPpuEngine*>(pEngine)->PopWorldObj();
    BWorldCamera* pCamera = DynamicCast<BWorldCamera>(static_cast<CPpuEngine*>(pEngine)->PopWorldObj());

    bool result = false;
    if (pObj != nullptr && pCamera != nullptr) {
        nw4r::g3d::CameraData& rData = pCamera->GetG3dCamera().ref();
        if (aspect == sSafeFrameCache.aspect && rData.flags == sSafeFrameCache.projFlags &&
            rData.projTop == sSafeFrameCache.projTop && rData.projBottom == sSafeFrameCache.projBottom &&
            rData.projFovy == sSafeFrameCache.projFovy && rData.projNear == sSafeFrameCache.projNear) {
            result = true;
        } else {
            SSafeFrame frame = GetSafeFrame(pCamera, aspect);
            nw4r::math::VEC3 origin(0.0f, 0.0f, 0.0f);
            nw4r::math::VEC3 topLeft(frame.left, frame.top, frame.near);
            nw4r::math::VEC3 bottomLeft(frame.left, frame.bottom, frame.near);
            nw4r::math::VEC3 topRight(frame.right, frame.top, frame.near);
            nw4r::math::VEC3 bottomRight(frame.right, frame.bottom, frame.near);
            nw4r::math::PLANE left;
            nw4r::math::PLANE top;
            nw4r::math::PLANE right;
            nw4r::math::PLANE bottom;
            if (MakePlane(&left, origin, topLeft, bottomLeft) && MakePlane(&top, origin, topRight, topLeft) &&
                MakePlane(&right, origin, bottomRight, topRight) &&
                MakePlane(&bottom, origin, bottomLeft, bottomRight)) {
                sSafeFrameCache.aspect = aspect;
                sSafeFrameCache.projFlags = rData.flags;
                sSafeFrameCache.projTop = rData.projTop;
                sSafeFrameCache.projBottom = rData.projBottom;
                sSafeFrameCache.projFovy = rData.projFovy;
                sSafeFrameCache.projNear = rData.projNear;
                sSafeFrameCache.planes[0] = left;
                sSafeFrameCache.planes[1] = top;
                sSafeFrameCache.planes[2] = right;
                sSafeFrameCache.planes[3] = bottom;
                result = true;
            }
        }

        if (result) {
            nw4r::math::MTX34 viewMtx;
            pCamera->GetG3dCamera().GetCameraMtx(&viewMtx);
            nw4r::math::VEC3 pos = Transform(viewMtx, pObj->GetPositionRef());

            result = false;
            if (IsInFrontOfPlane(pos, sSafeFrameCache.planes[0], margin) &&
                IsInFrontOfPlane(pos, sSafeFrameCache.planes[1], margin) &&
                IsInFrontOfPlane(pos, sSafeFrameCache.planes[2], margin) &&
                IsInFrontOfPlane(pos, sSafeFrameCache.planes[3], margin)) {
                result = true;
            }
        }
    }

    pEngine->PushInt(result);
    return 0;
}

int method_camIsObjInSafeFrame(NPpu::CEngine* pEngine) {
    u8 aspect = SCGetAspectRatio();
    return IsObjInSafeFrame(pEngine, aspect);
}

int method_camIsObjInSafeFrame4x3(NPpu::CEngine* pEngine) {
    return IsObjInSafeFrame(pEngine, SC_ASPECT_RATIO_4x3);
}

int method_camIsObjInSafeFrame16x9(NPpu::CEngine* pEngine) {
    return IsObjInSafeFrame(pEngine, SC_ASPECT_RATIO_16x9);
}

static nw4r::math::VEC3 ProjectToHeight(const nw4r::math::MTX34& rInvViewMtx, const nw4r::math::VEC3& rViewPos,
                                        f32 height) {
    nw4r::math::VEC3 eye = Transform(rInvViewMtx, nw4r::math::VEC3(0.0f, 0.0f, 0.0f));
    nw4r::math::VEC3 pos = Transform(rInvViewMtx, rViewPos);
    nw4r::math::VEC3 dir = pos - eye;
    if (0.0f == dir.y) {
        return nw4r::math::VEC3(pos.x, height, pos.z);
    }

    f32 t = (height - eye.y) / dir.y;
    return nw4r::math::VEC3(eye.x + dir.x * t, height, eye.z + dir.z * t);
}

class CCameraAdjuster {
public:
    enum Result {
        RESULT_INSIDE,
        RESULT_BEHIND,
        RESULT_MOVED,
        RESULT_BLOCKED
    };

    CCameraAdjuster();

    bool Init(BWorldCamera* pCamera, u32 aspect, f32 maxDepth, f32 margin) {
        if (pCamera == nullptr) {
            return false;
        }

        pCamera->GetG3dCamera().GetCameraMtx(&mViewMtx);
        if (!Inverse(&mInvViewMtx, &mViewMtx)) {
            return false;
        }

        SSafeFrame frame = GetSafeFrame(pCamera, aspect);
        f32 scale = 1.0f / -frame.near;
        mTopSlope = scale * frame.top;
        mBottomSlope = scale * frame.bottom;
        mLeftSlope = scale * frame.left;
        mRightSlope = scale * frame.right;
        mTopLimit = margin * mTopSlope;
        mBottomLimit = margin * mBottomSlope;
        mLeftLimit = margin * mLeftSlope;
        mRightLimit = margin * mRightSlope;
        mMaxDepth = maxDepth;
        mDepth = 0.0f;
        mCount = 0;
        mSum = nw4r::math::VEC3(0.0f, 0.0f, 0.0f);
        mClipped = false;
        mGround = DynamicCast<CDividedGround>(pCamera->GetWorld()->GetGround());
        mPosition = nw4r::math::VEC3(0.0f, 0.0f, 0.0f);
        return Adjust();
    }

    const nw4r::math::VEC3& GetPosition() const { return mPosition; }
    f32 GetDepth() const { return mDepth; }
    bool IsClipped() const { return mClipped; }

private:
    bool Adjust() {
        CPlayerPii* pFirst = nullptr;
        nw4r::math::VEC3 firstOffset;
        std::vector<CPlayerPii*> blocked;
        blocked.reserve(4);

        for (int i = 0; i < 4; i++) {
            CPlayer* pPlayer = IGameSession::GetInstance()->GetPlayer(i);
            if (!pPlayer->IsJoined()) {
                continue;
            }

            CPlayerPii* pPii = pPlayer->GetPii();
            if (pPii == nullptr) {
                continue;
            }

            nw4r::math::VEC3 viewPos;
            nw4r::math::VEC3 worldPos;
            nw4r::math::VEC3 pos;
            pos = pPii->GetPositionRef();
            for (u32 step = 0;;) {
                switch (AdjustPii(&viewPos, &worldPos, pos)) {
                case RESULT_BEHIND:
                    goto next;
                case RESULT_MOVED:
                    pPii->SetPosition(worldPos);
                    break;
                case RESULT_BLOCKED: {
                    std::deque<nw4r::math::VEC3>& rHistory = pPii->GetPositionHistory();
                    if (rHistory.size() <= step) {
                        blocked.push_back(pPii);
                        goto next;
                    }
                    pos = rHistory[rHistory.size() - step - 1];
                    step++;
                    continue;
                }
                }

                mSum += viewPos;
                mCount++;
                if (pFirst == nullptr) {
                    pFirst = pPii;
                    firstOffset = viewPos;
                }
                break;
            }
        next:;
        }

        if (blocked.size()) {
            if (mCount == 0) {
                pFirst = blocked.back();
                blocked.pop_back();
                while (blocked.size()) {
                    CPlayerPii* pPii = blocked.back();
                    blocked.pop_back();
                    pPii->SetPosition(nw4r::math::VEC3(pFirst->GetPositionRef().x, pPii->GetPositionRef().y,
                                                       pFirst->GetPositionRef().z));
                }

                mPosition = pFirst->GetPositionRef();
                mPosition.y = 0.0f;
                return true;
            }

            while (blocked.size()) {
                CPlayerPii* pPii = blocked.back();
                blocked.pop_back();
                pPii->SetPosition(nw4r::math::VEC3(pFirst->GetPositionRef().x, pPii->GetPositionRef().y,
                                                   pFirst->GetPositionRef().z));
                mSum += firstOffset;
                mCount++;
            }
        }

        if (mCount <= 0) {
            return false;
        }

        mPosition = ProjectToHeight(mInvViewMtx, mSum / mCount, 0.0f);
        return true;
    }

    Result AdjustPii(nw4r::math::VEC3* pViewPos, nw4r::math::VEC3* pWorldPos,
                                                       const nw4r::math::VEC3& rPos) {
        *pViewPos = Transform(mViewMtx, rPos);
        if (0.0f <= pViewPos->z) {
            return RESULT_BEHIND;
        }

        *pViewPos = *pViewPos * (-1.0f / pViewPos->z);
        if (mDepth <= mMaxDepth) {
            f32 top = pViewPos->z - pViewPos->y / mTopLimit;
            f32 bottom = pViewPos->z - pViewPos->y / mBottomLimit;
            f32 left = pViewPos->z - pViewPos->x / mLeftLimit;
            f32 right = pViewPos->z - pViewPos->x / mRightLimit;
            f32 depth = std::max(std::max(top, bottom), std::max(left, right));
            if (0.0f < depth) {
                mClipped = true;
            }
            mDepth = std::min(std::max(mDepth, depth), mMaxDepth);
        }

        f32 distance = pViewPos->z - mDepth;
        bool moved = false;
        f32 limit = distance * mTopSlope;
        if (limit < pViewPos->y) {
            pViewPos->y = limit;
            moved = true;
        } else {
            limit = distance * mBottomSlope;
            if (pViewPos->y < limit) {
                pViewPos->y = limit;
                moved = true;
            }
        }

        limit = distance * mRightSlope;
        if (limit < pViewPos->x) {
            pViewPos->x = limit;
            moved = true;
        } else {
            limit = distance * mLeftSlope;
            if (pViewPos->x < limit) {
                pViewPos->x = limit;
                moved = true;
            }
        }

        if (!moved) {
            return RESULT_INSIDE;
        }

        *pWorldPos = ProjectToHeight(mInvViewMtx, *pViewPos, rPos.y);
        if (mGround != nullptr && !mGround->IsOutOfBounds(*pWorldPos)) {
            return RESULT_MOVED;
        }

        return RESULT_BLOCKED;
    }

    nw4r::math::MTX34 mViewMtx;
    nw4r::math::MTX34 mInvViewMtx;
    f32 mTopSlope;
    f32 mBottomSlope;
    f32 mLeftSlope;
    f32 mRightSlope;
    f32 mTopLimit;
    f32 mBottomLimit;
    f32 mLeftLimit;
    f32 mRightLimit;
    f32 mMaxDepth;
    f32 mDepth;
    int mCount;
    nw4r::math::VEC3 mSum;
    bool mClipped;
    nw4r::math::VEC3 mPosition;
    CDividedGround* mGround;
};

int method_camAdjustPos(NPpu::CEngine* pEngine) {
    f32 margin = pEngine->PopFloat();
    f32 maxDepth = pEngine->PopFloat();
    NPpu::CValue* pClipped = pEngine->PopReference();
    NPpu::CValue* pDepth = pEngine->PopReference();
    NPpu::CValue* pPosition = pEngine->PopReference();
    BWorldCamera* pCamera = DynamicCast<BWorldCamera>(static_cast<CPpuEngine*>(pEngine)->PopWorldObj());

    CCameraAdjuster adjuster;
    u8 aspect = SCGetAspectRatio();
    if (adjuster.Init(pCamera, aspect, maxDepth, margin)) {
        pPosition[0].SetFloat(adjuster.GetPosition().x);
        pPosition[1].SetFloat(adjuster.GetPosition().y);
        pPosition[2].SetFloat(adjuster.GetPosition().z);
        pDepth->SetFloat(adjuster.GetDepth());
        pClipped->SetInt(adjuster.IsClipped());
        pEngine->PushInt(1);
    } else {
        pEngine->PushInt(0);
    }

    return 0;
}

inline CCameraAdjuster::CCameraAdjuster() {}
