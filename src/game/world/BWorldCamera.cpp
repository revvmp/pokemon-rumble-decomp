#include <game/world/BWorldCamera.h>

#include <game/world/BWorld.h>
#include <game/io/CFileData.h>
#include <game/graphics/RenderMode.h>
#include <nw4r/g3d/res/g3d_resfile.h>

BWorldCamera::BWorldCamera(u32 type) : BWorldObj(type | TYPE), mIndex(-1), mNextMotionID(1) {}

BWorldCamera::~BWorldCamera() {}

nw4r::g3d::Camera BWorldCamera::GetG3dCamera() {
    int index = mIndex;
    BWorld* pWorld = mWorld;
    if (index >= 0 && pWorld != nullptr) {
        return pWorld->GetScnRoot()->GetCamera(index);
    }
    return nw4r::g3d::Camera(nullptr);
}

inline bool BWorldCamera::Load(const std::tr1::shared_ptr<CFileData>& rFile) {
    if (!rFile || !rFile->IsValid()) {
        return false;
    }
    if (rFile.get() == mAnmFile.get()) {
        return true;
    }
    nw4r::g3d::ResFile resource(rFile->GetData());
    if (!resource.Bind() || !resource.HasResAnmScn()) {
        return false;
    }
    Stop();
    mAnmFile = rFile;
    return true;
}

bool BWorldCamera::Load(const char* pPath, bool background) {
    if (pPath == nullptr) {
        return false;
    }
    std::tr1::shared_ptr<CFileData> file = LoadResourceFile(pPath, 0, background);
    return Load(file);
}

std::tr1::shared_ptr<CFileData> LoadResourceFile(const char* pPath, u32 flags, int background) {
    return CreateFileData(pPath, flags | 0x2000, background);
}

BWorldCamera::MotionPtr BWorldCamera::GetCurrentMotion() {
    if (!mMotionPrograms.empty()) {
        return mMotionPrograms.front();
    }
    return MotionPtr();
}

BWorldCamera::MotionPtr BWorldCamera::FindMotion(u32 id) {
    if (!mMotionPrograms.empty()) {
        for (MotionList::iterator it = mMotionPrograms.begin(); it != mMotionPrograms.end(); ++it) {
            if ((*it)->mId == id) {
                return *it;
            }
        }
    }
    return MotionPtr();
}

BWorldCamera::MotionPtr BWorldCamera::Play(MotionPtr& rMotion) {
    if (!rMotion) {
        return MotionPtr();
    }
    for (MotionList::iterator it = mMotionPrograms.begin(); it != mMotionPrograms.end(); ++it) {
        (*it)->SetState(CMotionProgram::FLAG_STOPPED);
    }
    mMotionPrograms.clear();
    mMotionPrograms.push_back(rMotion);
    rMotion->SetState(CMotionProgram::FLAG_PLAYING);
    f32 frame = mMotionFrame;
    if (rMotion->mStartFrame < 0.0f) {
        mMotionFrame = 0.0f;
        UpdateFrame(frame);
        return !mMotionPrograms.empty() ? rMotion : MotionPtr();
    }
    mMotionFrame = rMotion->mStartFrame;
    return rMotion;
}

BWorldCamera::MotionPtr BWorldCamera::Play(const char* pName, f32 frame, f32 rate) {
    if (pName == nullptr) {
        return MotionPtr();
    }
    MotionPtr motion = CreateMotion(pName, frame, rate);
    return Play(motion);
}

BWorldCamera::MotionPtr BWorldCamera::Request(MotionPtr& rMotion) {
    if (!rMotion) {
        return MotionPtr();
    }
    if (mMotionPrograms.empty()) {
        return Play(rMotion);
    }
    if (mMotionPrograms.size() >= 2) {
        MotionList current;
        current.push_back(mMotionPrograms.front());
        mMotionPrograms.pop_front();
        for (MotionList::iterator it = mMotionPrograms.begin(); it != mMotionPrograms.end(); ++it) {
            (*it)->SetState(CMotionProgram::FLAG_STOPPED);
        }
        mMotionPrograms.clear();
        mMotionPrograms.swap(current);
    }
    mMotionPrograms.push_back(rMotion);
    return rMotion;
}

