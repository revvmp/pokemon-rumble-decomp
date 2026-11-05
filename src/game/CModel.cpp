#define CMODEL_EMISSION_LAYOUT

#pragma section code_type ".text.model_prefix"
#pragma section code_type ".text.model_copy"
#pragma section code_type ".text.model_update"
#pragma section code_type ".text.model_compare"
#pragma section code_type ".text.model_reset"
#pragma section code_type ".text.model_iterators"
#pragma section code_type ".text.model_reserve"
#pragma section code_type ".text.model_vis_assignment"
#pragma section code_type ".text.model_push"
#pragma section code_type ".text.model_post_push"
#pragma section code_type ".text.model_root_bool"
#pragma section code_type ".text.model_middle"
#pragma section code_type ".text.model_owners"
#pragma section code_type ".text.model_shared"
#pragma section code_type ".text.model_moves"
#pragma section code_type ".text.model_holder"
#pragma section code_type ".text.model_allocator"
#pragma section code_type ".text.model_creators"
#pragma section code_type ".text.model_chr_deleter"
#pragma section code_type ".text.model_partial"
#pragma section code_type ".text.model_deleters"
#pragma section code_type ".text.model_split"
#pragma section code_type ".text.model_late_owners"
#pragma section code_type ".text.model_late_holders"
#pragma section code_type ".text.model_mat_deleter"
#pragma section code_type ".text.model_tex_deleter"
#pragma section code_type ".text.model_shp_deleter"
#pragma section code_type ".text.model_buffers"
#pragma section code_type ".text"
#pragma section code_type ".text.model_prefix"
#include <memory>
namespace nw4r { namespace g3d { class AnmObj; } }
enum { kModelAnimationPointerSize = sizeof(std::tr1::shared_ptr<nw4r::g3d::AnmObj>) };
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

typedef std::tr1::shared_ptr<CModel::CMotionProgram> ModelMotionPointer;
typedef std::list<ModelMotionPointer> ModelMotionList;
typedef std::tr1::shared_ptr<CModel::TPartialAnm<false> > ModelPartialPointer;
typedef std::pair<const u32, ModelPartialPointer> ModelMotionPair;
typedef std::detail::tree_imp<ModelMotionPair, std::detail::map_member_compare<u32, ModelPartialPointer, std::less<u32> >, std::allocator<ModelMotionPair> > ModelMotionTree;

template <class Tree> struct ModelInsertRequest {
    typedef typename Tree::node* (Tree::*Function)(std::detail::tree_node_base*, bool, bool, const typename Tree::value_type&);
};
template <class Tree> typename ModelInsertRequest<Tree>::Function RequestModelInsert0() {
    return &Tree::insert_node_at;
}

void CModel::CMotionProgram::Wait() {
    RequestModelInsert0<ModelMotionTree>();
    while (!IsCompleted()) {
        YieldFiber();
    }
}

u32 CModel::CMotionProgram::TestFlags(u32 mask) const {
    return mFlags & mask;
}

void NotifyMotionCallback(const std::tr1::shared_ptr<CModel::CMotionProgram>& rMotion, bool notify, CModel* pModel) {
    bool requestListDestructor = false;
    if (requestListDestructor) static_cast<ModelMotionList*>(nullptr)->~ModelMotionList();
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
    bool requestListFront = false;
    if (requestListFront) mMotions.front();
    Reset();
}

bool CModel::BPartialAnm::CreateAnimObj(nw4r::g3d::ResMdl mdl) {
    bool requestPlaybackFrame = false;
    if (requestPlaybackFrame) static_cast<CMotionProgram*>(nullptr)->GetPlaybackFrame();
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
    for (std::vector<std::tr1::shared_ptr<CFileData> >::iterator it = rFiles.begin(); it != rFiles.end(); ++it) {
        if (*it == file) {
            return;
        }
    }
    rFiles.push_back(file);
}

inline void CModel::Unload() {
        mAnimations.clear();
        mPartialAnimations.reset();
        mMainAnimation.Reset();
        mModelData.Reset();
    }

