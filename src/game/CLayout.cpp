#include <game/CLayout.h>

#include <game/CFiber.h>
#include <game/MainRuntime.h>
#include <game/CFileData.h>
#include <nw4r/ut/ut_algorithm.h>
#include <nw4r/lyt/lyt_layout.h>
#include <nw4r/lyt/lyt_animation.h>
#include <nw4r/lyt/lyt_pane.h>
#include <nw4r/lyt/lyt_material.h>
#include <nw4r/lyt/lyt_resources.h>
#include <nw4r/lyt/lyt_textBox.h>
#include <nw4r/ut/ut_RuntimeTypeInfo.h>
#include <wchar.h>
#include <game/RenderMode.h>
#include <functional>
#include <nw4r/g3d/g3d_state.h>
#include <revolution/gx.h>
#include <revolution/gx/GXTransform.h>
#include <revolution/mtx.h>
#include <revolution/os.h>
#include <revolution/sc.h>

namespace {

struct Viewport {
    void Set(f32 left, f32 top, f32 w, f32 h) {
        x = left;
        y = top;
        width = w;
        height = h;
    }
    void SetSize(f32 w, f32 h) { Set(x, y, w, h); }

    f32 x, y, width, height;
};

struct ScissorBox {
    u32 x, y, width, height;
};

inline TPLPalette* FindImage(nw4r::lyt::MultiArcResourceAccessor& rAccessor, const char* pName) {
    TPLPalette* pPalette = static_cast<TPLPalette*>(rAccessor.GetResource('timg', pName, nullptr));
    if (pPalette == nullptr) {
        std::string imageName(pName);
        imageName.append(".tpl");
        pPalette = static_cast<TPLPalette*>(rAccessor.GetResource('timg', imageName.c_str(), nullptr));
    }
    return pPalette;
}

const char* const sTextPaneSuffixes[] = {"_nw", "_n", "_ne", "_w", "_e", "_sw", "_s", "_se", "_c", "", nullptr};

std::tr1::shared_ptr<std::tr1::function<void (const CLayout::CInitializeException&)> > sInitializeErrorHandler;
nw4r::math::_VEC2 sLocationAdjustScale = {-1.0f, -1.0f};
nw4r::math::_VEC2 sDefaultPixelScale = {-1.0f, -1.0f};

nw4r::math::VEC2 GetAspectScale() {
    if (SCGetAspectRatio() == 1) {
        return nw4r::math::VEC2(0.75f, 1.0f);
    }
    return nw4r::math::VEC2(1.0f, 1.0f);
}

nw4r::math::VEC2 GetLocationAdjustScale() {
    if (sLocationAdjustScale.x < 0.0f) {
        sLocationAdjustScale = GetAspectScale();
    }
    return nw4r::math::VEC2(sLocationAdjustScale.x, sLocationAdjustScale.y);
}

nw4r::math::VEC2 GetDefaultPixelScale() {
    if (sDefaultPixelScale.x < 0.0f) {
        const GXRenderModeObj* pRenderMode = nw4r::g3d::G3DState::GetRenderModeObj();
        u32 tvMode = pRenderMode->viTVmode;
        u8 aspect = SCGetAspectRatio();
        sDefaultPixelScale.x = GetPixelAspectRatio(tvMode, aspect);
        sDefaultPixelScale.y = 1.0f;
        if (tvMode - 4 <= 1u) {
            sDefaultPixelScale.x *= 0.83623695f;
            sDefaultPixelScale.y *= 0.83623695f;
        }
        if (SCGetAspectRatio() == 1) {
            sDefaultPixelScale.x *= GetAspectScale().x;
        }
    }
    return nw4r::math::VEC2(sDefaultPixelScale.x, sDefaultPixelScale.y);
}

std::string get_what(int error, const char* pArchive, const char* pLayoutName, const std::tr1::shared_ptr<CLayout::FontMap>& rFonts) {
    std::string what;
    what += "CLayout.cpp: in ";
    what += __FUNCTION__;
    return what;
}

std::string get_what() {
    std::string what;
    what += "CLayout.cpp: in ";
    what += __FUNCTION__;
    return what;
}

std::list<std::string> MakeArchiveList(const char* pArchive) {
    std::list<std::string> archives;
    archives.push_back(std::string(pArchive));
    return archives;
}

std::list<std::string> MakeArchiveList(const char* const* pArchives) {
    std::list<std::string> archives;
    for (const char* const* pName = pArchives; *pName != nullptr; ++pName) {
        archives.push_back(std::string(*pName));
    }
    return archives;
}

}

CLayout::CInitializeException::CInitializeException(int error, const char* pArchive, const char* pLayoutName, const std::tr1::shared_ptr<FontMap>& rFonts) :
    std::runtime_error(get_what(error, pArchive, pLayoutName, rFonts).c_str()),
    mError(error),
    mArchives(MakeArchiveList(pArchive)),
    mLayoutName(pLayoutName),
    mFonts(rFonts) {}

