#include <game/PaneRectangle.h>
#include <game/CLayout.h>
#include <game/GameLoop.h>
#include <game/Graphics.h>
#include <game/CRomFont.h>
#include <game/Pad.h>
#include <game/CScopedActivity.h>
#include <game/NResetMgr.h>
#include <game/SystemMessage.h>
#include <nw4r/lyt/lyt_pane.h>
#include <nw4r/lyt/lyt_textBox.h>
#include <nw4r/ut/ut_RuntimeTypeInfo.h>
#include <nw4r/ut/ut_Rect.h>
#include <nw4r/g3d/g3d_state.h>
#include <nw4r/g3d/g3d_init.h>
#include <revolution/GX.h>
#include <revolution/gx/GXTexture.h>
#include <revolution/gx/GXPixel.h>
#include <revolution/wpad.h>
#include <algorithm>
#include <functional>
#include <iterator>
#include <list>
#include <math.h>

inline bool IsWiiPadReadable(const WiiPadState* pPad) {
    return pPad->wpad_err == 0 || pPad->wpad_err == -7;
}
inline bool IsWiiPadConnected(const WiiPadState* pPad) {
    return pPad && IsWiiPadReadable(pPad);
}
inline bool IsWiiPadActive(const WiiPadState* pPad) {
    return pPad && pPad->mProcessingState == 0;
}
inline bool CanReadWiiPad(const WiiPadState* pPad) {
    return IsWiiPadActive(pPad) && IsWiiPadConnected(pPad);
}
inline bool IsGCPadReadable(const GCPadState* pPad) {
    return pPad->err == 0 || pPad->err == -3;
}
inline bool IsGCPadConnected(const GCPadState* pPad) {
    return pPad && IsGCPadReadable(pPad);
}
std::wstring WrapText(const std::wstring& rText, const nw4r::lyt::TextBox& rTextBox);

namespace NErrMsg {
PaneRectangle GetPaneRectangle(const nw4r::lyt::Pane* pPane) {
    const nw4r::math::VEC3 position(pPane->GetTranslate());
    PaneRectangle rectangle;
    const nw4r::lyt::Size size(pPane->GetSize());
    switch (pPane->GetBasePositionH()) {
    case 0: rectangle.left = position.x; break;
    case 1: rectangle.left = position.x - size.width / 2.0f; break;
    case 2: rectangle.left = position.x - size.width; break;
    }
    rectangle.right = rectangle.left + size.width;
    switch (pPane->GetBasePositionV()) {
    case 2: rectangle.bottom = position.y; break;
    case 1: rectangle.bottom = position.y - size.height / 2.0f; break;
    case 0: rectangle.bottom = position.y - size.height; break;
    }
    rectangle.top = rectangle.bottom + size.height;
    return rectangle;
}
void SetPaneRectangle(nw4r::lyt::Pane* pPane, const PaneRectangle& rRectangle) {
    nw4r::math::VEC3 position = pPane->GetTranslate();
    switch (pPane->GetBasePositionH()) {
    case 0: position.x = rRectangle.left; break;
    case 1: position.x = rRectangle.left + rRectangle.GetWidth() / 2.0f; break;
    case 2: position.x = rRectangle.left + rRectangle.GetWidth(); break;
    }
    switch (pPane->GetBasePositionV()) {
    case 2: position.y = rRectangle.bottom; break;
    case 1: position.y = rRectangle.bottom + rRectangle.GetHeight() / 2.0f; break;
    case 0: position.y = rRectangle.bottom + rRectangle.GetHeight(); break;
    }
    pPane->SetTranslate(position);
    pPane->SetSize(nw4r::lyt::Size(rRectangle.GetWidth(), rRectangle.GetHeight()));
}
PaneRectangle UnionRectangles(const PaneRectangle& rLeft, const PaneRectangle& rRight) {
    PaneRectangle rectangle;
    rectangle.left = std::min(rLeft.left, rRight.left);
    rectangle.right = std::max(rLeft.right, rRight.right);
    rectangle.bottom = std::min(rLeft.bottom, rRight.bottom);
    rectangle.top = std::max(rLeft.top, rRight.top);
    return rectangle;
}
}

