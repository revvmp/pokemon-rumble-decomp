#include <game/IPauseMenu.h>

#include <nw4r/lyt/lyt_texMap.h>
#include <game/BUILayout.h>
#include <game/CControllerComposition.h>
#include <game/CFiber.h>
#include <game/CLayout.h>
#include <game/CUIManager.h>
#include <game/CUIMenu.h>
#include <game/CaptureTexture.h>
#include <game/ControllerTable.h>
#include <game/IBattleRoyalDisplay.h>
#include <game/IController.h>
#include <game/IDlgMsgWin.h>
#include <game/IDlgPiiSelector.h>
#include <game/IGameMap.h>
#include <game/IGameSession.h>
#include <game/LayoutResources.h>
#include <game/MapDesc.h>
#include <game/MessageFormat.h>
#include <game/Memory.h>
#include <game/Message.h>
#include <game/RenderMode.h>
#include <game/Pad.h>
#include <game/SaveData.h>
#include <game/Sound.h>
#include <game/StageID.h>
#include <game/SystemMessage.h>
#include <algorithm>
#include <functional>
#include <TMC_JPEG/tmcjpeg.h>
#include <math.h>
#include <nw4r/g3d/g3d_init.h>
#include <nw4r/g3d/g3d_state.h>
#include <nw4r/lyt/lyt_pane.h>
#include <revolution/gx.h>
#include <revolution/gx/GXTexture.h>
#include <revolution/os.h>
#include <vector>

bool IsStage7EvenMap(MapIndex index);
bool IsBattleRoyalFirstPart(MapIndex index);
bool IsGameFlagSet(int flag);
void PauseGameAudio();
void ResumeGameAudio();
void SaveScreenshotToRemoteMemory(IDlgMsgWin* pDialog, BUILayout* pLayout, const void* pJpeg, u32 size);
void OpenRemoteMemoryDialog(IDlgMsgWin* pDialog, BUILayout* pLayout);

template <typename A, typename B, typename C>
std::wstring FormatMessage(const wchar_t* pFormat, const A& rFirst, const B& rSecond, const C& rThird);

namespace {

inline bool IsHubMap() {
    return GetCurrentGameMap()->GetMapDesc().mIndex.mStage == 0;
}

inline MapIndex GetHubIndex(MapIndex index) {
    MapIndex hub = {index.mMode, index.mArea, 0, 0};
    return hub;
}

enum MenuKind {
    MENU_LEAVE = 0,
    MENU_CONTROLLERS = 1,
    MENU_SCREENSHOT = 2,
    MENU_REMOTE_MEMORY = 3,
    MENU_RESUME = 0x20000000
};

class CPauseMenuInput : public IUIInput {
public:
    virtual bool IsConnected() const { return GetControllerTable()->IsConnected(); }
    virtual f32 GetStickLength() const { return GetControllerTable()->GetStickLength(); }
    virtual f32 GetStickAngle() const { return GetControllerTable()->GetStickAngle(); }
    virtual u32 GetHold() const { return Remap(GetControllerTable()->GetHold()); }
    virtual u32 GetTrig() const { return Remap(GetControllerTable()->GetTrig()); }
    virtual u32 GetRelease() const { return Remap(GetControllerTable()->GetRelease()); }
    virtual u32 GetRepeat() const { return Remap(GetControllerTable()->GetRepeat()); }

private:
    static u32 Remap(u32 buttons) {
        if (buttons & 0x100) {
            buttons |= 0x10;
        }
        return buttons;
    }
};

class CPauseMenu : public IPauseMenu {
public:
    CPauseMenu() :
        mBackManager(new CUIManager),
        mFrontManager(new CUIManager),
        mBackgroundLayout(new BUILayout),
        mBlackLayout(new BUILayout),
        mRemoteMemoryLayout(new BUILayout),
        mIsOpen(false),
        mScreenshotBusy(false),
        mCaptureRequested(false),
        mHideBackground(false) {
        mPictureSize.x = 0.0f;
        mPictureSize.y = 0.0f;
    }

    virtual ~CPauseMenu() {}

    virtual bool IsOpen() { return mIsOpen; }

    virtual void Calc() {
        if (mScreenshotLayout.IsVisible()) {
            mScreenshotLayout.Calc();
        }
        mBackManager->Calc();
        mFrontManager->Calc();
        mFiber.Resume();
        if (mDialog.get()) {
            mDialog->Calc();
        }
    }

    virtual void CalcMtx() {
        mBackManager->CalcMtx();
        mFrontManager->CalcMtx();
        if (mDialog.get()) {
            mDialog->CalcMtx();
        }
    }