CLayout::CInitializeException::CInitializeException(int error, const char* const* pArchives, const char* pLayoutName, const std::tr1::shared_ptr<FontMap>& rFonts) :
    std::runtime_error(get_what().c_str()),
    mError(error),
    mArchives(MakeArchiveList(pArchives)),
    mLayoutName(pLayoutName),
    mFonts(rFonts) {}

f32 CLayout::CMotionProgram::GetFrame() const {
    if (mState < 1 || mState > 2) {
        if (mState == 0) {
            return 0.0f;
        }
        return mLastFrame;
    }
    return mpTarget->mFrame;
}

void CLayout::CMotionProgram::Wait() {
    while (mState != 3) {
        YieldFiber();
    }
}

void CLayout::CMotionProgram::WaitStart() {
    while (mState == 0) {
        YieldFiber();
    }
}

CLayout::CLayout() :
    mFlags(1),
    mDepthCompare(GX_NEVER) {
    mDrawInfo.SetLocationAdjust(true);
    SetViewScale(GetLocationAdjustScale());
    mPixelScale = GetDefaultPixelScale();
    UpdateViewRect();
}

void CLayout::UpdateViewRect() {
    const GXRenderModeObj* pRenderMode = nw4r::g3d::G3DState::GetRenderModeObj();
    mViewRect.right = 0.5f * (pRenderMode->viWidth * mPixelScale.x);
    mViewRect.top = 0.5f * (pRenderMode->viHeight * mPixelScale.y);
    mViewRect.left = -mViewRect.right;
    mViewRect.bottom = -mViewRect.top;
}

#pragma dont_inline on
CLayout::~CLayout() {
    for (PaneProgramMap::iterator it = mPanePrograms.begin(); it != mPanePrograms.end(); ++it) {
        for (MotionList::iterator motion = (*it).second->mMotions.begin(); motion != (*it).second->mMotions.end(); ++motion) {
            (*motion)->Finish();
        }
    }
    DetachAttachedLayouts();
    Detach();
}
#pragma dont_inline reset

void SetLayoutInitializeExceptionHandler(const std::tr1::function<void(const CLayout::CInitializeException&)>& rHandler) {
    if (rHandler) {
        sInitializeErrorHandler.reset(new std::tr1::function<void(const CLayout::CInitializeException&)>(rHandler));
    } else {
        sInitializeErrorHandler.reset();
    }
}

void CLayout::HandleInitializeError(const CInitializeException& rError) {
    if (sInitializeErrorHandler) {
        (*sInitializeErrorHandler)(rError);
        OSPanic("CLayout.cpp", 337, rError.what());
    } else {
        throw rError;
    }
}

void CLayout::Load(const char* pArchive, const char* pLayoutName, const std::tr1::shared_ptr<FontMap>& rFonts, int background) {
    int error = LoadArchive(pArchive, pLayoutName, rFonts, background);
    if (error != 0) {
        CInitializeException exception(error, pArchive, pLayoutName, rFonts);
        HandleInitializeError(exception);
    }
}

void CLayout::Load(const char* const* pArchives, const char* pLayoutName, const std::tr1::shared_ptr<FontMap>& rFonts, int background) {
    int error = LoadArchives(pArchives, pLayoutName, rFonts, background);
    if (error != 0) {
        CInitializeException exception(error, pArchives, pLayoutName, rFonts);
        HandleInitializeError(exception);
    }
}

int CLayout::LoadArchive(const char* pArchive, const char* pLayoutName, const std::tr1::shared_ptr<FontMap>& rFonts, int background) {
    std::tr1::shared_ptr<CFileData> file = LoadResourceFile(pArchive, 0, background);
    if (!file || !file->IsLoaded()) {
        return 1;
    }
    mFiles.push_back(file);
    std::tr1::shared_ptr<nw4r::lyt::ArcResourceLink> link(new nw4r::lyt::ArcResourceLink);
    link->Set(file->GetData(), "");
    mArchives.reserve(1);
    mArchives.push_back(link);
    mAccessor.Attach(link.get());
    return LoadLayout(pLayoutName, rFonts);
}

int CLayout::LoadFromMemory(void* pArchive, const char* pLayoutName, const std::tr1::shared_ptr<FontMap>& rFonts) throw() {
    std::tr1::shared_ptr<nw4r::lyt::ArcResourceLink> link(new nw4r::lyt::ArcResourceLink);
    link->Set(pArchive, "");
    mArchives.reserve(1);
    mArchives.push_back(link);
    mAccessor.Attach(link.get());
    return LoadLayout(pLayoutName, rFonts);
}

