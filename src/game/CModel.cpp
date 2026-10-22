#include <game/CModel.h>
#include <game/ScnMdlSimpleDataInline.h>
#include <game/CFiber.h>
#include <game/CFileData.h>
#include <game/AnmObjChrRootScale.h>
#include <game/FrameStep.h>
#include <game/Memory.h>
#include <nw4r/g3d/res/g3d_resfile.h>
#include <nw4r/g3d/res/g3d_resmdl.h>
#include <nw4r/g3d/res/g3d_resnode.h>
#include <algorithm>
#include <math.h>


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

CModel::BPartialAnm::~BPartialAnm() {
    Reset();
}

bool CModel::BPartialAnm::CreateAnimObj(nw4r::g3d::ResMdl mdl) {
    if (mMotions.size() == 0) {
        mAnimObj.reset();
        return false;
    }
    std::tr1::shared_ptr<CMotionProgram>& rMotion = mMotions.front();
    bool needsObject = false;
    if (!mAnimObj) {
        needsObject = true;
    } else if (rMotion->mResource.ptr() != mAnimObj->GetResAnm().ptr()) {
        needsObject = true;
    }
    if (needsObject && (rMotion->mFlags & CMotionProgram::FLAG_BIND_FAILED)) {
        return false;
    }
    if (!needsObject) {
        return false;
    }
    mAnimObj.reset(nw4r::g3d::AnmObjChrRes::Construct(GetDefaultAllocator(), nullptr, rMotion->mResource, mdl, false), DestroyG3dObj);
    mAnimObj->SetFrame(rMotion->GetCurrentFrame());
    rMotion->SetBindFailed(!mAnimObj->Bind(mdl));
    if (rMotion->mFlags & CMotionProgram::FLAG_BIND_FAILED) {
        mAnimObj.reset();
        return false;
    }
    return true;
}

CModel::CModel() : mNextMotionID(1), mAnimationChanged(true), mAimTargetPending(false) {}