    virtual void CalcView() {
        mBackManager->CalcView();
        if (mDialog.get()) {
            mDialog->CalcView();
        }
        mFrontManager->CalcView();
    }

    virtual void Draw() {
        if (mCaptureRequested) {
            mCapture.CopyFromEFB(false);
            mCaptureRequested = false;
        }
        mBackManager->Draw();
        mScreenshotLayout.Draw();
        if (mDialog.get()) {
            mDialog->Draw();
        }
        mFrontManager->Draw();
    }

    virtual bool TakeScreenshot();
    void Run();
    void LeaveStage();
    bool RunRemoteMemoryMenu();

    void Init(const MapDesc* pDesc, bool hideBackground, bool background) {
        mHideBackground = hideBackground;
        const GXRenderModeObj* pMode = nw4r::g3d::G3DState::GetRenderModeObj();
        mCapture.Create(pMode->fbWidth, pMode->efbHeight, GX_TF_RGBA8, GX_CLAMP, GX_LINEAR);
        std::tr1::shared_ptr<CLayout::FontMap> fonts = GetDefaultFontMap();
        const char* const* pArchives = gSharedLayoutArchives;
        mBackgroundLayout->Load(pArchives, "pausemenu_bk", fonts, background);
        mBlackLayout->Load(pArchives, "black", fonts, background);
        mRemoteMemoryLayout->Load(pArchives, "remote_memory", fonts, background);
        mCopyrightLayout.Load(pArchives, "copyright", fonts, background);
        mCopyrightLayout.SetText("COPYRIGHT", GetMessageString(0x6008A, nullptr));
        nw4r::math::VEC2 copyrightScale(1.0f, 1.0f);
        mCopyrightLayout.SetViewScale(copyrightScale);
        mCopyrightLayout.Calc();
        mScreenshotLayout.Load(pArchives, "screenshot", fonts, background);
        mScreenshotLayout.SetVisible(false);
        nw4r::lyt::Pane* pScreenshotPane = mScreenshotLayout.FindPane("SS");
        mScreenshotScale.x = pScreenshotPane->GetScale().x;
        mScreenshotScale.y = pScreenshotPane->GetScale().y;
        nw4r::lyt::Pane* pPicturePane = mScreenshotLayout.FindPane("PICTURE");
        mPictureSize.x = pPicturePane->GetSize().width;
        mPictureSize.y = pPicturePane->GetSize().height;

        std::wstring title;
        MapIndex index = pDesc->mIndex;
        int mode = index.mMode;
        const wchar_t* pModeText = GetMessageString(gStageKindMessageIDs[mode], nullptr);
        const wchar_t* pAreaText = GetMessageString(gStageRanks[index.mArea].mMessageID, nullptr);
        int stage = index.mStage;
        const wchar_t* pStageText = GetMessageString(gStageWorlds[stage].mMessageID, nullptr);
        if (mode == 2) {
            if (stage == 7) {
                int count;
                int part = IsBattleRoyalFirstPart(pDesc->mIndex) ? 1 : 2;
                count = static_cast<s8>(GetSaveData(-1)->mData.mStatus[0x13]);
                title = FormatMessage(GetMessageString(0x6007D, nullptr), count, part);
            } else {
                title = FormatMessage(GetMessageString(0x6007C, nullptr), pStageText);
            }
        } else if (static_cast<s8>(GetSaveData(-1)->mData.mFlags[1]) >= 1) {
            title = FormatMessage(GetMessageString(0x6007B, nullptr), pModeText, pAreaText, pStageText);
        } else {
            title = FormatMessage(GetMessageString(0x6007A, nullptr), pAreaText, pStageText);
        }
        mBackgroundLayout->GetLayout()->SetText("MAPID", title.c_str());
        if (mHideBackground) {
            mBackgroundLayout->SetVisible(false);
        }
        mBackManager->AddChild(mBackgroundLayout);
        mFrontManager->AddChild(mRemoteMemoryLayout);
        mFiber.Start(std::tr1::bind(&CPauseMenu::Run, this), 0x8000);
    }

private:
    void ShowMenu(bool simplified) {
        UIMenuItem controllers = {nullptr, MENU_CONTROLLERS};
        UIMenuItem remoteMemory = {nullptr, MENU_REMOTE_MEMORY};
        UIMenuItem screenshot = {nullptr, MENU_SCREENSHOT};
        UIMenuItem resume = {nullptr, MENU_RESUME};
        std::vector<UIMenuItem> items;
        {
            UIMenuItem item = resume;
            item.mText = GetMessageString(0x60080, nullptr);
            items.push_back(item);
        }
        if (!simplified) {
            UIMenuItem leave = {nullptr, MENU_LEAVE};
            leave.mText = GetMessageString(IsHubMap() ? 0x60082 : 0x60081, nullptr);
            items.push_back(leave);
        }
        {
            UIMenuItem item = screenshot;
            item.mText = GetMessageString(0x60083, nullptr);
            items.push_back(item);
        }
        if (!simplified) {
            UIMenuItem item = remoteMemory;
            item.mText = GetMessageString(0x60084, nullptr);
            items.push_back(item);
        }
        if (!simplified && IsHubMap() && IsGameFlagSet(3)) {
            UIMenuItem item = controllers;
            item.mText = GetMessageString(0x60086, nullptr);
            items.push_back(item);
        }

        UIMenuParam baseParam = {nullptr, nullptr, 0, 1, 0, -1, nullptr, "menu"};
        UIMenuParam param = baseParam;
        param.mTitle = GetMessageString(0x6007E, nullptr);
        param.mpItems = &items[0];
        param.mItemCount = items.size();
        param.mRowCount = items.size();
        param.mpArchives = gSharedLayoutArchives;
        PlaySound(9);

        while (true) {
            CPauseMenuInput input;
            CUIMenuCallbacks callbacks(0);
            int choice = RunUIMenu(param, callbacks, &input, mBackManager, GetDefaultFontMap(), true);
            switch (choice) {
            default:
                return;
            case MENU_LEAVE:
                LeaveStage();
                return;
            case MENU_CONTROLLERS:
                GetCurrentGameMap()->GetDlgPiiSelector()->ShowMultiPlayerEntrance();
                return;
            case MENU_SCREENSHOT:
                TakeScreenshot();
                return;
            case MENU_REMOTE_MEMORY:
                if (RunRemoteMemoryMenu()) {
                    return;
                }
                YieldFiber();
                break;
            }
        }
    }

