#include <game/CModel.h>
#include <game/CFiber.h>
#include <game/CFileData.h>
#include <nw4r/g3d/res/g3d_resfile.h>
#include <nw4r/g3d/res/g3d_resmdl.h>
#include <nw4r/g3d/res/g3d_resnode.h>

#include <algorithm>

void CModel::CMotionProgram::Wait() {
    while (!IsCompleted()) {
        YieldFiber();
    }
}

u32 CModel::CMotionProgram::TestFlags(u32 mask) const {
    return mFlags & mask;
}

void NotifyMotionCallback(const std::tr1::shared_ptr<CModel::CMotionProgram>& rMotion, bool notify, CModel* pModel) {
    CModel::CMotionProgram* pMotion = rMotion.get();
    if (notify) {
        pMotion->mFlags |= CModel::CMotionProgram::FLAG_NOTIFY_CALLBACK;
    } else {
        pMotion->mFlags &= ~CModel::CMotionProgram::FLAG_NOTIFY_CALLBACK;
    }
    if (notify && pModel != nullptr && rMotion->IsPlaying() && pModel->mMotionCallback) {
        pModel->mMotionCallback(pModel, rMotion);
    }
}

namespace {

const u32 kPlaybackStateMask = CModel::CMotionProgram::FLAG_PENDING | CModel::CMotionProgram::FLAG_PLAYING |
                               CModel::CMotionProgram::FLAG_LOOPED | CModel::CMotionProgram::FLAG_FINISHED |
                               CModel::CMotionProgram::FLAG_STOPPED;

inline void SetPlaybackState(CModel::CMotionProgram* pMotion, u32 state) {
    pMotion->mFlags = (pMotion->mFlags & ~kPlaybackStateMask) | state;
}

}

CModel::CModel() : mNextMotionID(1), mAnimationChanged(true), mAimTargetPending(false) {}

CModel::~CModel() {
    Unload();
}

void CModel::BPartialAnm::Reset() {
    mAnimObj.reset();
    for (std::list<std::tr1::shared_ptr<CMotionProgram> >::iterator it = mMotions.begin(); it != mMotions.end(); ++it) {
        SetPlaybackState(it->get(), CMotionProgram::FLAG_STOPPED);
    }
    mMotions.clear();
}

CModel::BPartialAnm::~BPartialAnm() {
    Reset();
}

void CModel::Unload() {
    mAnimations.clear();
    mPartialAnimations.reset();
    mMainAnimation.Reset();
    mModelData.Reset();
}

nw4r::g3d::ResFile CModel::GetResFile() const {
    return GetResMdl().GetParent();
}

void CModel::AddResourceFile(std::tr1::shared_ptr<CFileData> file) {
    std::vector<std::tr1::shared_ptr<CFileData> >& rFiles = mModelData.GetFiles();
    if (std::find(rFiles.begin(), rFiles.end(), file) == rFiles.end()) {
        rFiles.push_back(file);
    }
}

bool CModel::Load(const LoadParams& rParams, bool background) {
    Unload();
    LoadParams params = rParams;
    if (!LoadScnMdlSimpleData(&mModelData, params, 0, background)) {
        return false;
    }
    mModelData.GetModel()->SetCallback(this);
    mModelData.GetModel()->EnableScnObjCallbackTiming(nw4r::g3d::ScnObj::CALLBACK_TIMING_A);
    mModelData.GetModel()->EnableScnObjCallbackExecOp(nw4r::g3d::ScnObj::EXECOP_CALC_WORLD);
    mMainAnimation.mFrame = 0.0f;
    return true;
}

bool CModel::SetAimNode(const char* pNodeName) {
    mAnimationControlsAim = false;
    mWorldAimTarget = false;
    nw4r::g3d::ResNode node = GetResMdl().GetResNode(pNodeName);
    if (!mAim.Initialize(mModelData.GetModel(), node)) {
        return false;
    }
    mModelData.GetModel()->EnableScnObjCallbackExecOp(nw4r::g3d::ScnObj::EXECOP_CALC_VIEW);
    mModelData.GetModel()->SetScnMdlCallback(this);
    mModelData.GetModel()->SetScnMdlCallbackNodeID(node.IsValid() ? node.ref().id : 0);
    mModelData.GetModel()->EnableScnMdlCallbackTiming(nw4r::g3d::ScnObj::CALLBACK_TIMING_A);
    return true;
}

void CModel::DisableAimLimit() {
    mAim.DisableLimit();
}

void CModel::SetAimTarget(const nw4r::math::VEC3& rTarget, bool snap) {
    mAimTarget = rTarget;
    mWorldAimTarget = true;
    mAim.EnableLimit();
    if (snap) {
        mAimTargetPending = true;
    }
}

