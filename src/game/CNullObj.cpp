#include <game/CNullObj.h>

#include <game/BModelObj.h>
#include <game/CFileData.h>
#include <nw4r/g3d/res/g3d_resfile.h>
#include <nw4r/math/math_triangular.h>

CNullObj::CNullObj(u32 type) : BWorldObj(type | TYPE), mNextMotionID(1) {}

CNullObj::~CNullObj() {}

void CNullObj::SetTarget(const std::tr1::shared_ptr<BWorldObj>& rTarget) {
    mTarget = rTarget;
}

bool CNullObj::LoadAnm(const char* pPath, bool background) {
    if (pPath == nullptr) {
        return false;
    }
    std::tr1::shared_ptr<CFileData> file = LoadResourceFile(pPath, 0, background);
    bool loaded;
    if (!file || !file->IsValid()) {
        loaded = false;
    } else if (file.get() == mAnmFile.get()) {
        loaded = true;
    } else {
        nw4r::g3d::ResFile resource(file->GetData());
        if (!resource.Bind() || !resource.HasResAnmChr()) {
            loaded = false;
        } else {
            mAnmFile = file;
            loaded = true;
        }
    }
    return loaded;
}

CNullObj::MotionPtr CNullObj::GetCurrentMotion() {
    if (!mMotionPrograms.empty()) {
        return mMotionPrograms.front();
    }
    return MotionPtr();
}

CNullObj::MotionPtr CNullObj::FindMotion(u32 id) {
    if (!mMotionPrograms.empty()) {
        for (MotionList::iterator it = mMotionPrograms.begin(); it != mMotionPrograms.end(); ++it) {
            if ((*it)->mId == id) {
                return *it;
            }
        }
    }
    return MotionPtr();
}

CNullObj::MotionPtr CNullObj::Play(MotionPtr& rMotion) {
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

CNullObj::MotionPtr CNullObj::Play(const char* pName, f32 frame, f32 rate) {
    if (pName == nullptr) {
        return MotionPtr();
    }
    MotionPtr motion = CreateMotion(pName, frame, rate);
    return Play(motion);
}

CNullObj::MotionPtr CNullObj::Request(MotionPtr& rMotion) {
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

CNullObj::MotionPtr CNullObj::Request(const char* pName, f32 frame, f32 rate) {
    if (pName == nullptr) {
        return MotionPtr();
    }
    MotionPtr motion = CreateMotion(pName, frame, rate);
    return Request(motion);
}

CNullObj::MotionPtr CNullObj::RequestNext(MotionPtr& rMotion) {
    if (!rMotion) {
        return MotionPtr();
    }
    if (mMotionPrograms.empty()) {
        return Play(rMotion);
    }
    mMotionPrograms.push_back(rMotion);
    return rMotion;
}

CNullObj::MotionPtr CNullObj::RequestNext(const char* pName, f32 frame, f32 rate) {
    if (pName == nullptr) {
        return MotionPtr();
    }
    MotionPtr motion = CreateMotion(pName, frame, rate);
    return RequestNext(motion);
}

void CNullObj::Stop() {
    for (MotionList::iterator it = mMotionPrograms.begin(); it != mMotionPrograms.end(); ++it) {
        it->get()->SetState(CMotionProgram::FLAG_STOPPED);
    }
    mMotionPrograms.clear();
}

void CNullObj::Pause(bool pause) {
    if (!mMotionPrograms.empty()) {
        CMotionProgram* pMotion = mMotionPrograms.front().get();
        if (pause) {
            pMotion->mFlags |= CMotionProgram::FLAG_PAUSED;
        } else {
            pMotion->mFlags &= ~CMotionProgram::FLAG_PAUSED;
        }
    }
}

int CNullObj::GetMotionLength(const char* pName) {
    if (!mAnmFile || !mAnmFile->IsValid()) {
        return 0;
    }
    nw4r::g3d::ResAnmChr animation = nw4r::g3d::ResFile(mAnmFile->GetData()).GetResAnmChr(pName);
    if (!animation.IsValid()) {
        return 0;
    }
    return animation.GetNumFrame();
}

void CNullObj::UpdateFrame(f32 step) {
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

CNullObj::MotionPtr CNullObj::CreateMotion(MotionPtr& rMotion, f32 frame, f32 rate) {
    if (!rMotion->mResource.IsValid()) {
        return MotionPtr();
    }
    rMotion->mId = mNextMotionID;
    rMotion->mStartFrame = frame;
    rMotion->mRate = rate;
    rMotion->mUserData = 0;
    rMotion->mpCurrentFrame = &mMotionFrame;
    rMotion->mLength = rMotion->mResource.GetNumFrame();
    rMotion->SetState(CMotionProgram::FLAG_QUEUED);
    if (++mNextMotionID == 0) {
        ++mNextMotionID;
    }
    return rMotion;
}

CNullObj::MotionPtr CNullObj::CreateMotion(const char* pName, f32 frame, f32 rate) {
    if (pName == nullptr) {
        return MotionPtr();
    }
    nw4r::g3d::ResFile resource(mAnmFile->GetData());
    MotionPtr motion(new CMotionProgram);
    motion->mFlags = 0;
    motion->mFlags |= CMotionProgram::FLAG_CREATED;
    motion->mResource = resource.GetResAnmChr(pName);
    motion->mName = pName;
    return CreateMotion(motion, frame, rate);
}

void CNullObj::ApplyFrame() {
    nw4r::g3d::ChrAnmResult result;
    mMotionPrograms.front()->mResource.GetAnmResult(&result, 0, mMotionFrame);
    if (result.flags & nw4r::g3d::ChrAnmResult::FLAG_ANM_EXISTS) {
        nw4r::math::MTX34 animated;
        result.GetMtx(&animated);
        f32 sine;
        f32 cosine;
        nw4r::math::SinCosFIdx(&sine, &cosine, NW4R_MATH_RAD_TO_FIDX(mRotation));
        nw4r::math::MTX34 rotation(cosine, 0.0f, sine, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, -sine, 0.0f, cosine, 0.0f);
        nw4r::math::MTX34 scale(mScale.x, 0.0f, 0.0f, 0.0f, 0.0f, mScale.y, 0.0f, 0.0f, 0.0f, 0.0f, mScale.z, 0.0f);
        nw4r::math::MTX34 translation(1.0f, 0.0f, 0.0f, mPosition.x, 0.0f, 1.0f, 0.0f, mPosition.y, 0.0f, 0.0f, 1.0f, mPosition.z);
        nw4r::math::MTX34 scaled;
        nw4r::math::MTX34 rotated;
        nw4r::math::MTX34 transformed;
        nw4r::math::MTX34Mult(&scaled, &translation, &scale);
        nw4r::math::MTX34Mult(&rotated, &scaled, &rotation);
        nw4r::math::MTX34Mult(&transformed, &rotated, &animated);
        animated = transformed;
        std::tr1::shared_ptr<BWorldObj> target = mTarget.lock();
        if (target) {
            std::tr1::shared_ptr<BModelObj> model = DynamicCast<BModelObj>(target);
            if (model) {
                model->SetBaseMtx(animated);
            }
        }
    }
}

void CNullObj::Calc() {
    UpdateFrame(1.0f);
    if (!mMotionPrograms.empty()) {
        ApplyFrame();
    }
}