namespace {
struct CallbackEntry {
    int mId;
    int mFlags;
    std::tr1::function<void()> mCallback;
};
struct Work {
    void* mpArchive;
    const char* mpLayoutName;
    std::string mFontName;
    u32 mWiiButtons;
    u32 mGCButtons;
    std::tr1::weak_ptr<CLayout::FontMap> mFonts;
    std::list<CallbackEntry> mCallbacks;
};
std::tr1::shared_ptr<Work> sWork;
class CErrMsgFatalLoop : public IGameLoopCallback {
public:
    CErrMsgFatalLoop(CLayout* pLayout, std::tr1::shared_ptr<CLayout::CMotionProgram> motion, int action);
    ~CErrMsgFatalLoop();
    virtual bool IsRunning() { return true; }
    virtual void ResetTiming() { SetPadReadDelay(1); }
    virtual void Update() { UpdatePad(); NResetMgr::Update(); }
    virtual bool ShouldRender() const { return true; }
    virtual void Prepare();
    void StartEndMotion() {
        mMotion = mpLayout->CreateMotion("end");
        mState = 2;
    }
    virtual void Calc() {}
    virtual void Draw() {}
    virtual void Render();
virtual void Present() {
    Graphics::DisplayFrameBuffer();
    for (std::list<CallbackEntry>::iterator it = sWork->mCallbacks.end(); it != sWork->mCallbacks.begin(); --it) {
        std::list<CallbackEntry>::const_iterator current = it;
        --current;
        if (((*current).mFlags & 1) != 0) (*current).mCallback();
    }
}
private:
    CLayout* mpLayout;
    int mState;
    std::tr1::shared_ptr<CLayout::CMotionProgram> mMotion;
    int mAction;
    CDisablePadRumble mDisableRumble;
};

inline void CErrMsgFatalLoop::Prepare() {
    switch (mState) {
    case 0:
        if (mMotion->IsEnded()) { mMotion.reset(); mState = 1; }
        break;
    case 1:
        if (mAction) {
            for (int channel = 0; channel < 4; ++channel) {
                WiiPadState* pPad = GetWiiPad(channel);
                if (CanReadWiiPad(pPad) && (pPad->trig & sWork->mWiiButtons)) {
                    StartEndMotion();
                    goto calcLayout;
                }
            }
            for (int channel = 0; channel < 4; ++channel) {
                GCPadState* pPad = GetGCPad(channel);
                if (IsGCPadConnected(pPad) && (pPad->mTrigger & sWork->mGCButtons)) {
                    StartEndMotion();
                    goto calcLayout;
                }
            }
        }
        break;
    case 2:
        if (mMotion->IsEnded()) {
            if (mAction == 2) NResetMgr::RequestReturnToDataManager();
            else NResetMgr::RequestReturnToMenu();
        }
        break;
    }
calcLayout:
    mpLayout->Calc();
}
inline void CErrMsgFatalLoop::Render() {
    nw4r::g3d::G3DState::Invalidate(2047);
    nw4r::g3d::G3dReset();
    GXInvalidateVtxCache();
    GXInvalidateTexAll();
    GXSetPixelFmt(GX_PF_RGB8_Z24, GX_ZC_LINEAR);
    mpLayout->Draw();
    Graphics::ResetState();
    Graphics::CopyFrameBuffer();
}
std::tr1::shared_ptr<CLayout::FontMap> GetFontMap(bool& rCreated) {
    std::tr1::shared_ptr<CLayout::FontMap> fonts = sWork->mFonts.lock();
    rCreated = false;
    if (!fonts) {
        fonts.reset(new CLayout::FontMap);
        std::tr1::shared_ptr<CRomFont> font(new CRomFont);
        font->SetAlternateChar('?');
        (*fonts)[sWork->mFontName] = font;
        sWork->mFonts = fonts;
        rCreated = true;
    }
    return fonts;
}
void ResizeFrame(CLayout* pLayout, const std::string& rPrefix, f32 width, f32 height) {
    const f32 halfWidth = width / 2.0f;
    const f32 halfHeight = height / 2.0f;
    nw4r::lyt::Pane* pNorthWest = pLayout->FindPane((rPrefix + "_nw").c_str());
    nw4r::lyt::Pane* pNorth = pLayout->FindPane((rPrefix + "_n").c_str());
    nw4r::lyt::Pane* pNorthEast = pLayout->FindPane((rPrefix + "_ne").c_str());
    nw4r::lyt::Pane* pWest = pLayout->FindPane((rPrefix + "_w").c_str());
    nw4r::lyt::Pane* pCenter = pLayout->FindPane((rPrefix + "_c").c_str());
    nw4r::lyt::Pane* pEast = pLayout->FindPane((rPrefix + "_e").c_str());
    nw4r::lyt::Pane* pSouthWest = pLayout->FindPane((rPrefix + "_sw").c_str());
    nw4r::lyt::Pane* pSouth = pLayout->FindPane((rPrefix + "_s").c_str());
    nw4r::lyt::Pane* pSouthEast = pLayout->FindPane((rPrefix + "_se").c_str());
    if (pNorthWest) pNorthWest->SetTranslate(nw4r::math::VEC3(-halfWidth, halfHeight, 0.0f));
    if (pNorth) pNorth->SetTranslate(nw4r::math::VEC3(0.0f, halfHeight, 0.0f));
    if (pNorthEast) pNorthEast->SetTranslate(nw4r::math::VEC3(halfWidth, halfHeight, 0.0f));
    if (pWest) pWest->SetTranslate(nw4r::math::VEC3(-halfWidth, 0.0f, 0.0f));
    if (pEast) pEast->SetTranslate(nw4r::math::VEC3(halfWidth, 0.0f, 0.0f));
    if (pSouthWest) pSouthWest->SetTranslate(nw4r::math::VEC3(-halfWidth, -halfHeight, 0.0f));
    if (pSouth) pSouth->SetTranslate(nw4r::math::VEC3(0.0f, -halfHeight, 0.0f));
    if (pSouthEast) pSouthEast->SetTranslate(nw4r::math::VEC3(halfWidth, -halfHeight, 0.0f));
    if (pNorth) pNorth->SetSize(nw4r::lyt::Size(width, pNorth->GetSize().height));
    if (pWest) pWest->SetSize(nw4r::lyt::Size(pWest->GetSize().width, height));
    if (pCenter) pCenter->SetSize(nw4r::lyt::Size(width, height));
    if (pEast) pEast->SetSize(nw4r::lyt::Size(pEast->GetSize().width, height));
    if (pSouth) pSouth->SetSize(nw4r::lyt::Size(width, pSouth->GetSize().height));
}
}
void InitializeSystemMessages(const void* pArchive, const char* pLayoutName, const char* pFontName, u32 wiiButtons, u32 gcButtons) {
    sWork.reset(new Work);
    sWork->mpArchive = const_cast<void*>(pArchive);
    sWork->mpLayoutName = pLayoutName;
    sWork->mFontName = pFontName;
    sWork->mWiiButtons = wiiButtons;
    sWork->mGCButtons = gcButtons;
}
void SetSystemMessageFonts(const std::tr1::shared_ptr<CLayout::FontMap>& rFonts) { sWork->mFonts = rFonts; }
int RegisterSystemMessageCallback(const std::tr1::function<void()>& rCallback, int flags) {
    const int id = sWork->mCallbacks.empty() ? 1 : sWork->mCallbacks.back().mId + 1;
    CallbackEntry entry;
    entry.mId = id;
    entry.mCallback = rCallback;
    entry.mFlags = flags;
    sWork->mCallbacks.push_back(entry);
    return id;
}