    std::tr1::shared_ptr<CUIManager> mBackManager;
    std::tr1::shared_ptr<CUIManager> mFrontManager;
    std::tr1::shared_ptr<BUILayout> mBackgroundLayout;
    std::tr1::shared_ptr<BUILayout> mBlackLayout;
    std::tr1::shared_ptr<BUILayout> mRemoteMemoryLayout;
    bool mIsOpen;
    CFiber mFiber;
    std::tr1::shared_ptr<IDlgMsgWin> mDialog;
    CaptureTexture mCapture;
    bool mScreenshotBusy;
    bool mCaptureRequested;
    bool mHideBackground;
    CLayout mScreenshotLayout;
    CLayout mCopyrightLayout;
    nw4r::math::VEC2 mScreenshotScale;
    nw4r::math::VEC2 mPictureSize;
};

}

std::tr1::shared_ptr<IPauseMenu> IPauseMenu::Create(const MapDesc* pDesc, bool hideBackground, bool background) {
    std::tr1::shared_ptr<CPauseMenu> menu(new CPauseMenu);
    menu->Init(pDesc, hideBackground, background);
    return menu;
}

void CPauseMenu::Run() {
    YieldFiberStarted();
    while (true) {
        mIsOpen = false;
        while (true) {
            if ((GetControllerTable()->GetTrig() & 0x100) && IGameSession::GetInstance()->IsMapChangeFinished() &&
                (GetCurrentGameMap()->GetCurrentDialog() == nullptr || GetCurrentGameMap()->GetCurrentDialog()->IsClosable()) &&
                IsGameFlagSet(2)) {
                break;
            }
            YieldFiber();
        }

        CDisablePadRumble rumble;
        mIsOpen = true;
        mBackgroundLayout->GetLayout()->CreateMotion("begin");
        PauseGameAudio();

        bool simplified = mHideBackground;
        if (IsStage7EvenMap(GetCurrentGameMap()->GetMapDesc().mIndex)) {
            if (GetCurrentGameMap()->GetBattleRoyalDisplay() != nullptr) {
                simplified = GetCurrentGameMap()->GetBattleRoyalDisplay()->GetEnemyCount() == 0;
            }
        }
        if (GetCurrentGameMap()->IsPausing()) {
            simplified = true;
        }

        ShowMenu(simplified);

        ResumeGameAudio();
        YieldFiber();
        mBackgroundLayout->GetLayout()->CreateMotion("end");
    }
}

