#include <game/detail/CDisconMsg.h>
#include <game/ControllerTable.h>
#include <game/LayoutResources.h>
#include <game/Message.h>

void PauseGameAudio();
void ResumeGameAudio();

std::tr1::shared_ptr<IDisconMsg> CreateDisconMsg(bool background) {
    return std::tr1::shared_ptr<CDisconMsg>(new CDisconMsg);
}
namespace {
void CDisconMsg::Draw() {
    if (mLayout) { mLayout->Draw(); }
}
void CDisconMsg::CalcView() {
    if (mLayout) { mLayout->CalcView(); }
}
void CDisconMsg::CalcMtx() {
    if (mLayout) { mLayout->CalcMtx(); }
}
void CDisconMsg::Calc() {
    if (!mFiber) {
        if (GetControllerDisconnectState() && CanShow()) {
            mFiber.reset(new CFiber("CDisconMsg", std::tr1::bind(&CDisconMsg::Run, this), 0x8000));
        }
    } else {
        mFiber->Resume();
        if (mFiber->IsFinished()) { mFiber.reset(); }
    }
    if (mLayout) { mLayout->Calc(); }
}
bool CDisconMsg::CanShow() const { return mSuppressCount == 0; }
void CDisconMsg::Run() {
    PauseGameAudio();
    std::tr1::shared_ptr<CLayout::FontMap> fonts = GetDefaultFontMap();
    mLayout.reset(new CLayout);
    mLayout->Load("5:/game2d.arc", "disconmsg", fonts, true);
    mLayout->SetVisible(false);
    mLayout->SetPaneVisible("PROMPT", false);
    mLayout->CreateMotion("loop", "PROMPT", 0.0f, 1.0f);
    YieldFiberStarted();
    for (;;) {
        int reason = GetControllerDisconnectState();
        if (!reason) { break; }
        mLayout->SetVisible(true);
        int message;
        switch (reason) {
        case 1: message = 393449; break;
        case 2: message = 393450; break;
        case 3: message = 393451; break;
        case 5: message = 393448; break;
        default: message = 65557; break;
        }
        const wchar_t* pText = GetMessageText(message, L"");
        mLayout->SetText("TEXT", pText);
        mLayout->CreateMotion("begin");
        mLayout->QueueMotion("loop");
        while (reason == GetControllerDisconnectState()) {
            YieldFiber();
        }
        for (int frame = 0; frame < 60; ++frame) {
            YieldFiber();
        }
        mLayout->CreateMotion("end")->Wait();
    }
    mLayout.reset();
    ResumeGameAudio();
}
void CDisconMsg::AdjustSuppressCount(int amount) { mSuppressCount += amount; }
bool CDisconMsg::IsActive() const { return bool(mFiber); }
CDisconMsg::~CDisconMsg() {}
}