bool CModel::Load(const LoadParams& rParams, bool background) {
    Unload();
    LoadParams params = {rParams.mpFileNames, false, rParams.mSimple, rParams.mOptions, rParams.mMode, rParams.mpModelName};
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
    template <class Key> static Res Find(const std::vector<std::tr1::shared_ptr<CFileData> >& rFiles, Key key) {
        Res resource;
        std::vector<std::tr1::shared_ptr<CFileData> >::const_iterator it = rFiles.begin();
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
    template <class Key> static Res Find(const std::vector<std::tr1::shared_ptr<CFileData> >& rFiles, Key key) {
        Res resource;
        std::vector<std::tr1::shared_ptr<CFileData> >::const_iterator it = rFiles.begin();
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
    template <class Key> static Res Find(const std::vector<std::tr1::shared_ptr<CFileData> >& rFiles, Key key) {
        Res resource;
        std::vector<std::tr1::shared_ptr<CFileData> >::const_iterator it = rFiles.begin();
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
    template <class Key> static Res Find(const std::vector<std::tr1::shared_ptr<CFileData> >& rFiles, Key key) {
        Res resource;
        std::vector<std::tr1::shared_ptr<CFileData> >::const_iterator it = rFiles.begin();
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
    template <class Key> static Res Find(const std::vector<std::tr1::shared_ptr<CFileData> >& rFiles, Key key) {
        Res resource;
        std::vector<std::tr1::shared_ptr<CFileData> >::const_iterator it = rFiles.begin();
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
    template <class Key> static Res Find(const std::vector<std::tr1::shared_ptr<CFileData> >& rFiles, Key key) {
        Res resource;
        std::vector<std::tr1::shared_ptr<CFileData> >::const_iterator it = rFiles.begin();
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

int CModel::GetMotionLength(const char* pName) const {
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
    bool requestMapIterators = false;
    if (requestMapIterators) {
        ModelMotionTree::iterator it;
        mPartialAnimations->mPartials.end();
        (void)(it != it);
        it.operator->();
        mPartialAnimations->mPartials.begin();
        ++it;
    }
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

template std::detail::tree_node_base* ModelMotionTree::find_leaf<u32>(const u32&, ModelMotionTree::node*&, bool&, bool&);

void RequestModelMotionPairDestructorEmission() {
    bool requestPairDestructor = false;
    if (requestPairDestructor) static_cast<ModelMotionPair*>(nullptr)->~ModelMotionPair();
}

std::tr1::shared_ptr<CModel::CMotionProgram> CModel::SetupMotion(std::tr1::shared_ptr<CMotionProgram>& rProgram, f32 frame, f32 rate, BPartialAnm** ppPartial) {
    nw4r::g3d::ResAnmChr resource = rProgram->mResource;
    if (!resource.IsValid()) {
        return std::tr1::shared_ptr<CMotionProgram>();
    }
    const nw4r::g3d::ResAnmChrNodeData* pNodeAnm = resource.GetNodeAnm(0);
    nw4r::g3d::ResName name(reinterpret_cast<const u8*>(pNodeAnm) + (pNodeAnm->name - 4));
    nw4r::g3d::ResMdl mdl = GetResMdl();
    nw4r::g3d::ResNode node = mdl.GetResNode(name);
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

__declspec(section ".text.model_update") std::tr1::shared_ptr<CModel::CMotionProgram> CModel::CreateMotion(const char* pName, f32 frame, f32 rate, BPartialAnm** ppPartial) {
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

__declspec(section ".text.model_update") std::tr1::shared_ptr<CModel::CMotionProgram> CModel::CreateMotion(int index, f32 frame, f32 rate, BPartialAnm** ppPartial) {
    std::tr1::shared_ptr<CMotionProgram> program(new CMotionProgram);
    program->mFlags = 0;
    nw4r::g3d::ResAnmChr resource = ChrAnimTraits::Find(mModelData.GetFiles(), index);
    program->mResource = resource;
    program->mReserved = index;
    return SetupMotion(program, frame, rate, ppPartial);
}

namespace std { namespace tr1 {
template <> template <> __declspec(section ".text.model_reset")  void shared_ptr<nw4r::g3d::AnmObjChrOverride>::reset<nw4r::g3d::AnmObjChrOverride, void (*)(nw4r::g3d::G3dObj*)>(nw4r::g3d::AnmObjChrOverride* p, void (*d)(nw4r::g3d::G3dObj*));
} }
__declspec(section ".text.model_update") void CModel::UpdateAnimations() {
    bool requestAnimationComparison = false;
    if (requestAnimationComparison) {
        typedef std::vector<std::tr1::shared_ptr<nw4r::g3d::AnmObj> >::iterator AnimationIterator;
        (void)(*static_cast<AnimationIterator*>(nullptr) != *static_cast<AnimationIterator*>(nullptr));
    }
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
            nw4r::g3d::FrameCtrl::SetBaseUpdateRate(rMain.mMotions.front()->GetPlaybackFrame() - frame);
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

#pragma section code_type ".text.model_update"
namespace std { namespace tr1 {
template <> template <> __declspec(section ".text.model_reset")  void shared_ptr<nw4r::g3d::AnmObjChrOverride>::reset<nw4r::g3d::AnmObjChrOverride, void (*)(nw4r::g3d::G3dObj*)>(nw4r::g3d::AnmObjChrOverride* p, void (*d)(nw4r::g3d::G3dObj*)) { shared_ptr(p, d).swap(*this); }
} }
namespace std {
template <> __declspec(section ".text.model_reserve") void vector<tr1::shared_ptr<nw4r::g3d::AnmObj> >::reserve(unsigned long n) { base::reserve(n); }
template <> __declspec(section ".text.model_push") void vector<tr1::shared_ptr<nw4r::g3d::AnmObj> >::push_back(const tr1::shared_ptr<nw4r::g3d::AnmObj>& x) { base::push_back(x); }
}
__declspec(section ".text.model_middle") nw4r::g3d::ResAnmVis CModel::FindVis(const char* pName) const {
    nw4r::g3d::ResAnmVis resource;
    const std::vector<std::tr1::shared_ptr<CFileData> >& rFiles = mModelData.GetFiles();
    std::vector<std::tr1::shared_ptr<CFileData> >::const_iterator it = rFiles.begin();
    for (; it != rFiles.end(); ++it) {
        resource = nw4r::g3d::ResFile((*it)->GetData()).GetResAnmVis(pName);
        if (resource.IsValid()) {
            break;
        }
    }
    return resource;
}

__declspec(section ".text.model_middle") nw4r::g3d::ResAnmVis CModel::FindVis(int index) const {
    nw4r::g3d::ResAnmVis resource;
    const std::vector<std::tr1::shared_ptr<CFileData> >& rFiles = mModelData.GetFiles();
    std::vector<std::tr1::shared_ptr<CFileData> >::const_iterator it = rFiles.begin();
    for (; it != rFiles.end(); ++it) {
        resource = nw4r::g3d::ResFile((*it)->GetData()).GetResAnmVis(index);
        if (resource.IsValid()) {
            break;
        }
    }
    return resource;
}

__declspec(section ".text.model_middle") void CModel::ExecCallback_CALC_WORLD(nw4r::g3d::ScnObj::Timing timing, nw4r::g3d::ScnObj* pObject, u32 param, void* pInfo) {
    if (timing == nw4r::g3d::ScnObj::CALLBACK_TIMING_A) {
        UpdateAnimations();
    }
}

__declspec(section ".text.model_middle") void CModel::ExecCallbackA(nw4r::g3d::ChrAnmResult* pResult, nw4r::g3d::ResMdl mdl, nw4r::g3d::FuncObjCalcWorld* pFuncObj) {
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

__declspec(section ".text.model_middle") void CModel::ExecCallback_CALC_VIEW(nw4r::g3d::ScnObj::Timing timing, nw4r::g3d::ScnObj* pObject, u32 param, void* pInfo) {
    if (timing == nw4r::g3d::ScnObj::CALLBACK_TIMING_A && mAim.mpModel != nullptr && !mAnimationControlsAim) {
        if (!mWorldAimTarget) {
            mAim.AimAtMatrixOrigin(*static_cast<nw4r::math::MTX34*>(pInfo), static_cast<int>(ceil(static_cast<f32>(gFrameStep))));
        } else {
            mAim.AimAtPoint(mAimTarget, gFrameStep);
        }
    }
}

__declspec(section ".text.model_middle") bool CModel::LoadBounds(nw4r::math::AABB* pBounds, const nw4r::g3d::ResMdl& rMdl) {
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

__declspec(section ".text.model_middle") void CModel::ExecCallback_DRAW_XLU(nw4r::g3d::ScnObj::Timing timing, nw4r::g3d::ScnObj* pObject, u32 param, void* pInfo) {}

__declspec(section ".text.model_middle") void CModel::ExecCallback_DRAW_OPA(nw4r::g3d::ScnObj::Timing timing, nw4r::g3d::ScnObj* pObject, u32 param, void* pInfo) {}

__declspec(section ".text.model_middle") void CModel::ExecCallback_CALC_MAT(nw4r::g3d::ScnObj::Timing timing, nw4r::g3d::ScnObj* pObject, u32 param, void* pInfo) {}

void RequestModelCallbackEmission() {
    bool requestCallbacks = false;
    if (requestCallbacks) {
        nw4r::g3d::ICalcWorldCallback* pCallback = nullptr;
        pCallback->nw4r::g3d::ICalcWorldCallback::ExecCallbackC(nullptr, nw4r::g3d::ResMdl(), nullptr);
        pCallback->nw4r::g3d::ICalcWorldCallback::ExecCallbackB(nullptr, nw4r::g3d::ResMdl(), nullptr);
    }
}

template <> inline bool CModel::TPartialAnm<true>::IsReplaceable() {
    bool finished;
    bool replaceable = true;
    if (mMotions.size() != 0) {
        finished = false;
        if (mMotions.size() == 1) {
            if (mMotions.front()->mFlags & CMotionProgram::FLAG_FINISHED) {
                finished = true;
            }
        }
        if (!finished) {
            replaceable = false;
        }
    }
    return replaceable;
}

template <bool Main> __declspec(section ".text.model_middle") bool CModel::TPartialAnm<Main>::SetMotion(CModel* pModel, const std::tr1::shared_ptr<CMotionProgram>& rMotion) {
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

template <bool Main> __declspec(section ".text.model_middle") bool CModel::TPartialAnm<Main>::QueueMotion(CModel* pModel, const std::tr1::shared_ptr<CMotionProgram>& rMotion) {
    if (IsReplaceable()) {
        return SetMotion(pModel, rMotion);
    }
    if (static_cast<TPartialAnm*>(this)->mMotions.size() >= 2) {
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

template <bool Main> __declspec(section ".text.model_middle") bool CModel::TPartialAnm<Main>::AppendMotion(CModel* pModel, const std::tr1::shared_ptr<CMotionProgram>& rMotion) {
    if (IsReplaceable()) {
        return SetMotion(pModel, rMotion);
    }
    mMotions.push_back(rMotion);
    return true;
}

template <bool Main> __declspec(section ".text.model_middle") bool CModel::TPartialAnm<Main>::Update(CModel* pModel, f32 step) {
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
            typedef std::tr1::shared_ptr<CMotionProgram> MotionPointer;
            typedef std::__list_deleter<MotionPointer, std::allocator<MotionPointer> > MotionListBase;
            MotionListBase::node_base* pNode = mMotions.begin().base();
            pNode = reinterpret_cast<MotionListBase::node_base*>(reinterpret_cast<unsigned long>(pNode) | 0);
            MotionListBase::unlink_nodes(pNode, pNode);
            static_cast<MotionListBase::node*>(pNode)->data_.~MotionPointer();
            reinterpret_cast<MotionListBase&>(mMotions).node_alloc().deallocate(static_cast<MotionListBase::node*>(pNode), 1);
            --reinterpret_cast<MotionListBase&>(mMotions).size_ref();
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

template bool CModel::TPartialAnm<true>::SetMotion(CModel*, const std::tr1::shared_ptr<CMotionProgram>&);
#pragma opt_optimizecontrolflow off
template bool CModel::TPartialAnm<true>::QueueMotion(CModel*, const std::tr1::shared_ptr<CMotionProgram>&);
template bool CModel::TPartialAnm<true>::AppendMotion(CModel*, const std::tr1::shared_ptr<CMotionProgram>&);
template bool CModel::TPartialAnm<true>::Update(CModel*, f32);
#pragma opt_optimizecontrolflow on
template bool CModel::TPartialAnm<false>::SetMotion(CModel*, const std::tr1::shared_ptr<CMotionProgram>&);
template bool CModel::TPartialAnm<false>::QueueMotion(CModel*, const std::tr1::shared_ptr<CMotionProgram>&);
template bool CModel::TPartialAnm<false>::AppendMotion(CModel*, const std::tr1::shared_ptr<CMotionProgram>&);
template bool CModel::TPartialAnm<false>::Update(CModel*, f32);
template std::__vec_deleter<std::tr1::shared_ptr<nw4r::g3d::AnmObj>, std::allocator<std::tr1::shared_ptr<nw4r::g3d::AnmObj> > >::~__vec_deleter();

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

#pragma section code_type ".text.model_creators"
namespace std {
template <> __declspec(section ".text.model_compare") bool vector<tr1::shared_ptr<nw4r::g3d::AnmObj> >::iterator::operator!=(const iterator&) const;
template <> __declspec(section ".text.model_iterators")  vector<tr1::shared_ptr<nw4r::g3d::AnmObj> >::iterator vector<tr1::shared_ptr<nw4r::g3d::AnmObj> >::begin();
template <> __declspec(section ".text.model_iterators")  vector<tr1::shared_ptr<nw4r::g3d::AnmObj> >::iterator::iterator(const iterator&);
template <> __declspec(section ".text.model_iterators")  tr1::shared_ptr<nw4r::g3d::AnmObj>& vector<tr1::shared_ptr<nw4r::g3d::AnmObj> >::iterator::operator*() const;
template <> __declspec(section ".text.model_iterators")  vector<tr1::shared_ptr<nw4r::g3d::AnmObj> >::iterator& vector<tr1::shared_ptr<nw4r::g3d::AnmObj> >::iterator::operator++();
template <> __declspec(section ".text.model_iterators")  vector<tr1::shared_ptr<nw4r::g3d::AnmObj> >::iterator vector<tr1::shared_ptr<nw4r::g3d::AnmObj> >::end();

namespace tr1 {
template <> __declspec(section ".text.model_iterators") nw4r::g3d::AnmObjChrRes* shared_ptr<nw4r::g3d::AnmObjChrRes >::get() const;
template <> __declspec(section ".text.model_post_push") nw4r::g3d::AnmObjVisRes* shared_ptr<nw4r::g3d::AnmObjVisRes >::get() const;
template <> __declspec(section ".text.model_post_push") nw4r::g3d::AnmObjChrOverride* shared_ptr<nw4r::g3d::AnmObjChrOverride >::get() const;
template <> __declspec(section ".text.model_iterators") nw4r::g3d::AnmObj* shared_ptr<nw4r::g3d::AnmObj >::operator->() const;
template <> __declspec(section ".text.model_vis_assignment") nw4r::g3d::AnmObjVisRes* shared_ptr<nw4r::g3d::AnmObjVisRes >::operator->() const;
template <> __declspec(section ".text.model_middle") nw4r::g3d::AnmObjChrRootScale* shared_ptr<nw4r::g3d::AnmObjChrRootScale >::operator->() const;
namespace detail {
template <> __declspec(section ".text.model_chr_deleter") shared_ptr_deleter<nw4r::g3d::AnmObjChrRes, void (*)(nw4r::g3d::G3dObj*) >::~shared_ptr_deleter();
template <> __declspec(section ".text.model_deleters") shared_ptr_deleter<CModel::TPartialAnm<false>, std::detail::default_delete<CModel::TPartialAnm<false> > >::~shared_ptr_deleter();
template <> __declspec(section ".text.model_deleters") shared_ptr_deleter<CModel::PartialAnmManager, std::detail::default_delete<CModel::PartialAnmManager > >::~shared_ptr_deleter();
template <> __declspec(section ".text.model_deleters") shared_ptr_deleter<CModel::CMotionProgram, std::detail::default_delete<CModel::CMotionProgram > >::~shared_ptr_deleter();
template <> __declspec(section ".text.model_deleters") shared_ptr_deleter<nw4r::g3d::AnmObjVisRes, void (*)(nw4r::g3d::G3dObj*) >::~shared_ptr_deleter();
template <> __declspec(section ".text.model_deleters") shared_ptr_deleter<nw4r::g3d::AnmObjChrOverride, void (*)(nw4r::g3d::G3dObj*) >::~shared_ptr_deleter();
}
}
}
template <> __declspec(section ".text.model_partial") CModel::TPartialAnm<false>::~TPartialAnm();
