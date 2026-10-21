#include <game/CLayout.h>

#include <game/CFiber.h>
#include <game/RenderMode.h>
#include <functional>
#include <nw4r/g3d/g3d_state.h>
#include <revolution/os.h>
#include <revolution/sc.h>

namespace {

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

void CLayout::Load(const char* pArchive, const char* pLayoutName, const std::tr1::shared_ptr<FontMap>& rFonts, bool background) {
    int error = LoadArchive(pArchive, pLayoutName, rFonts, background);
    if (error != 0) {
        CInitializeException exception(error, pArchive, pLayoutName, rFonts);
        if (sInitializeErrorHandler) {
            (*sInitializeErrorHandler)(exception);
            OSPanic("CLayout.cpp", 337, exception.what());
        } else {
            throw exception;
        }
    }
}

void CLayout::SetViewScale(const nw4r::math::VEC2& rScale) {
    mDrawInfo.SetLocationAdjustScale(rScale);
    UpdateViewRect();
}