#pragma dont_inline on
int CLayout::LoadArchives(const char* const* pArchives, const char* pLayoutName, const std::tr1::shared_ptr<FontMap>& rFonts, int background) {
    std::list<std::tr1::shared_ptr<CFileData> > files;
    for (; *pArchives != nullptr; ++pArchives) {
        std::tr1::shared_ptr<CFileData> file = LoadResourceFile(*pArchives, 0, background);
        if (!file || !file->IsLoaded()) {
            return 1;
        }
        AppendLoadedFile(files, file);
    }
    if (files.size() == 0) {
        return 2;
    }
    mFiles.swap(files);
    mArchives.reserve(mFiles.size());
    for (std::list<std::tr1::shared_ptr<CFileData> >::const_iterator it = mFiles.begin(); it != mFiles.end(); ++it) {
        std::tr1::shared_ptr<nw4r::lyt::ArcResourceLink> link(new nw4r::lyt::ArcResourceLink);
        link->Set((*it)->GetData(), "");
        mAccessor.Attach(link.get());
        mArchives.push_back(link);
    }
    return LoadLayout(pLayoutName, rFonts);
}

int CLayout::LoadLayout(const char* pLayoutName, const std::tr1::shared_ptr<FontMap>& rFonts) throw() {
    std::vector<std::tr1::shared_ptr<nw4r::lyt::FontRefLink> > fontLinks;
    if (rFonts) {
        mFonts = rFonts;
        fontLinks.reserve(mFonts->size());
        for (FontMap::iterator it = mFonts->begin(); it != mFonts->end(); ++it) {
            std::tr1::shared_ptr<nw4r::lyt::FontRefLink> link(new nw4r::lyt::FontRefLink);
            link->Set((*it).first.c_str(), (*it).second.get());
            fontLinks.push_back(link);
            mAccessor.RegistFont(link.get());
        }
    }
    mLayout.reset(new nw4r::lyt::Layout);
    void* pResource = mAccessor.GetResource('blyt', (std::string(pLayoutName) + ".brlyt").c_str(), nullptr);
    if (!mLayout->Build(pResource, &mAccessor)) {
        return 3;
    }
    nw4r::ut::Rect layoutRect = mLayout->GetLayoutRect();
    mDrawInfo.SetViewRect(layoutRect);
    if (rFonts) {
        for (std::vector<std::tr1::shared_ptr<nw4r::lyt::FontRefLink> >::iterator it = fontLinks.begin(); it != fontLinks.end(); ++it) {
            mAccessor.UnregistFont((*it).get());
        }
    }
    mName = std::string(pLayoutName) + "_";
    SetFlags(FLAG_DIRTY | FLAG_LOADED);
    SetVisible(HasFlags(FLAG_VISIBLE));
    return 0;
}
#pragma dont_inline reset

void CLayout::SetVisible(bool visible) {
    if (mFlags & FLAG_LOADED) {
        SetVisible(mLayout->GetRootPane(), visible);
    } else if (visible) {
        mFlags |= FLAG_VISIBLE;
    } else {
        mFlags &= ~FLAG_VISIBLE;
    }
}

nw4r::lyt::AnimTransform* CLayout::FindAnimation(const char* pName) {
    typedef std::map<std::string, nw4r::lyt::AnimTransform*> AnimationMap;
    AnimationMap::iterator it = mAnimations.find(std::string(pName));
    if (it == mAnimations.end()) {
        void* pResource = mAccessor.GetResource('anim', pName, nullptr);
        if (pResource != nullptr) {
            std::string animName(pName);
            nw4r::lyt::AnimTransform* pAnim = mLayout->CreateAnimTransform(pResource, &mAccessor);
            it = mAnimations.insert(AnimationMap::value_type(animName, pAnim)).first;
        } else {
            std::string path = mName + pName + ".brlan";
            pResource = mAccessor.GetResource('anim', path.c_str(), nullptr);
            if (pResource != nullptr) {
                std::string animName(pName);
                nw4r::lyt::AnimTransform* pAnim = mLayout->CreateAnimTransform(pResource, &mAccessor);
                it = mAnimations.insert(AnimationMap::value_type(animName, pAnim)).first;
            } else {
                return nullptr;
            }
        }
    }
    return (*it).second;
}