BWorldCamera::MotionPtr BWorldCamera::Request(const char* pName, f32 frame, f32 rate) {
    if (pName == nullptr) {
        return MotionPtr();
    }
    MotionPtr motion = CreateMotion(pName, frame, rate);
    return Request(motion);
}

BWorldCamera::MotionPtr BWorldCamera::RequestNext(MotionPtr& rMotion) {
    if (!rMotion) {
        return MotionPtr();
    }
    if (mMotionPrograms.empty()) {
        return Play(rMotion);
    }
    mMotionPrograms.push_back(rMotion);
    return rMotion;
}

BWorldCamera::MotionPtr BWorldCamera::RequestNext(const char* pName, f32 frame, f32 rate) {
    if (pName == nullptr) {
        return MotionPtr();
    }
    MotionPtr motion = CreateMotion(pName, frame, rate);
    return RequestNext(motion);
}

void BWorldCamera::Stop() {
    for (MotionList::iterator it = mMotionPrograms.begin(); it != mMotionPrograms.end(); ++it) {
        it->get()->SetState(CMotionProgram::FLAG_STOPPED);
    }
    mMotionPrograms.clear();
}

void BWorldCamera::Pause(bool pause) {
    if (!mMotionPrograms.empty()) {
        CMotionProgram* pMotion = mMotionPrograms.front().get();
        if (pause) {
            pMotion->mFlags |= CMotionProgram::FLAG_PAUSED;
        } else {
            pMotion->mFlags &= ~CMotionProgram::FLAG_PAUSED;
        }
    }
}

int BWorldCamera::GetMotionLength(const char* pName) {
    if (!mAnmFile || !mAnmFile->IsValid()) {
        return 0;
    }
    nw4r::g3d::ResAnmScn animation = nw4r::g3d::ResFile(mAnmFile->GetData()).GetResAnmScn(pName);
    if (!animation.IsValid() || !animation.HasResAnmCamera()) {
        return 0;
    }
    return animation.ref().info.numFrame;
}

void BWorldCamera::UpdateFrame(f32 step) {
    if (mMotionPrograms.empty()) {
        mMotionFrame += step;
        return;
    }
    for (;;) {
        MotionPtr& motion = mMotionPrograms.front();
        if (motion->mFlags & CMotionProgram::FLAG_PAUSED) {
            break;
        }
        f32 length = motion->mLength;
        mMotionFrame += step * motion->mRate;
        if (mMotionFrame < length) {
            break;
        }
        f32 frame = mMotionFrame;
        mMotionFrame -= length;
        if (mMotionPrograms.size() >= 2) {
            step = mMotionFrame / motion->mRate;
            motion->SetState(CMotionProgram::FLAG_STOPPED);
            mMotionPrograms.pop_front();
            f32 firstFrame = 0.0f;
            f32 startFrame = mMotionPrograms.front()->mStartFrame;
            mMotionFrame = startFrame < firstFrame ? firstFrame : startFrame;
            mMotionPrograms.front()->SetState(CMotionProgram::FLAG_PLAYING);
            continue;
        }
        if (motion->mResource.ref().info.policy == nw4r::g3d::ANM_POLICY_LOOP) {
            motion->SetState(CMotionProgram::FLAG_LOOPED);
            step = 0.0f;
            motion->mFlags += CMotionProgram::LOOP_COUNT_INCREMENT;
            continue;
        }
        step = mMotionFrame;
        mMotionFrame = frame;
        ApplyFrame();
        mMotionFrame = step;
        mMotionFrame /= motion->mRate;
        motion->SetState(CMotionProgram::FLAG_STOPPED);
        mMotionPrograms.pop_front();
        break;
    }
}