int CModel::GetMotionLength(const char* pName) {
    std::vector<std::tr1::shared_ptr<CFileData> >& rFiles = mModelData.GetFiles();
    std::vector<std::tr1::shared_ptr<CFileData> >::iterator it = rFiles.begin();
    nw4r::g3d::ResAnmChr resource;
    for (; it != rFiles.end(); ++it) {
        resource = nw4r::g3d::ResFile((*it)->GetData()).GetResAnmChr(pName);
        if (resource.IsValid()) {
            break;
        }
    }
    return resource.IsValid() ? resource.GetNumFrame() : 0;
}

void CModel::SetRootScale(const std::tr1::shared_ptr<nw4r::g3d::AnmObjChrRootScale>& rRootScale) {
    if (rRootScale.get() != mRootScale.get()) {
        if (mPartialAnimations) {
            if (mPartialAnimations->mAnimObj) {
                mPartialAnimations->mAnimObj->DetachFromParent();
            }
        } else if (mMainAnimation.mAnimObj) {
            mMainAnimation.mAnimObj->DetachFromParent();
        }
        mRootScale = rRootScale;
        mAnimationChanged = true;
    }
}

std::tr1::shared_ptr<CModel::CMotionProgram> CModel::GetCurrentMotion() {
    if (mMainAnimation.mMotions.size() != 0) {
        return mMainAnimation.mMotions.front();
    }
    return std::tr1::shared_ptr<CMotionProgram>();
}

std::tr1::shared_ptr<CModel::CMotionProgram> CModel::FindMotion(u32 id) {
    if (!mMainAnimation.mMotions.empty()) {
        for (std::list<std::tr1::shared_ptr<CMotionProgram> >::iterator it = mMainAnimation.mMotions.begin(); it != mMainAnimation.mMotions.end(); ++it) {
            if ((*it)->mId == id) {
                return *it;
            }
        }
    }
    if (mPartialAnimations) {
        for (std::map<int, std::tr1::shared_ptr<TPartialAnm<false> > >::iterator partial = mPartialAnimations->mPartials.begin();
             partial != mPartialAnimations->mPartials.end(); ++partial) {
            std::list<std::tr1::shared_ptr<CMotionProgram> >& rMotions = partial->second->mMotions;
            for (std::list<std::tr1::shared_ptr<CMotionProgram> >::iterator it = rMotions.begin(); it != rMotions.end(); ++it) {
                if ((*it)->mId == id) {
                    return *it;
                }
            }
        }
    }
    return std::tr1::shared_ptr<CMotionProgram>();
}

std::tr1::shared_ptr<CModel::CMotionProgram> CModel::Play(const char* pName, f32 frame, f32 rate) {
    BPartialAnm* pPartial = nullptr;
    std::tr1::shared_ptr<CMotionProgram> motion = CreateMotion(pName, frame, rate, &pPartial);
    if (motion) {
        if (!pPartial->SetMotion(this, motion)) {
            mAnimations.clear();
        }
    }
    return motion;
}

std::tr1::shared_ptr<CModel::CMotionProgram> CModel::Play(int index, f32 frame, f32 rate) {
    BPartialAnm* pPartial = nullptr;
    std::tr1::shared_ptr<CMotionProgram> motion = CreateMotion(index, frame, rate, &pPartial);
    if (motion) {
        if (!pPartial->SetMotion(this, motion)) {
            mAnimations.clear();
        }
    }
    return motion;
}

std::tr1::shared_ptr<CModel::CMotionProgram> CModel::PlayLoop(const char* pName, f32 frame, f32 rate) {
    BPartialAnm* pPartial = nullptr;
    std::tr1::shared_ptr<CMotionProgram> motion = CreateMotion(pName, frame, rate, &pPartial);
    if (motion) {
        if (!pPartial->QueueMotion(this, motion)) {
            mAnimations.clear();
        }
    }
    return motion;
}

std::tr1::shared_ptr<CModel::CMotionProgram> CModel::PlayOnce(const char* pName, f32 frame, f32 rate) {
    BPartialAnm* pPartial = nullptr;
    std::tr1::shared_ptr<CMotionProgram> motion = CreateMotion(pName, frame, rate, &pPartial);
    if (motion) {
        if (!pPartial->AppendMotion(this, motion)) {
            mAnimations.clear();
        }
    }
    return motion;
}

void CModel::Stop() {
    mAnimations.clear();
    mPartialAnimations.reset();
    mMainAnimation.Reset();
}

void CModel::Pause(bool pause) {
    if (mMainAnimation.mMotions.size() != 0) {
        CMotionProgram* pMotion = mMainAnimation.mMotions.front().get();
        if (pause) {
            pMotion->Pause();
        } else {
            pMotion->Resume();
        }
    }
    if (mPartialAnimations) {
        for (std::map<int, std::tr1::shared_ptr<TPartialAnm<false> > >::iterator partial = mPartialAnimations->mPartials.begin();
             partial != mPartialAnimations->mPartials.end(); ++partial) {
            if (partial->second->mMotions.size() != 0) {
                CMotionProgram* pMotion = partial->second->mMotions.front().get();
                if (pause) {
                    pMotion->Pause();
                } else {
                    pMotion->Resume();
                }
            }
        }
    }
}