CLayout::PaneMP* CLayout::FindPaneMP(nw4r::lyt::Pane* pPane) {
    if (pPane == nullptr) {
        pPane = mLayout->GetRootPane();
    }
    std::tr1::shared_ptr<PaneMP>& rEntry = mPanePrograms[pPane];
    if (!rEntry) {
        rEntry.reset(new PaneMP);
        rEntry->mpPane = pPane;
        rEntry->mFrame = 0.0f;
        rEntry->mRunning = false;
        PaneProgramMap::iterator it;
        for (it = mPanePrograms.begin(); it != mPanePrograms.end(); ++it) {
            nw4r::lyt::Pane* pKey = (*it).first;
            for (nw4r::lyt::Pane* pAncestor = pKey->GetParent(); pAncestor != nullptr; pAncestor = pAncestor->GetParent()) {
                PaneProgramMap::iterator found = mPanePrograms.find(pAncestor);
                if (found != mPanePrograms.end()) {
                    (*found).second->mDescendants.insert(pKey);
                }
            }
        }
        for (it = mPanePrograms.begin(); it != mPanePrograms.end(); ++it) {
            PaneMP* pOther = (*it).second.get();
            std::set<nw4r::lyt::Pane*>::iterator descendant = pOther->mDescendants.begin();
            while (descendant != pOther->mDescendants.end()) {
                bool nested = false;
                for (nw4r::lyt::Pane* pAncestor = (*descendant)->GetParent(); pAncestor != nullptr; pAncestor = pAncestor->GetParent()) {
                    if (pOther->mDescendants.find(pAncestor) != pOther->mDescendants.end()) {
                        nested = true;
                        break;
                    }
                }
                if (nested) {
                    pOther->mDescendants.erase(descendant);
                    descendant = pOther->mDescendants.begin();
                } else {
                    ++descendant;
                }
            }
        }
    }
    return rEntry.get();
}

std::tr1::shared_ptr<CLayout::CMotionProgram> CLayout::BuildMotion(const char* pName, nw4r::lyt::Pane* pPane, f32 startFrame, f32 rate) {
    if (pName == nullptr) {
        return std::tr1::shared_ptr<CMotionProgram>();
    }
    nw4r::lyt::AnimTransform* pAnim = FindAnimation(pName);
    if (pAnim != nullptr) {
        std::tr1::shared_ptr<CMotionProgram> pProgram(new CMotionProgram);
        pProgram->mFrame = startFrame;
        pProgram->mRate = rate;
        pProgram->mpAnimTransform = pAnim;
        pProgram->mpTarget = FindPaneMP(pPane);
        pProgram->mLastFrame = pProgram->mpAnimTransform->GetFrameSize();
        if (!pProgram->mpAnimTransform->IsLoopData()) {
            pProgram->mLastFrame--;
        }
        pProgram->mState = 0;
        return pProgram;
    }
    return std::tr1::shared_ptr<CMotionProgram>();
}

inline void CLayout::RebindAnimations() {
    PaneProgramMap::iterator it;
    std::set<nw4r::lyt::Pane*>::iterator child;
    for (it = mPanePrograms.begin(); it != mPanePrograms.end(); ++it) {
        if (!(*it).second->mMotions.empty()) {
            MotionList::iterator front = (*it).second->mMotions.begin();
            (*it).second->mpPane->UnbindAnimation((*front)->mpAnimTransform, true);
            (*it).second->mpPane->BindAnimation((*front)->mpAnimTransform, true, false);
            for (child = (*it).second->mDescendants.begin(); child != (*it).second->mDescendants.end(); ++child) {
                (*child)->UnbindAnimation((*front)->mpAnimTransform, true);
            }
        }
    }
}

std::tr1::shared_ptr<CLayout::CMotionProgram> CLayout::CreateMotion(const char* pName, nw4r::lyt::Pane* pPane, f32 startFrame, f32 rate) {
    const std::tr1::shared_ptr<CMotionProgram>& rProgram = BuildMotion(pName, pPane, startFrame, rate);
    if (!rProgram) {
        return rProgram;
    }
    return ReplaceMotions(rProgram);
}

std::tr1::shared_ptr<CLayout::CMotionProgram> CLayout::ReplaceMotions(const std::tr1::shared_ptr<CMotionProgram>& rProgram) {
    PaneMP* pPaneMP = rProgram->mpTarget;
    if (!pPaneMP->mMotions.empty()) {
        pPaneMP->mpPane->UnbindAnimation(pPaneMP->mMotions.front()->mpAnimTransform, true);
        for (MotionList::iterator motion = pPaneMP->mMotions.begin(); motion != pPaneMP->mMotions.end(); ++motion) {
            (*motion)->Finish();
        }
        pPaneMP->mMotions.clear();
    }
    pPaneMP->mMotions.push_back(rProgram);
    rProgram->mState = 1;
    SetFlags(4);
    pPaneMP->mRunning = true;
    RebindAnimations();
    if (0.0 <= rProgram->mFrame) {
        pPaneMP->mFrame = rProgram->mFrame;
    } else {
        AdvancePaneMotions(pPaneMP, 1.0f);
    }
    return rProgram;
}

std::tr1::shared_ptr<CLayout::CMotionProgram> CLayout::CreateMotion(const char* pName, const char* pPaneName, f32 startFrame, f32 rate) {
    nw4r::lyt::Pane* pPane = FindPane(pPaneName);
    if (pPane != nullptr) {
        return CreateMotion(pName, pPane, startFrame, rate);
    }
    return std::tr1::shared_ptr<CMotionProgram>();
}