CModel::~CModel() {
    Unload();
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
    LoadBounds(&mBounds, GetResFile().GetResMdl(0));
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

namespace {

struct ChrAnimTraits {
    typedef nw4r::g3d::ResAnmChr Res;
    template <class Key> static Res Find(std::vector<std::tr1::shared_ptr<CFileData> >& rFiles, Key key) {
        Res resource;
        std::vector<std::tr1::shared_ptr<CFileData> >::iterator it = rFiles.begin();
        for (; it != rFiles.end(); ++it) {
            nw4r::g3d::ResFile file((*it)->GetData());
            resource = file.GetResAnmChr(key);
            if (resource.IsValid()) {
                break;
            }
        }
        return resource;
    }
};

struct VisAnimTraits {
    typedef nw4r::g3d::ResAnmVis Res;
    template <class Key> static Res Find(std::vector<std::tr1::shared_ptr<CFileData> >& rFiles, Key key) {
        Res resource;
        std::vector<std::tr1::shared_ptr<CFileData> >::iterator it = rFiles.begin();
        for (; it != rFiles.end(); ++it) {
            nw4r::g3d::ResFile file((*it)->GetData());
            resource = file.GetResAnmVis(key);
            if (resource.IsValid()) {
                break;
            }
        }
        return resource;
    }
};

struct MatClrAnimTraits {
    typedef nw4r::g3d::AnmObjMatClrRes Obj;
    typedef nw4r::g3d::ResAnmClr Res;
    template <class Key> static Res Find(std::vector<std::tr1::shared_ptr<CFileData> >& rFiles, Key key) {
        Res resource;
        std::vector<std::tr1::shared_ptr<CFileData> >::iterator it = rFiles.begin();
        for (; it != rFiles.end(); ++it) {
            nw4r::g3d::ResFile file((*it)->GetData());
            resource = file.GetResAnmClr(key);
            if (resource.IsValid()) {
                break;
            }
        }
        return resource;
    }
};

struct TexPatAnimTraits {
    typedef nw4r::g3d::AnmObjTexPatRes Obj;
    typedef nw4r::g3d::ResAnmTexPat Res;
    template <class Key> static Res Find(std::vector<std::tr1::shared_ptr<CFileData> >& rFiles, Key key) {
        Res resource;
        std::vector<std::tr1::shared_ptr<CFileData> >::iterator it = rFiles.begin();
        for (; it != rFiles.end(); ++it) {
            nw4r::g3d::ResFile file((*it)->GetData());
            resource = file.GetResAnmTexPat(key);
            if (resource.IsValid()) {
                break;
            }
        }
        return resource;
    }
};

struct TexSrtAnimTraits {
    typedef nw4r::g3d::AnmObjTexSrtRes Obj;
    typedef nw4r::g3d::ResAnmTexSrt Res;
    template <class Key> static Res Find(std::vector<std::tr1::shared_ptr<CFileData> >& rFiles, Key key) {
        Res resource;
        std::vector<std::tr1::shared_ptr<CFileData> >::iterator it = rFiles.begin();
        for (; it != rFiles.end(); ++it) {
            nw4r::g3d::ResFile file((*it)->GetData());
            resource = file.GetResAnmTexSrt(key);
            if (resource.IsValid()) {
                break;
            }
        }
        return resource;
    }
};

struct ShpAnimTraits {
    typedef nw4r::g3d::AnmObjShpRes Obj;
    typedef nw4r::g3d::ResAnmShp Res;
    template <class Key> static Res Find(std::vector<std::tr1::shared_ptr<CFileData> >& rFiles, Key key) {
        Res resource;
        std::vector<std::tr1::shared_ptr<CFileData> >::iterator it = rFiles.begin();
        for (; it != rFiles.end(); ++it) {
            nw4r::g3d::ResFile file((*it)->GetData());
            resource = file.GetResAnmShp(key);
            if (resource.IsValid()) {
                break;
            }
        }
        return resource;
    }
};

}

int CModel::GetMotionLength(const char* pName) {
    nw4r::g3d::ResAnmChr resource = ChrAnimTraits::Find(mModelData.GetFiles(), pName);
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
        for (std::map<u32, std::tr1::shared_ptr<TPartialAnm<false> > >::iterator partial = mPartialAnimations->mPartials.begin();
             partial != mPartialAnimations->mPartials.end(); ++partial) {
            for (std::list<std::tr1::shared_ptr<CMotionProgram> >::iterator it = partial->second->mMotions.begin(); it != partial->second->mMotions.end(); ++it) {
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
        for (std::map<u32, std::tr1::shared_ptr<TPartialAnm<false> > >::iterator partial = mPartialAnimations->mPartials.begin();
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
        std::map<u32, std::tr1::shared_ptr<TPartialAnm<false> > >::iterator it = mPartialAnimations->mPartials.begin();
        while (it != mPartialAnimations->mPartials.end()) {
            std::map<u32, std::tr1::shared_ptr<TPartialAnm<false> > >::iterator next = it;
            ++next;
            TPartialAnm<false>* pPartial = it->second.get();
            pPartial->Update(this, step);
            if (pPartial->mMotions.size() == 0) {
                erased = true;
                mPartialAnimations->mPartials.erase(it);
            }
            it = next;
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

CModel::BPartialAnm* CModel::GetPartialAnm(u32 nodeId) {
    if (nodeId == 0) {
        return &mMainAnimation;
    }
    if (!mPartialAnimations) {
        mPartialAnimations.reset(new PartialAnmManager);
    } else {
        std::map<u32, std::tr1::shared_ptr<TPartialAnm<false> > >::iterator it = mPartialAnimations->mPartials.find(nodeId);
        if (it != mPartialAnimations->mPartials.end()) {
            return it->second.get();
        }
    }
    mPartialAnimations->mAnimObj.reset();
    std::tr1::shared_ptr<TPartialAnm<false> > partial(new TPartialAnm<false>);
    mPartialAnimations->mPartials[nodeId] = partial;
    return partial.get();
}

std::tr1::shared_ptr<CModel::CMotionProgram> CModel::SetupMotion(const std::tr1::shared_ptr<CMotionProgram>& rProgram, f32 frame, f32 rate, BPartialAnm** ppPartial) {
    nw4r::g3d::ResAnmChr resource = rProgram->mResource;
    if (!resource.IsValid()) {
        return std::tr1::shared_ptr<CMotionProgram>();
    }
    const nw4r::g3d::ResAnmChrNodeData* pNodeAnm = resource.GetNodeAnm(0);
    nw4r::g3d::ResMdl mdl = GetResMdl();
    nw4r::g3d::ResNode node = mdl.GetResNode(nw4r::g3d::ResName(reinterpret_cast<const u8*>(pNodeAnm) + pNodeAnm->name - 4));
    if (!node.IsValid()) {
        return std::tr1::shared_ptr<CMotionProgram>();
    }
    BPartialAnm* pPartial = GetPartialAnm(node.IsValid() ? node.ref().id : 0);
    *ppPartial = pPartial;
    rProgram->mId = mNextMotionID;
    rProgram->mStartFrame = frame;
    rProgram->mRate = rate;
    rProgram->mpUserData = nullptr;
    rProgram->mpCurrentFrame = &pPartial->mFrame;
    rProgram->mLength = rProgram->mResource.GetNumFrame();
    rProgram.get()->SetPlaybackState(CMotionProgram::FLAG_PENDING);
    if (++mNextMotionID == 0) {
        ++mNextMotionID;
    }
    return rProgram;
}

std::tr1::shared_ptr<CModel::CMotionProgram> CModel::CreateMotion(const char* pName, f32 frame, f32 rate, BPartialAnm** ppPartial) {
    if (pName == nullptr) {
        return std::tr1::shared_ptr<CMotionProgram>();
    }
    std::tr1::shared_ptr<CMotionProgram> program(new CMotionProgram);
    program->mFlags = 0;
    program->mFlags |= CMotionProgram::FLAG_CREATED;
    nw4r::g3d::ResAnmChr resource = ChrAnimTraits::Find(mModelData.GetFiles(), pName);
    program->mResource = resource;
    program->mName = pName;
    return SetupMotion(program, frame, rate, ppPartial);
}

std::tr1::shared_ptr<CModel::CMotionProgram> CModel::CreateMotion(int index, f32 frame, f32 rate, BPartialAnm** ppPartial) {
    std::tr1::shared_ptr<CMotionProgram> program(new CMotionProgram);
    program->mFlags = 0;
    nw4r::g3d::ResAnmChr resource = ChrAnimTraits::Find(mModelData.GetFiles(), index);
    program->mResource = resource;
    program->mReserved = index;
    return SetupMotion(program, frame, rate, ppPartial);
}

template <class Traits, class Key> std::tr1::shared_ptr<typename Traits::Obj> CModel::CreateAnimObj(Key key, f32 frame) {
    typename Traits::Res resource = Traits::Find(mModelData.GetFiles(), key);
    if (!resource.IsValid()) {
        return std::tr1::shared_ptr<typename Traits::Obj>();
    }
    nw4r::g3d::ResMdl mdl = GetResMdl();
    std::tr1::shared_ptr<typename Traits::Obj> animObj(Traits::Obj::Construct(GetDefaultAllocator(), nullptr, resource, mdl, false), DestroyG3dObj);
    animObj->SetFrame(frame);
    if (!animObj->Bind(mdl)) {
        return std::tr1::shared_ptr<typename Traits::Obj>();
    }
    mAnimations.push_back(animObj);
    mModelData.GetModel()->SetAnmObj(animObj.get(), nw4r::g3d::ScnMdlSimple::ANMOBJTYPE_NOT_SPECIFIED);
    return animObj;
}

nw4r::g3d::ResAnmVis CModel::FindVis(const char* pName) {
    nw4r::g3d::ResAnmVis resource;
    std::vector<std::tr1::shared_ptr<CFileData> >& rFiles = mModelData.GetFiles();
    std::vector<std::tr1::shared_ptr<CFileData> >::iterator it = rFiles.begin();
    for (; it != rFiles.end(); ++it) {
        resource = nw4r::g3d::ResFile((*it)->GetData()).GetResAnmVis(pName);
        if (resource.IsValid()) {
            break;
        }
    }
    return resource;
}

nw4r::g3d::ResAnmVis CModel::FindVis(int index) {
    nw4r::g3d::ResAnmVis resource;
    std::vector<std::tr1::shared_ptr<CFileData> >& rFiles = mModelData.GetFiles();
    std::vector<std::tr1::shared_ptr<CFileData> >::iterator it = rFiles.begin();
    for (; it != rFiles.end(); ++it) {
        resource = nw4r::g3d::ResFile((*it)->GetData()).GetResAnmVis(index);
        if (resource.IsValid()) {
            break;
        }
    }
    return resource;
}

void CModel::UpdateAnimations() {
    mAnimationControlsAim = false;
    bool overrideCreated = false;
    bool animationChanged = mAnimationChanged;
    nw4r::g3d::ResMdl mdl = GetResMdl();
    if (mPartialAnimations) {
        if (!mPartialAnimations->mAnimObj) {
            mPartialAnimations->mAnimObj.reset(nw4r::g3d::AnmObjChrOverride::Construct(GetDefaultAllocator(), nullptr, mdl, mPartialAnimations->mPartials.size() + 1), DestroyG3dObj);
            overrideCreated = true;
            animationChanged = true;
        }
        int attachIndex = 1;
        std::map<u32, std::tr1::shared_ptr<TPartialAnm<false> > >::iterator it = mPartialAnimations->mPartials.begin();
        for (; it != mPartialAnimations->mPartials.end(); ++it, ++attachIndex) {
            TPartialAnm<false>* pPartial = it->second.get();
            bool created = pPartial->CreateAnimObj(mdl);
            nw4r::g3d::AnmObjChrRes* pAnimObj = pPartial->mAnimObj.get();
            if (pAnimObj != nullptr) {
                mAnimationControlsAim = mAnimationControlsAim || !pPartial->mMotions.front()->IsOneShotTexture();
                if (created || overrideCreated) {
                    pAnimObj->DetachFromParent();
                    mPartialAnimations->mAnimObj->AttachChild(attachIndex, pAnimObj);
                }
                if (!created) {
                    f32 frame = pAnimObj->GetFrame();
                    nw4r::g3d::FrameCtrl::SetBaseUpdateRate(pPartial->mFrame - frame);
                    pAnimObj->UpdateFrame();
                }
            }
        }
    }
    TPartialAnm<true>& rMain = mMainAnimation;
    bool created = rMain.CreateAnimObj(mdl);
    nw4r::g3d::AnmObjChrRes* pMainObj = rMain.mAnimObj.get();
    if (pMainObj != nullptr) {
        mAnimationControlsAim = mAnimationControlsAim || !rMain.mMotions.front()->IsOneShotTexture();
        if (created || overrideCreated) {
            if (mPartialAnimations) {
                pMainObj->DetachFromParent();
                mPartialAnimations->mAnimObj->AttachChild(0, pMainObj);
            } else {
                animationChanged = true;
            }
        }
        if (!created) {
            f32 frame = pMainObj->GetFrame();
            nw4r::g3d::FrameCtrl::SetBaseUpdateRate(rMain.mMotions.front()->GetCurrentFrame() - frame);
            pMainObj->UpdateFrame();
            for (std::vector<std::tr1::shared_ptr<nw4r::g3d::AnmObj> >::iterator it = mAnimations.begin(); it != mAnimations.end(); ++it) {
                (*it)->UpdateFrame();
            }
        } else {
            mAnimations.clear();
            mAnimations.reserve(6);
            std::tr1::shared_ptr<CMotionProgram>& rMotion = rMain.mMotions.front();
            f32 frame = rMain.mFrame;
            nw4r::g3d::ResAnmVis vis;
            if (rMotion->TestFlags(CMotionProgram::FLAG_CREATED)) {
                vis = FindVis(rMotion->mName.c_str());
            } else {
                vis = FindVis(rMotion->mReserved);
            }
            if (vis.IsValid()) {
                std::tr1::shared_ptr<nw4r::g3d::AnmObjVisRes> visObj(nw4r::g3d::AnmObjVisRes::Construct(GetDefaultAllocator(), nullptr, vis, mdl), DestroyG3dObj);
                visObj->SetFrame(frame);
                visObj->Bind(mdl);
                mAnimations.push_back(visObj);
                mModelData.GetModel()->SetAnmObj(visObj.get(), nw4r::g3d::ScnMdlSimple::ANMOBJTYPE_NOT_SPECIFIED);
            }
            if (rMotion->TestFlags(CMotionProgram::FLAG_CREATED)) {
                CreateAnimObj<MatClrAnimTraits>(rMotion->mName.c_str(), frame);
                CreateAnimObj<TexPatAnimTraits>(rMotion->mName.c_str(), frame);
                CreateAnimObj<TexSrtAnimTraits>(rMotion->mName.c_str(), frame);
                CreateAnimObj<ShpAnimTraits>(rMotion->mName.c_str(), frame);
            } else {
                CreateAnimObj<MatClrAnimTraits>(rMotion->mReserved, frame);
                CreateAnimObj<TexPatAnimTraits>(rMotion->mReserved, frame);
                CreateAnimObj<TexSrtAnimTraits>(rMotion->mReserved, frame);
                CreateAnimObj<ShpAnimTraits>(rMotion->mReserved, frame);
            }
        }
    }
    if (animationChanged) {
        nw4r::g3d::G3dObj* pRoot;
        if (mPartialAnimations) {
            pRoot = mPartialAnimations->mAnimObj.get();
        } else {
            pRoot = rMain.mAnimObj.get();
        }
        if (pRoot != nullptr) {
            if (mRootScale) {
                pRoot->DetachFromParent();
                mRootScale->AttachChild(0, static_cast<nw4r::g3d::AnmObjChr*>(pRoot));
                mModelData.GetModel()->SetAnmObj(mRootScale.get(), nw4r::g3d::ScnMdlSimple::ANMOBJTYPE_NOT_SPECIFIED);
            } else {
                mModelData.GetModel()->SetAnmObj(static_cast<nw4r::g3d::AnmObj*>(pRoot), nw4r::g3d::ScnMdlSimple::ANMOBJTYPE_NOT_SPECIFIED);
            }
        }
        mAnimationChanged = false;
    }
}

void CModel::ExecCallback_CALC_WORLD(nw4r::g3d::ScnObj::Timing timing, nw4r::g3d::ScnObj* pObject, u32 param, void* pInfo) {
    if (timing == nw4r::g3d::ScnObj::CALLBACK_TIMING_A) {
        UpdateAnimations();
    }
}

void CModel::ExecCallbackA(nw4r::g3d::ChrAnmResult* pResult, nw4r::g3d::ResMdl mdl, nw4r::g3d::FuncObjCalcWorld* pFuncObj) {
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

void CModel::ExecCallback_CALC_VIEW(nw4r::g3d::ScnObj::Timing timing, nw4r::g3d::ScnObj* pObject, u32 param, void* pInfo) {
    if (timing == nw4r::g3d::ScnObj::CALLBACK_TIMING_A && mAim.mpModel != nullptr && !mAnimationControlsAim) {
        if (!mWorldAimTarget) {
            mAim.AimAtMatrixOrigin(*static_cast<nw4r::math::MTX34*>(pInfo), static_cast<int>(ceil(static_cast<f32>(gFrameStep))));
        } else {
            mAim.AimAtPoint(mAimTarget, gFrameStep);
        }
    }
}

bool CModel::LoadBounds(nw4r::math::AABB* pBounds, const nw4r::g3d::ResMdl& rMdl) {
    bool loaded = false;
    nw4r::g3d::ResNode root = rMdl.GetResNode(0);
    if (root.IsValid()) {
        nw4r::g3d::ResUserData userData = root.GetResUserData();
        if (userData.IsValid()) {
            const nw4r::g3d::ResDic dic(&userData.ref().dict);
            nw4r::g3d::ResUserDataEntry entry(dic["COLL"]);
            if (entry.IsValid()) {
                const f32* pValues = entry.ref().toData != 0 ? reinterpret_cast<const f32*>(reinterpret_cast<const u8*>(entry.ptr()) + entry.ref().toData) : nullptr;
                loaded = true;
                pBounds->min.x = pValues[0];
                pBounds->min.y = pValues[1];
                pBounds->min.z = pValues[2];
                pBounds->max.x = pValues[3];
                pBounds->max.y = pValues[4];
                pBounds->max.z = pValues[5];
            }
        }
    }
    if (!loaded) {
        nw4r::g3d::ResMdlInfo info = rMdl.GetResMdlInfo();
        if (info.IsValid() && info.ref().is_valid_volume) {
            loaded = true;
            pBounds->min.x = info.ref().volume_min.x;
            pBounds->min.y = info.ref().volume_min.y;
            pBounds->min.z = info.ref().volume_min.z;
            pBounds->max.x = info.ref().volume_max.x;
            pBounds->max.y = info.ref().volume_max.y;
            pBounds->max.z = info.ref().volume_max.z;
        }
    }
    if (!loaded) {
        pBounds->min.x = 0.0f;
        pBounds->min.y = 0.0f;
        pBounds->min.z = 0.0f;
        pBounds->max.x = 0.0f;
        pBounds->max.y = 0.0f;
        pBounds->max.z = 0.0f;
    }
    return loaded;
}

template <bool Main> bool CModel::TPartialAnm<Main>::SetMotion(CModel* pModel, const std::tr1::shared_ptr<CMotionProgram>& rMotion) {
    for (std::list<std::tr1::shared_ptr<CMotionProgram> >::iterator it = mMotions.begin(); it != mMotions.end(); ++it) {
        it->get()->SetPlaybackState(CMotionProgram::FLAG_STOPPED);
    }
    mMotions.clear();
    mMotions.push_back(rMotion);
    rMotion.get()->SetPlaybackState(CMotionProgram::FLAG_PLAYING);
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
    if (IsReplaceable()) {
        return SetMotion(pModel, rMotion);
    }
    if (mMotions.size() >= 2) {
        std::list<std::tr1::shared_ptr<CMotionProgram> > current;
        current.push_back(mMotions.front());
        mMotions.pop_front();
        for (std::list<std::tr1::shared_ptr<CMotionProgram> >::iterator it = mMotions.begin(); it != mMotions.end(); ++it) {
            it->get()->SetPlaybackState(CMotionProgram::FLAG_STOPPED);
        }
        mMotions.clear();
        mMotions.swap(current);
    }
    mMotions.push_back(rMotion);
    return true;
}

template <bool Main> bool CModel::TPartialAnm<Main>::AppendMotion(CModel* pModel, const std::tr1::shared_ptr<CMotionProgram>& rMotion) {
    if (IsReplaceable()) {
        return SetMotion(pModel, rMotion);
    }
    mMotions.push_back(rMotion);
    return true;
}

template <bool Main> bool CModel::TPartialAnm<Main>::Update(CModel* pModel, f32 step) {
    if (IsReplaceable()) {
        mFrame += step;
        return true;
    }
    bool advanced = false;
    for (;;) {
        std::tr1::shared_ptr<CMotionProgram>& rMotion = mMotions.front();
        if (rMotion->mFlags & CMotionProgram::FLAG_PAUSED) {
            return true;
        }
        f32 length = static_cast<f32>(rMotion->mLength);
        mFrame += step * rMotion->mRate;
        if (mFrame < length) {
            return !advanced;
        }
        mFrame -= length;
        if (mMotions.size() >= 2) {
            step = mFrame / rMotion->mRate;
            rMotion->SetPlaybackState(CMotionProgram::FLAG_STOPPED);
            mMotions.pop_front();
            f32 start = mMotions.front()->mStartFrame;
            mFrame = start < 0.0f ? 0.0f : start;
            mAnimObj.reset();
            advanced = true;
            mMotions.front().get()->SetPlaybackState(CMotionProgram::FLAG_PLAYING);
            if (pModel->mMotionCallback) {
                pModel->mMotionCallback(pModel, mMotions.front());
            }
        } else if (rMotion->mResource.GetAnmPolicy() == nw4r::g3d::ANM_POLICY_LOOP) {
            step = 0.0f;
            rMotion->SetPlaybackState(CMotionProgram::FLAG_LOOPED);
            rMotion->mFlags += 0x10000;
        } else {
            if (rMotion->mRate != 0.0f) {
                mFrame = mFrame / rMotion->mRate;
            }
            rMotion->SetPlaybackState(Main ? CMotionProgram::FLAG_FINISHED : CMotionProgram::FLAG_STOPPED);
            if (!Main) {
                mMotions.pop_front();
                mAnimObj.reset();
                return false;
            }
            return true;
        }
    }
}

template <bool Main> CModel::TPartialAnm<Main>::~TPartialAnm() {}
