#include <game/CLayout.h>

#include <game/CFiber.h>
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
#include <revolution/os.h>
#include <revolution/sc.h>

namespace {

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
        f32 ratio = GetPixelAspectRatio(tvMode, SCGetAspectRatio());
        sDefaultPixelScale.x = ratio;
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
    DetachChildren();
}
#pragma dont_inline reset

void CLayout::HandleInitializeError(const CInitializeException& rError) {
    if (sInitializeErrorHandler) {
        (*sInitializeErrorHandler)(rError);
        OSPanic("CLayout.cpp", 337, rError.what());
    } else {
        throw rError;
    }
}

void CLayout::Load(const char* pArchive, const char* pLayoutName, const std::tr1::shared_ptr<FontMap>& rFonts, bool background) {
    int error = LoadArchive(pArchive, pLayoutName, rFonts, background);
    if (error != 0) {
        CInitializeException exception(error, pArchive, pLayoutName, rFonts);
        HandleInitializeError(exception);
    }
}

void CLayout::Load(const char* const* pArchives, const char* pLayoutName, const std::tr1::shared_ptr<FontMap>& rFonts, bool background) {
    int error = LoadArchives(pArchives, pLayoutName, rFonts, background);
    if (error != 0) {
        CInitializeException exception(error, pArchives, pLayoutName, rFonts);
        HandleInitializeError(exception);
    }
}

int CLayout::LoadArchive(const char* pArchive, const char* pLayoutName, const std::tr1::shared_ptr<FontMap>& rFonts, bool background) {
    std::tr1::shared_ptr<CFileData> file = LoadResourceFile(pArchive, 0, background);
    if (!file || !file->IsLoaded()) {
        return 1;
    }
    mFiles.push_back(file);
    std::tr1::shared_ptr<nw4r::lyt::ArcResourceLink> link(new nw4r::lyt::ArcResourceLink);
    link->Set(file->GetData(), "");
    mArchives.push_back(link);
    mAccessor.Attach(link.get());
    return LoadLayout(pLayoutName, rFonts);
}

#pragma dont_inline on
int CLayout::LoadArchives(const char* const* pArchives, const char* pLayoutName, const std::tr1::shared_ptr<FontMap>& rFonts, bool background) {
    std::list<std::tr1::shared_ptr<CFileData> > files;
    for (; *pArchives != nullptr; ++pArchives) {
        std::tr1::shared_ptr<CFileData> file = LoadResourceFile(*pArchives, 0, background);
        if (!file || !file->IsLoaded()) {
            return 1;
        }
        files.push_back(file);
    }
    if (files.size() == 0) {
        return 2;
    }
    mFiles = files;
    mArchives.reserve(mFiles.size());
    for (std::list<std::tr1::shared_ptr<CFileData> >::iterator it = mFiles.begin(); it != mFiles.end(); ++it) {
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
    void* pResource;
    {
        std::string baseName(pLayoutName);
        std::string layoutPath = baseName + ".brlyt";
        pResource = mAccessor.GetResource('blyt', layoutPath.c_str(), nullptr);
    }
    if (!mLayout->Build(pResource, &mAccessor)) {
        return 3;
    }
    mDrawInfo.SetViewRect(mLayout->GetLayoutRect());
    if (rFonts) {
        std::vector<std::tr1::shared_ptr<nw4r::lyt::FontRefLink> >::iterator it = fontLinks.end();
        while (it != fontLinks.begin()) {
            --it;
            mAccessor.UnregistFont((*it).get());
        }
    }
    {
        std::string baseName(pLayoutName);
        mName = baseName + "_";
    }
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

nw4r::lyt::Pane* CLayout::FindPane(const char* pName) {
    return mLayout->GetRootPane()->FindPaneByName(pName, true);
}

void CLayout::UpdateAttachedLayouts() {
    if (!(mFlags & FLAG_DIRTY)) {
        mFlags |= FLAG_DIRTY;
        if (mAttach && !mAttach->mChildren.empty()) {
            for (std::set<CLayout*>::iterator it = mAttach->mChildren.begin(); it != mAttach->mChildren.end(); ++it) {
                (*it)->UpdateAttachedLayouts();
            }
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
        nw4r::lyt::TextBox* pTextBox = nw4r::ut::DynamicCast<nw4r::lyt::TextBox*>(pPane);
        if (pTextBox != nullptr) {
            pTextBox->AllocStringBuffer(length);
            pTextBox->SetString(pText, 0);
            pResult = pTextBox;
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

nw4r::math::_VEC2 CLayout::NormalizeViewPosition(const nw4r::math::VEC2& rPosition) const {
    nw4r::math::_VEC2 result;
    result.y = -2.0f * ((rPosition.y - mViewRect.bottom) / (mViewRect.top - mViewRect.bottom) - 0.5f);
    result.x = 2.0f * ((rPosition.x - mViewRect.left) / (mViewRect.right - mViewRect.left) - 0.5f);
    return result;
}

void CLayout::Calc() {
    if (!mPanePrograms.empty()) {
        for (PaneProgramMap::iterator it = mPanePrograms.begin(); it != mPanePrograms.end(); ++it) {
            AdvancePaneMotions((*it).second.get(), 1.0f);
        }
    }
}