std::tr1::shared_ptr<CLayout::CMotionProgram> CLayout::QueueMotion(const char* pName, nw4r::lyt::Pane* pPane, f32 startFrame, f32 rate) {
    const std::tr1::shared_ptr<CMotionProgram>& rProgram = BuildMotion(pName, pPane, startFrame, rate);
    if (!rProgram) {
        return rProgram;
    }
    return ReplaceQueuedMotions(rProgram);
}

std::tr1::shared_ptr<CLayout::CMotionProgram> CLayout::ReplaceQueuedMotions(const std::tr1::shared_ptr<CMotionProgram>& rProgram) {
    PaneMP* pPaneMP = rProgram->mpTarget;
    if (pPaneMP->mMotions.empty()) {
        return ReplaceMotions(rProgram);
    }
    MotionList::iterator first = ++pPaneMP->mMotions.begin();
    for (MotionList::iterator it = first; it != pPaneMP->mMotions.end(); ++it) {
        (*it)->Finish();
    }
    pPaneMP->mMotions.erase(first, pPaneMP->mMotions.end());
    pPaneMP->mMotions.push_back(rProgram);
    return rProgram;
}

std::tr1::shared_ptr<CLayout::CMotionProgram> CLayout::QueueMotion(const char* pName, const char* pPaneName, f32 startFrame, f32 rate) {
    nw4r::lyt::Pane* pPane = FindPane(pPaneName);
    if (pPane != nullptr) {
        return QueueMotion(pName, pPane, startFrame, rate);
    }
    return std::tr1::shared_ptr<CMotionProgram>();
}

std::tr1::shared_ptr<CLayout::CMotionProgram> CLayout::AppendMotion(const char* pName, nw4r::lyt::Pane* pPane, f32 startFrame, f32 rate) {
    const std::tr1::shared_ptr<CMotionProgram>& rProgram = BuildMotion(pName, pPane, startFrame, rate);
    if (!rProgram) {
        return rProgram;
    }
    return AppendProgram(rProgram);
}

std::tr1::shared_ptr<CLayout::CMotionProgram> CLayout::AppendProgram(const std::tr1::shared_ptr<CMotionProgram>& rProgram) {
    PaneMP* pPaneMP = rProgram->mpTarget;
    if (pPaneMP->mMotions.empty()) {
        return ReplaceMotions(rProgram);
    }
    if (pPaneMP->mMotions.size() == 1) {
        return ReplaceQueuedMotions(rProgram);
    }
    pPaneMP->mMotions.push_back(rProgram);
    return rProgram;
}

std::tr1::shared_ptr<CLayout::CMotionProgram> CLayout::AppendMotion(const char* pName, const char* pPaneName, f32 startFrame, f32 rate) {
    nw4r::lyt::Pane* pPane = FindPane(pPaneName);
    if (pPane != nullptr) {
        return AppendMotion(pName, pPane, startFrame, rate);
    }
    return std::tr1::shared_ptr<CMotionProgram>();
}

void CLayout::StopMotion(nw4r::lyt::Pane* pPane) {
    if (pPane == nullptr) {
        pPane = mLayout->GetRootPane();
    }
    PaneProgramMap::iterator it = mPanePrograms.find(pPane);
    if (it != mPanePrograms.end()) {
        if (!(*it).second->mMotions.empty()) {
            MotionList::iterator front = (*it).second->mMotions.begin();
            (*front)->mpAnimTransform->SetFrame((*it).second->mFrame);
            (*it).second->mpPane->Animate(0);
            (*it).second->mpPane->UnbindAnimation((*front)->mpAnimTransform, true);
            for (MotionList::iterator motion = (*it).second->mMotions.begin(); motion != (*it).second->mMotions.end(); ++motion) {
                (*motion)->Finish();
            }
            (*it).second->mMotions.clear();
            (*it).second->mRunning = false;
            UpdateAttachedLayouts();
        }
    }
}

u16 CLayout::GetMotionLength(const char* pName) {
    if (pName != nullptr) {
        nw4r::lyt::AnimTransform* pAnim = FindAnimation(pName);
        if (pAnim != nullptr) {
            return pAnim->GetFrameSize();
        }
    }
    return 0;
}

bool CLayout::IsVisible() const {
    if (HasFlags(FLAG_LOADED)) {
        return mLayout->GetRootPane()->IsVisible();
    }
    return HasFlags(FLAG_VISIBLE);
}

void CLayout::SetViewScale(const nw4r::math::VEC2& rScale) {
    mDrawInfo.SetLocationAdjustScale(rScale);
    UpdateViewRect();
}

nw4r::math::_VEC2 CLayout::GetViewScale() const {
    return mDrawInfo.GetLocationAdjustScale();
}