void CPauseMenu::LeaveStage() {
    if (!IsHubMap()) {
        mDialog = IDlgMsgWin::Create(false, true);
        int stage = GetCurrentGameMap()->GetMapDesc().mIndex.mStage;
        const wchar_t* pMessage = GetMessageString(stage == 7 ? 0x60088 : 0x60087, nullptr);
        IDlgMsgWin* pDialog = mDialog.get();
        if (!pDialog->AskYesNo(pMessage)) {
            pDialog = mDialog.get();
            pDialog->WaitUntilClosed();
            mDialog.reset();
            return;
        }
    }

    mBackManager->AddChild(mBlackLayout);
    mBlackLayout->GetLayout()->CreateMotion("fadeout")->Wait();
    IGameSession::GetInstance()->SetFade(1, 0);
    if (IsHubMap()) {
        IGameSession::GetInstance()->RequestSceneChange(3, 4);
    } else {
        IGameSession::GetInstance()->RequestMapChange(GetHubIndex(GetCurrentGameMap()->GetMapDesc().mIndex), 2, 4);
    }
    ResumeGameAudio();
    mIsOpen = false;
    while (true) {
        YieldFiber();
    }
}

bool CPauseMenu::TakeScreenshot() {
    if (mScreenshotBusy) {
        return false;
    }
    mScreenshotBusy = true;

    std::tr1::shared_ptr<u8> work(static_cast<u8*>(GameMemory::Allocate(0x1c00, 32)), GameMemory::Free);
    if (work.get() == nullptr) {
        OSReport("%s(%d): not enough memory\n", "CPauseMenu.cpp", 439);
        mScreenshotBusy = false;
        return true;
    }

    const GXRenderModeObj* pMode = nw4r::g3d::G3DState::GetRenderModeObj();
    mCaptureRequested = true;
    while (mCaptureRequested) {
        YieldFiber();
    }

    u16 efbWidth = pMode->fbWidth;
    u16 efbHeight = pMode->efbHeight;
    f32 pixelAspect = (static_cast<f32>(pMode->viWidth) / static_cast<f32>(efbWidth)) * GetPixelAspectRatio() /
                      (static_cast<f32>(pMode->viHeight) / static_cast<f32>(efbHeight));
    f32 maxWidth = static_cast<f32>(efbWidth);
    f32 maxHeight = static_cast<f32>(efbHeight);
    f32 maxAspect = maxWidth / maxHeight;
    f32 shotWidth = pixelAspect * static_cast<f32>(mCapture.GetWidth());
    f32 shotHeight = static_cast<f32>(mCapture.GetHeight());
    f32 shotAspect = shotWidth / shotHeight;
    u16 width;
    u16 height;
    if (shotWidth <= maxWidth && shotHeight <= maxHeight) {
        width = static_cast<u16>(ceil(shotWidth));
        height = static_cast<u16>(ceil(shotHeight));
    } else if (shotAspect <= maxAspect) {
        width = static_cast<u16>(ceil(maxHeight * shotAspect));
        height = static_cast<u16>(ceil(maxHeight));
    } else {
        width = static_cast<u16>(ceil(maxWidth));
        height = static_cast<u16>(ceil(maxWidth / shotAspect));
    }
    u16 limitWidth = static_cast<u16>(maxWidth);
    width = std::min(width, limitWidth);
    u16 limitHeight = static_cast<u16>(maxHeight);
    height = std::min(height, limitHeight);

    nw4r::lyt::Pane* pPicture = mCopyrightLayout.FindPane("PICTURE");
    nw4r::lyt::TexMap texMap;
    texMap.Set(mCapture.GetTexObj());
    texMap.SetPalette(nullptr);
    texMap.SetPaletteFormat(GX_TL_IA8);
    texMap.SetPaletteEntryNum(0);
    mCopyrightLayout.SetTexture(pPicture, texMap, -1);
    pPicture->SetSize(nw4r::lyt::Size(width, height));
    nw4r::lyt::Pane* pCopyright = mCopyrightLayout.FindPane("COPYRIGHT");
    pCopyright->SetTranslate(nw4r::math::VEC3(width * 0.5f, -height * 0.5f, 0.0f));

    nw4r::g3d::G3DState::Invalidate(0x7ff);
    nw4r::g3d::G3dReset();
    GXInvalidateVtxCache();
    GXInvalidateTexAll();
    GXSetPixelFmt(GX_PF_RGB8_Z24, GX_ZC_LINEAR);
    mCopyrightLayout.Render(nw4r::math::VEC2(height, width));
    GXDrawDone();

    CaptureTexture shot;
    shot.Init(mCapture.GetBufferOwner(), mCapture.GetBufferSize(), width, height, mCapture.GetFormat(), GX_CLAMP, GX_LINEAR);
    shot.CopyFromEFB(true);

    nw4r::lyt::TexMap shotMap;
    shotMap.Set(shot.GetTexObj());
    shotMap.SetPalette(nullptr);
    shotMap.SetPaletteFormat(GX_TL_IA8);
    shotMap.SetPaletteEntryNum(0);
    mScreenshotLayout.SetTexture("PICTURE", shotMap, -1);

    f32 shotRatio = static_cast<f32>(shot.GetWidth()) / static_cast<f32>(shot.GetHeight());
    f32 frameRatio = (mPictureSize.x / mScreenshotLayout.GetViewScale().x) / (mPictureSize.y / mScreenshotLayout.GetViewScale().y);
    nw4r::math::VEC2 scale = mScreenshotScale;
    if (frameRatio <= shotRatio) {
        scale.y *= frameRatio / shotRatio;
    } else {
        scale.x *= shotRatio / frameRatio;
    }
    mScreenshotLayout.FindPane("SS")->SetScale(scale);

    PlaySound(0);
    mScreenshotLayout.SetVisible(true);
    mScreenshotLayout.CreateMotion("prompt_begin", "PROMPT", 0.0f, 1.0f);
    mScreenshotLayout.QueueMotion("prompt_loop", "PROMPT", 0.0f, 1.0f);
    mScreenshotLayout.CreateMotion("begin", "SS_ROOT", 0.0f, 1.0f)->Wait();

    u32 jpegSize;
    TMCJPEGEncode(work.get(), shot.GetBufferOwner().get(), shot.GetWidth(), shot.GetHeight(), shot.GetFormat(), &jpegSize, 400);
    std::tr1::shared_ptr<u8> jpeg(static_cast<u8*>(GameMemory::AllocateMEM2(jpegSize, 32)), GameMemory::Free);
    if (jpeg.get() == nullptr) {
        OSReport("%s(%d): not enough memory\n", "CPauseMenu.cpp", 556);
        mScreenshotBusy = false;
        return true;
    }
    TMCJPEGEncodeToBuffer(work.get(), shot.GetBufferOwner().get(), shot.GetWidth(), shot.GetHeight(), shot.GetFormat(), 400, jpeg.get(), jpegSize, &jpegSize);

    while (!(GetControllerTable()->GetTrig() & 0x20)) {
        YieldFiber();
    }
    PlaySound(8);
    mScreenshotLayout.CreateMotion("prompt_end", "PROMPT", 0.0f, 1.0f)->Wait();

    mDialog = IDlgMsgWin::Create(false, true);
    SaveScreenshotToRemoteMemory(mDialog.get(), mRemoteMemoryLayout.get(), jpeg.get(), jpegSize);
    mDialog->WaitUntilClosed();
    mDialog.reset();

    mScreenshotLayout.CreateMotion("end", "SS_ROOT", 0.0f, 1.0f)->Wait();
    mScreenshotLayout.SetVisible(false);
    mScreenshotBusy = false;
    return true;
}