BWorldCamera::MotionPtr BWorldCamera::CreateMotion(MotionPtr& rMotion, f32 frame, f32 rate) {
    if (!rMotion->mResource.IsValid() || !rMotion->mResource.HasResAnmCamera()) {
        return MotionPtr();
    }
    nw4r::g3d::ResAnmCamera camera = rMotion->mResource.GetResAnmCamera(0);
    if (!camera.IsValid()) {
        return MotionPtr();
    }
    rMotion->mId = mNextMotionID;
    rMotion->mStartFrame = frame;
    rMotion->mRate = rate;
    rMotion->mUserData = 0;
    rMotion->mpCurrentFrame = &mMotionFrame;
    rMotion->mLength = rMotion->mResource.ref().info.numFrame;
    rMotion->SetState(CMotionProgram::FLAG_QUEUED);
    rMotion->mCamera = camera;
    if (++mNextMotionID == 0) {
        ++mNextMotionID;
    }
    return rMotion;
}

BWorldCamera::MotionPtr BWorldCamera::CreateMotion(const char* pName, f32 frame, f32 rate) {
    if (pName == nullptr) {
        return MotionPtr();
    }
    nw4r::g3d::ResFile resource(mAnmFile->GetData());
    MotionPtr motion(new CMotionProgram);
    motion->mFlags = 0;
    motion->mFlags |= CMotionProgram::FLAG_CREATED;
    motion->mResource = resource.GetResAnmScn(pName);
    motion->mName = pName;
    return CreateMotion(motion, frame, rate);
}

void BWorldCamera::ApplyFrame() {
    nw4r::g3d::Camera camera = GetG3dCamera();
    MotionPtr& rMotion = mMotionPrograms.front();
    nw4r::g3d::CameraAnmResult result;
    rMotion->mCamera.GetAnmResult(&result, mMotionFrame);
    if (!(result.flags & nw4r::g3d::CameraAnmResult::FLAG_ANM_EXISTS)) {
        return;
    }
    f32 aspect = camera.ref().projAspect;
    switch (result.projType) {
    case GX_PERSPECTIVE:
        camera.SetPerspective(result.perspFovy, aspect, result.near, result.far);
        break;
    case GX_ORTHOGRAPHIC: {
        f32 height = result.orthoHeight;
        f32 top = height;
        top *= 0.5f;
        f32 right = top * aspect;
        camera.SetOrtho(top, -top, -right, right, result.near, result.far);
        break;
    }
    }

    nw4r::g3d::Camera::PostureInfo posture;
    switch (result.flags & nw4r::g3d::CameraAnmResult::FLAG_CAMERA_TYPE_MASK) {
    case nw4r::g3d::ResAnmCameraDataTypedef::CAMERATYPE_ROTATE:
        posture.tp = nw4r::g3d::Camera::POSTURE_ROTATE;
        posture.cameraRotate = result.rotate.rot;
        break;
    case nw4r::g3d::ResAnmCameraDataTypedef::CAMERATYPE_AIM:
        if (rMotion->mFlags & CMotionProgram::FLAG_IGNORE_AIM) {
            camera.GetPosture(&posture);
            result.pos += posture.cameraTarget;
        } else {
            posture.cameraTarget = result.aim.aim;
        }
        posture.tp = nw4r::g3d::Camera::POSTURE_AIM;
        posture.cameraTwist = result.aim.twist;
        break;
    }
    SetPosition(result.pos);
    camera.SetPosture(posture);
}

void BWorldCamera::SetPosition(const nw4r::math::VEC3& rPosition) {
    mPosition = rPosition;
    nw4r::g3d::Camera camera = GetG3dCamera();
    if (camera.IsValid()) {
        camera.SetPosition(mPosition);
    }
}

void BWorldCamera::Calc() {
    UpdateFrame(1.0f);
    if (!mMotionPrograms.empty()) {
        ApplyFrame();
    }
}

void BWorldCamera::OnEnterWorld() {}

void BWorldCamera::OnLeaveWorld(BWorld* pWorld) {}

void BWorldCamera::UpdateIndex(int prevIndex) {
    if (mIndex < 0) {
        return;
    }
    nw4r::g3d::Camera camera = GetG3dCamera();
    camera.SetPosition(mPosition);
    nw4r::g3d::Camera::PostureInfo posture;
    posture.tp = nw4r::g3d::Camera::POSTURE_ROTATE;
    posture.cameraRotate = nw4r::math::VEC3(-45.0f, 0.0f, 0.0f);
    camera.SetPosture(posture);
    camera.SetPerspective(20.256166f, GetDisplayAspectRatio(), 0.01f, 1000.0f);
}