void CLayout::SetVisible(nw4r::lyt::Pane* pPane, bool visible) {
    if (pPane == nullptr) {
        pPane = mLayout->GetRootPane();
    }
    pPane->SetVisible(visible);
    if (mAttach && mAttach->mpPane != nullptr && pPane == mLayout->GetRootPane()) {
        mAttach->mpParent->SetVisible(mAttach->mpPane, visible);
    } else if (visible) {
        UpdateAttachedLayouts();
    }
}

void CLayout::AdvancePaneMotions(PaneMP* pPaneMP, f32 frames) {
    for (;;) {
        if (pPaneMP->mMotions.empty()) {
            pPaneMP->mFrame += frames;
            return;
        }
        MotionList::iterator front = pPaneMP->mMotions.begin();
        f32 lastFrame = (*front)->mLastFrame;
        pPaneMP->mFrame += frames * (*front)->mRate;
        bool running = pPaneMP->mFrame < lastFrame;
        if (running) {
            pPaneMP->mRunning = true;
            return;
        }
        pPaneMP->mFrame -= lastFrame;
        if (pPaneMP->mMotions.size() >= 2) {
            frames = pPaneMP->mFrame / (*front)->mRate;
            pPaneMP->mpPane->UnbindAnimation((*front)->mpAnimTransform, true);
            (*front)->Finish();
            pPaneMP->mMotions.pop_front();
            f32 startFrame = (*pPaneMP->mMotions.begin())->mFrame;
            pPaneMP->mFrame = std::max(startFrame, 0.0f);
            (*pPaneMP->mMotions.begin())->mState = 1;
            RebindAnimations();
        } else if ((*front)->mpAnimTransform->IsLoopData()) {
            (*front)->mState = 2;
            frames = 0.0f;
        } else {
            pPaneMP->mFrame /= (*front)->mRate;
            (*front)->mpAnimTransform->SetFrame((*front)->mpAnimTransform->GetFrameSize());
            pPaneMP->mpPane->Animate(0);
            pPaneMP->mpPane->UnbindAnimation((*front)->mpAnimTransform, true);
            (*front)->Finish();
            pPaneMP->mMotions.pop_front();
            pPaneMP->mRunning = false;
            UpdateAttachedLayouts();
            return;
        }
    }
}

#pragma auto_inline off
void CLayout::ApplyMotionFrames() {
    PaneProgramMap::iterator it;
    if (HasFlags(FLAG_LOADED | FLAG_ANIMATED) && !mPanePrograms.empty()) {
        for (it = mPanePrograms.begin(); it != mPanePrograms.end(); ++it) {
            if ((*it).second->mRunning && !(*it).second->mMotions.empty()) {
                (*(*it).second->mMotions.begin())->mpAnimTransform->SetFrame((*it).second->mFrame);
                (*it).second->mpPane->Animate(0);
                (*it).second->mRunning = false;
            }
        }
        mFlags &= ~FLAG_ANIMATED;
        UpdateAttachedLayouts();
    }
}
#pragma auto_inline reset

nw4r::lyt::Pane* CLayout::FindPane(const char* pName) {
    return mLayout->GetRootPane()->FindPaneByName(pName, true);
}

void CLayout::UpdateAttachedLayouts() {
    if (!(mFlags & FLAG_DIRTY)) {
        mFlags |= FLAG_DIRTY;
        if (mAttach && !mAttach->mChildren.empty()) {
            for (std::set<CLayout*>::const_iterator it = mAttach->mChildren.begin(); it != mAttach->mChildren.end(); ++it) {
                (*it)->UpdateAttachedLayouts();
            }
        }
    }
}

void CLayout::CalculateMtx(bool recursive) {
    if (HasFlags(FLAG_LOADED)) {
        ApplyMotionFrames();
        if (recursive) {
            UpdateAttachedLayouts();
        }
        if (HasFlags(FLAG_DIRTY)) {
            if (mAttach && mAttach->mpParent != nullptr) {
                CalculateAttachedMtx();
            } else {
                mLayout->CalculateMtx(mDrawInfo);
            }
            mFlags &= ~FLAG_DIRTY;
        }
    }
}

inline f32 CLayout::GetLayoutAlpha() {
    return GetPaneAlpha(mLayout->GetRootPane());
}

void CLayout::CalculateAttachedMtx() {
    mAttach->mpParent->CalculateMtx(false);
    nw4r::lyt::Pane* pAttachPane = mAttach->mpPane;
    nw4r::lyt::Pane* pRoot = mLayout->GetRootPane();
    if (!pAttachPane->IsVisible()) {
        pRoot->SetVisible(false);
        return;
    }
    f32 alpha = GetLayoutAlpha();
    if (alpha == 0.0f) {
        pRoot->SetVisible(false);
        return;
    }
    pRoot->SetVisible(true);
    pRoot->SetGlobalMtx(mAttach->mpPane->GetGlobalMtx());
    pRoot->SetAlpha(mAttach->mpPane->GetAlpha());
    pRoot->SetGlbAlpha(mAttach->mpPane->GetGlbAlpha());
    f32 savedAlpha = mDrawInfo.GetGlobalAlpha();
    bool savedInfluenced = mDrawInfo.IsInfluencedAlpha();
    if (alpha < 1.0f) {
        mDrawInfo.SetGlobalAlpha(alpha);
        mDrawInfo.SetInfluencedAlpha(true);
    }
    for (nw4r::lyt::Pane::PaneList::Iterator it = pRoot->GetChildList()->GetBeginIter(); it != pRoot->GetChildList()->GetEndIter(); ++it) {
        it->CalculateMtx(mDrawInfo);
    }
    mDrawInfo.SetInfluencedAlpha(savedInfluenced);
    mDrawInfo.SetGlobalAlpha(savedAlpha);
}