bool CPauseMenu::RunRemoteMemoryMenu() {
    UIMenuItem items[2] = {{nullptr, 0}, {nullptr, MENU_RESUME}};
    items[0].mText = GetMessageString(0x60085, nullptr);
    items[1].mText = GetMessageString(0x60089, nullptr);
    UIMenuParam param = {nullptr, nullptr, 2, 1, 2, -1, nullptr, "menu"};
    param.mTitle = GetMessageString(0x6007F, nullptr);
    param.mpItems = items;
    param.mpArchives = gSharedLayoutArchives;
    CUIMenuCallbacks callbacks(0);
    IUIInput* pInput = GetControllerTable();
    int choice = RunUIMenu(param, callbacks, pInput, mBackManager, GetDefaultFontMap(), true);
    if (choice != 0) {
        return false;
    }

    mDialog = IDlgMsgWin::Create(false, true);
    OpenRemoteMemoryDialog(mDialog.get(), mRemoteMemoryLayout.get());
    mDialog->WaitUntilClosed();
    mDialog.reset();
    return true;
}

template <typename A, typename B, typename C>
std::wstring FormatMessage(const wchar_t* pFormat, const A& rFirst, const B& rSecond, const C& rThird) {
    NIntl::basic_formatter<wchar_t> formatter(pFormat);
    return (formatter % rFirst % rSecond % rThird).str();
}
