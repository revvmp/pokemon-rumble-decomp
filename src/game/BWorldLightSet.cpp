#include <game/BWorldLightSet.h>

#include <game/AnmObjChrRootScale.h>
#include <game/BWorld.h>
#include <game/CFileData.h>
#include <game/CShadowCamera.h>
#include <game/Memory.h>
#include <nw4r/g3d/res/g3d_resfile.h>

BWorldLightSet::BWorldLightSet(u32 type) : BWorldObj(type | TYPE), mNextMotionID(1) {}

BWorldLightSet::~BWorldLightSet() {}

bool BWorldLightSet::Load(const char* pPath, bool background) {
    if (pPath == nullptr) {
        return false;
    }
    std::tr1::shared_ptr<CFileData> file = LoadResourceFile(pPath, 0, background);
    return Load(file);
}

bool BWorldLightSet::Load(const std::tr1::shared_ptr<CFileData>& rFile) {
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

BWorldLightSet::MotionPtr BWorldLightSet::GetCurrentMotion() {
    if (!mMotionPrograms.empty()) {
        return mMotionPrograms.front();
    }
    return MotionPtr();
}

BWorldLightSet::MotionPtr BWorldLightSet::FindMotion(u32 id) {
    if (!mMotionPrograms.empty()) {
        for (MotionList::iterator it = mMotionPrograms.begin(); it != mMotionPrograms.end(); ++it) {
            if ((*it)->mId == id) {
                return *it;
            }
        }
    }
    return MotionPtr();
}

BWorldLightSet::MotionPtr BWorldLightSet::Play(MotionPtr& rMotion) {
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

BWorldLightSet::MotionPtr BWorldLightSet::Play(const char* pName, f32 frame, f32 rate) {
    if (pName == nullptr) {
        return MotionPtr();
    }
    MotionPtr motion = CreateMotion(pName, frame, rate);
    return Play(motion);
}

BWorldLightSet::MotionPtr BWorldLightSet::Request(MotionPtr& rMotion) {
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

BWorldLightSet::MotionPtr BWorldLightSet::Request(const char* pName, f32 frame, f32 rate) {
    if (pName == nullptr) {
        return MotionPtr();
    }
    MotionPtr motion = CreateMotion(pName, frame, rate);
    return Request(motion);
}

BWorldLightSet::MotionPtr BWorldLightSet::RequestNext(MotionPtr& rMotion) {
    if (!rMotion) {
        return MotionPtr();
    }
    if (mMotionPrograms.empty()) {
        return Play(rMotion);
    }
    mMotionPrograms.push_back(rMotion);
    return rMotion;
}

BWorldLightSet::MotionPtr BWorldLightSet::RequestNext(const char* pName, f32 frame, f32 rate) {
    if (pName == nullptr) {
        return MotionPtr();
    }
    MotionPtr motion = CreateMotion(pName, frame, rate);
    return RequestNext(motion);
}

void BWorldLightSet::Stop() {
    for (MotionList::iterator it = mMotionPrograms.begin(); it != mMotionPrograms.end(); ++it) {
        it->get()->SetState(CMotionProgram::FLAG_STOPPED);
    }
    mMotionPrograms.clear();
}

void BWorldLightSet::Pause(bool pause) {
    if (!mMotionPrograms.empty()) {
        CMotionProgram* pMotion = mMotionPrograms.front().get();
        if (pause) {
            pMotion->mFlags |= CMotionProgram::FLAG_PAUSED;
        } else {
            pMotion->mFlags &= ~CMotionProgram::FLAG_PAUSED;
        }
    }
}

int BWorldLightSet::GetMotionLength(const char* pName) {
    if (!mAnmFile || !mAnmFile->IsValid()) {
        return 0;
    }
    nw4r::g3d::ResAnmScn animation = nw4r::g3d::ResFile(mAnmFile->GetData()).GetResAnmScn(pName);
    if (!animation.IsValid() || !animation.HasResLightSet()) {
        return 0;
    }
    return animation.ref().info.numFrame;
}

void BWorldLightSet::UpdateFrame(f32 step) {
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
        ApplyFrame(motion);
        mMotionFrame = step;
        mMotionFrame /= motion->mRate;
        motion->SetState(CMotionProgram::FLAG_STOPPED);
        mMotionPrograms.pop_front();
        break;
    }
}

BWorldLightSet::MotionPtr BWorldLightSet::CreateMotion(MotionPtr& rMotion, f32 frame, f32 rate) {
    if (!rMotion->mResource.IsValid() || !rMotion->mResource.Bind() || !rMotion->mResource.HasResLightSet()) {
        return MotionPtr();
    }
    rMotion->mId = mNextMotionID;
    rMotion->mStartFrame = frame;
    rMotion->mRate = rate;
    rMotion->mReservedState = 0;
    rMotion->mpCurrentFrame = &mMotionFrame;
    rMotion->mLength = rMotion->mResource.ref().info.numFrame;
    rMotion->SetState(CMotionProgram::FLAG_QUEUED);
    rMotion->mAnimation.reset(nw4r::g3d::AnmScnRes::Construct(GetDefaultAllocator(), nullptr, rMotion->mResource, true), DestroyG3dObj);
    if (++mNextMotionID == 0) {
        ++mNextMotionID;
    }
    return rMotion;
}

BWorldLightSet::MotionPtr BWorldLightSet::CreateMotion(const char* pName, f32 frame, f32 rate) {
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

void BWorldLightSet::Calc() {
    UpdateFrame(1.0f);
    if (!mMotionPrograms.empty()) {
        ApplyFrame(mMotionPrograms.front());
    }
}

void BWorldLightSet::ApplyFrame(MotionPtr& rMotion) {
    rMotion->mAnimation->SetFrame(mMotionFrame);
    if (rMotion->mAnimation->GetLightSetMaxRefNumber() != 0) {
        BWorld* pWorld = mWorld;
        rMotion->mAnimation->GetLightSetting(&pWorld->GetScnRoot()->GetLightSetting());
        CShadowCamera* pShadow = pWorld->GetShadowCamera();
        if (pShadow != nullptr && pShadow->IsAutoSelectLight()) {
            pShadow->SelectLight();
        }
    }
}

void BWorldLightSet::OnEnterWorld() {}

void BWorldLightSet::OnLeaveWorld() {}

void BWorldObj::SetPosition(const nw4r::math::VEC3& rPosition) {
    mPosition = rPosition;
}