f32 CLayout::GetPaneAlpha(nw4r::lyt::Pane* pPane) {
    if (pPane == mLayout->GetRootPane()) {
        nw4r::lyt::Pane* pAttachPane = mAttach->mpPane;
        if (pAttachPane != nullptr) {
            return mAttach->mpParent->GetPaneAlpha(pAttachPane);
        }
    }
    if (pPane != nullptr) {
        if (!pPane->IsVisible()) {
            return 0.0f;
        }
        f32 alpha = GetPaneAlpha(pPane->GetParent());
        u8 paneAlpha = pPane->GetAlpha();
        if (paneAlpha != 255) {
            return (1.0f / 255.0f) * (alpha * paneAlpha);
        }
        return alpha;
    }
    return 1.0f;
}

bool CLayout::Attach(nw4r::lyt::Pane* pPane, CLayout* pChild) {
    if (pChild->mAttach) {
        if (pChild->mAttach->mpParent != nullptr) {
            return false;
        }
    } else {
        pChild->mAttach.reset(new CAttach);
    }
    pChild->mAttach->mpParent = this;
    pChild->mAttach->mpPane = pPane;
    if (!mAttach) {
        mAttach.reset(new CAttach);
    }
    std::set<CLayout*>::iterator it = mAttach->mChildren.insert(pChild).first;
    return true;
}

bool CLayout::AttachLayout(const char* pPaneName, CLayout* pChild) {
    nw4r::lyt::Pane* pPane = FindPane(pPaneName);
    if (pPane != nullptr) {
        return Attach(pPane, pChild);
    }
    return false;
}

bool CLayout::Detach() {
    if (!mAttach || mAttach->mpParent == nullptr) {
        return false;
    }
    CLayout* pParent = mAttach->mpParent;
    mAttach->mpParent = nullptr;
    mAttach->mpPane = nullptr;
    if (mAttach->mChildren.empty()) {
        mAttach.reset();
    }
    pParent->mAttach->mChildren.erase(this);
    if (pParent->mAttach->mChildren.empty() && pParent->mAttach->mpParent == nullptr) {
        pParent->mAttach.reset();
    }
    return true;
}

void CLayout::DetachAttachedLayouts() {
    if (mAttach) {
        std::set<CLayout*> children(mAttach->mChildren);
        std::set<CLayout*>::const_iterator it;
        for (it = children.begin(); it != children.end(); ++it) {
            (*it)->Detach();
        }
    }
}

void CLayout::SetPaneVisible(const char* pName, bool visible) {
    nw4r::lyt::Pane* pPane = FindPane(pName);
    if (pPane != nullptr) {
        SetVisible(pPane, visible);
    }
}

nw4r::lyt::TextBox* CLayout::SetText(const char* pName, const wchar_t* pText) {
    nw4r::lyt::TextBox* pResult = nullptr;
    int length = wcslen(pText);
    for (const char* const* pSuffix = sTextPaneSuffixes; *pSuffix != nullptr; ++pSuffix) {
        nw4r::lyt::Pane* pPane = FindPane((std::string(pName) + *pSuffix).c_str());
        if (pPane != nullptr) {
            nw4r::lyt::TextBox* pTextBox = nw4r::ut::DynamicCast<nw4r::lyt::TextBox*>(pPane);
            if (pTextBox != nullptr) {
                pTextBox->AllocStringBuffer(length);
                pTextBox->SetString(pText, 0);
                pResult = pTextBox;
            }
        }
    }
    return pResult;
}

bool CLayout::SetTexture(nw4r::lyt::Pane* pPane, const nw4r::lyt::TexMap& rTexture, int index) {
    if (pPane == nullptr) {
        return false;
    }
    nw4r::lyt::Material* pMaterial = pPane->GetMaterial();
    if (index < 0) {
        const nw4r::lyt::res::ExtUserData* pUserData = pPane->FindExtUserDataByName("REPLACE_TEXTURE");
        if (pUserData != nullptr && static_cast<int>(pUserData->type) == 1) {
            bool replaced = false;
            for (u16 i = 0; i < pUserData->dataNum; i++) {
                int texMapIndex = reinterpret_cast<const s32*>(reinterpret_cast<const u8*>(pUserData) + pUserData->dataOffset)[i];
                if (texMapIndex < pMaterial->GetTexCoordGenNum()) {
                    *pMaterial->GetTexturePtr(texMapIndex) = rTexture;
                    replaced = true;
                }
            }
            return replaced;
        }
        index = 0;
    }
    if (index < pMaterial->GetTexCoordGenNum()) {
        *pMaterial->GetTexturePtr(index) = rTexture;
        return true;
    }
    return false;
}