void ShowSystemMessage(const wchar_t* pText, int action) {
    CLayout layout;
    bool created = false;
    {
        std::tr1::shared_ptr<CLayout::FontMap> fonts = GetFontMap(created);
        layout.LoadFromMemory(sWork->mpArchive, sWork->mpLayoutName, fonts);
    }
    if (pText) {
        nw4r::lyt::TextBox* pTextBox = nw4r::ut::DynamicCast<nw4r::lyt::TextBox*>(layout.FindPane("text_c"));
        if (created) {
            nw4r::lyt::Size size = pTextBox->GetFontSize();
            size.width *= 0.9f;
            size.height *= 0.9f;
            pTextBox->SetFontSize(size);
        }
        layout.SetText("text_c", WrapText(std::wstring(pText), *pTextBox).c_str());
        nw4r::ut::Rect rectangle = pTextBox->GetTextDrawRect();
        f32 width = f32(ceil(rectangle.right - rectangle.left)) + 1.0f;
        f32 height = f32(ceil(rectangle.top - rectangle.bottom)) + 1.0f;
        ResizeFrame(&layout, "window", width, height);
        ResizeFrame(&layout, "shadow", width, height);
        ResizeFrame(&layout, "shadow2", width, height);
    }
    std::tr1::shared_ptr<CLayout::CMotionProgram> motion = layout.CreateMotion("begin");
    for (int channel = 0; channel < 4; ++channel) WPADControlSpeaker(channel, 1, nullptr);
    std::tr1::shared_ptr<NResetMgr::CDisable> disable;
    if (action == 0) {
        disable.reset(new NResetMgr::CDisable);
        layout.FindPane("button")->SetVisible(false);
    } else {
        NResetMgr::ClearDisableCount();
        layout.CreateMotion("button_a", layout.FindPane("button"), 0.0f, 1.0f);
    }
    typedef std::reverse_iterator<std::list<CallbackEntry>::iterator> Iterator;
    for (Iterator it(sWork->mCallbacks.end()); it != Iterator(sWork->mCallbacks.begin()); ++it) {
        if ((it->mFlags & 2) != 0) it->mCallback();
    }
    CErrMsgFatalLoop loop(&layout, motion, action);
    RunGameLoop(&loop);
}
namespace {
CErrMsgFatalLoop::CErrMsgFatalLoop(CLayout* pLayout, std::tr1::shared_ptr<CLayout::CMotionProgram> motion, int action)
    : mpLayout(pLayout), mState(1), mMotion(motion), mAction(action) {}
CErrMsgFatalLoop::~CErrMsgFatalLoop() {}
}