void CModel::UpdateFrame(f32 step) {
    if (!mMainAnimation.Update(this, step)) {
        mAnimations.clear();
    }
    if (mPartialAnimations) {
        bool erased = false;
        std::map<int, std::tr1::shared_ptr<TPartialAnm<false> > >::iterator it = mPartialAnimations->mPartials.begin();
        while (it != mPartialAnimations->mPartials.end()) {
            std::map<int, std::tr1::shared_ptr<TPartialAnm<false> > >::iterator current = it;
            ++it;
            current->second->Update(this, step);
            if (current->second->mMotions.size() == 0) {
                erased = true;
                mPartialAnimations->mPartials.erase(current);
            }
        }
        if (erased) {
            if (mPartialAnimations->mPartials.size() != 0) {
                mPartialAnimations->mAnimObj.reset();
            } else {
                mPartialAnimations.reset();
                mAnimationChanged = true;
            }
        }
    }
}

void CModel::SetMotionCallback(const MotionCallback& rCallback) {
    MotionCallback callback(rCallback);
    mMotionCallback.swap(callback);
}

template <bool Main> bool CModel::TPartialAnm<Main>::SetMotion(CModel* pModel, const std::tr1::shared_ptr<CMotionProgram>& rMotion) {
    for (std::list<std::tr1::shared_ptr<CMotionProgram> >::iterator it = mMotions.begin(); it != mMotions.end(); ++it) {
        SetPlaybackState(it->get(), CMotionProgram::FLAG_STOPPED);
    }
    mMotions.clear();
    mMotions.push_back(rMotion);
    SetPlaybackState(rMotion.get(), CMotionProgram::FLAG_PLAYING);
    f32 previousFrame = mFrame;
    if (rMotion->mStartFrame < 0.0f) {
        mFrame = 0.0f;
        if (pModel->mMotionCallback) {
            pModel->mMotionCallback(pModel, rMotion);
        }
        return Update(pModel, previousFrame);
    }
    mFrame = rMotion->mStartFrame;
    if (pModel->mMotionCallback) {
        pModel->mMotionCallback(pModel, rMotion);
    }
    return true;
}

template <bool Main> bool CModel::TPartialAnm<Main>::QueueMotion(CModel* pModel, const std::tr1::shared_ptr<CMotionProgram>& rMotion) {
    if (IsIdle()) {
        return SetMotion(pModel, rMotion);
    }
    if (mMotions.size() >= 2) {
        std::list<std::tr1::shared_ptr<CMotionProgram> > current;
        current.push_back(mMotions.front());
        mMotions.pop_front();
        for (std::list<std::tr1::shared_ptr<CMotionProgram> >::iterator it = mMotions.begin(); it != mMotions.end(); ++it) {
            SetPlaybackState(it->get(), CMotionProgram::FLAG_STOPPED);
        }
        mMotions.clear();
        mMotions.swap(current);
    }
    mMotions.push_back(rMotion);
    return true;
}

template <bool Main> bool CModel::TPartialAnm<Main>::AppendMotion(CModel* pModel, const std::tr1::shared_ptr<CMotionProgram>& rMotion) {
    if (IsIdle()) {
        return SetMotion(pModel, rMotion);
    }
    mMotions.push_back(rMotion);
    return true;
}

template <bool Main> bool CModel::TPartialAnm<Main>::Update(CModel* pModel, f32 step) {
    if (IsIdle()) {
        mFrame += step;
        return true;
    }
    bool advanced = false;
    for (;;) {
        CMotionProgram* pMotion = mMotions.front().get();
        if (pMotion->mFlags & CMotionProgram::FLAG_PAUSED) {
            return true;
        }
        f32 frame = mFrame + step * pMotion->mRate;
        f32 length = static_cast<f32>(pMotion->mLength);
        mFrame = frame;
        if (frame < length) {
            return !advanced;
        }
        mFrame = frame - length;
        if (mMotions.size() >= 2) {
            step = mFrame / pMotion->mRate;
            SetPlaybackState(pMotion, CMotionProgram::FLAG_STOPPED);
            mMotions.pop_front();
            mFrame = std::max(0.0f, mMotions.front()->mStartFrame);
            mAnimObj.reset();
            advanced = true;
            SetPlaybackState(mMotions.front().get(), CMotionProgram::FLAG_PLAYING);
            if (pModel->mMotionCallback) {
                pModel->mMotionCallback(pModel, mMotions.front());
            }
        } else if (pMotion->mResource.GetAnmPolicy() == nw4r::g3d::ANM_POLICY_LOOP) {
            step = 0.0f;
            SetPlaybackState(pMotion, CMotionProgram::FLAG_LOOPED);
            pMotion->mFlags += 0x10000;
        } else {
            if (pMotion->mRate != 0.0f) {
                mFrame = mFrame / pMotion->mRate;
            }
            SetPlaybackState(pMotion, CMotionProgram::FLAG_FINISHED);
            return true;
        }
    }
}