bool CLayout::SetTexture(const char* pName, const nw4r::lyt::TexMap& rTexture, int index) {
    nw4r::lyt::Pane* pPane = FindPane(pName);
    if (pPane != nullptr) {
        return SetTexture(pPane, rTexture, index);
    }
    return false;
}

bool CLayout::SetImage(const char* pPaneName, const char* pImageName, int index) {
    nw4r::lyt::Pane* pPane = FindPane(pPaneName);
    if (pPane != nullptr) {
        TPLPalette* pPalette = FindImage(mAccessor, pImageName);
        if (pPalette == nullptr) {
            return false;
        }
        nw4r::lyt::TexMap texMap(pPalette, 0);
        texMap.SetBiasClampEnable(false);
        texMap.SetAnisotropy(GX_ANISO_1);
        return SetTexture(pPane, texMap, index);
    }
    return false;
}

nw4r::math::_VEC2 CLayout::NormalizeViewPosition(const nw4r::math::VEC2& rPosition) {
    nw4r::math::_VEC2 result;
    result.y = -2.0f * ((rPosition.y - mViewRect.bottom) / (mViewRect.top - mViewRect.bottom) - 0.5f);
    result.x = 2.0f * ((rPosition.x - mViewRect.left) / (mViewRect.right - mViewRect.left) - 0.5f);
    return result;
}

void CLayout::Calc() {
    PaneProgramMap::iterator it;
    if (!mPanePrograms.empty()) {
        for (it = mPanePrograms.begin(); it != mPanePrograms.end(); ++it) {
            AdvancePaneMotions((*it).second.get(), 1.0f);
        }
    }
}

inline void CLayout::SetupScreen(Mtx44 projection) {
    const GXRenderModeObj* pRenderMode = nw4r::g3d::G3DState::GetRenderModeObj();
    nw4r::ut::Rect rect = mViewRect;
    C_MTXOrtho(projection, rect.top, rect.bottom, rect.left, rect.right, 0.0f, 500.0f);
    GXSetProjection(projection, GX_ORTHOGRAPHIC);
    Viewport viewport = {0.0f, 0.0f, 0.0f, 0.0f};
    f32 width = pRenderMode->fbWidth;
    f32 height = pRenderMode->efbHeight;
    viewport.SetSize(width, height);
    ScissorBox scissor = {0, 0, pRenderMode->fbWidth, pRenderMode->efbHeight};
    GXSetViewport(viewport.x, viewport.y, viewport.width, viewport.height, 0.0f, 1.0f);
    GXSetScissor(scissor.x, scissor.y, scissor.width, scissor.height);
}

void CLayout::Draw() {
    if (HasFlags(FLAG_LOADED)) {
        CalculateMtx(false);
        if (IsVisible()) {
            Mtx44 projection;
            SetupScreen(projection);
            mDrawInfo.SetViewMtx(nw4r::math::MTX34(1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, -250.0f));
            GXSetZMode(mDepthCompare != GX_NEVER, mDepthCompare, mDepthCompare != GX_NEVER);
            GXSetCullMode(GX_CULL_NONE);
            mLayout->Draw(mDrawInfo);
            nw4r::g3d::G3DState::Invalidate(0x7FF);
        }
    }
}

void CLayout::Render(const nw4r::math::VEC2& rSize) {
    if (HasFlags(FLAG_LOADED)) {
        CalculateMtx(false);
        Mtx44 projection;
        C_MTXOrtho(projection, rSize.y / 2.0f, -rSize.y / 2.0f, -rSize.x / 2.0f, rSize.x / 2.0f, 0.0f, 500.0f);
        GXSetProjection(projection, GX_ORTHOGRAPHIC);
        GXSetViewport(0.0f, 0.0f, rSize.x, rSize.y, 0.0f, 1.0f);
        GXSetScissor(0, 0, static_cast<u32>(rSize.x), static_cast<u32>(rSize.y));
        nw4r::math::MTX34 viewMtx;
        nw4r::math::MTX34Identity(&viewMtx);
        mDrawInfo.SetViewMtx(viewMtx);
        GXSetZMode(mDepthCompare != GX_NEVER, mDepthCompare, mDepthCompare != GX_NEVER);
        GXSetCullMode(GX_CULL_NONE);
        mLayout->Draw(mDrawInfo);
        nw4r::g3d::G3DState::Invalidate(0x7FF);
    }
}
