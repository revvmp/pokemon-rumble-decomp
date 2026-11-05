#include <game/ui/CStrapReminder.h>

#include <game/io/FileList.h>
#include <game/ui/LayoutResources.h>
#include <game/input/Pad.h>

inline bool IsWiiPadReady(const WiiPadState* pPad) { return pPad != nullptr && pPad->mProcessingState == 0; }
inline bool HasWiiSample(const KPADStatus* pStatus) { return pStatus != nullptr && (pStatus->wpad_err == 0 || pStatus->wpad_err == -7); }
inline bool IsWiiPadActive(const WiiPadState* pPad) { return IsWiiPadReady(pPad) && HasWiiSample(pPad); }
inline void WaitForButton(int frames) {
    for (int i = 0; i < frames; ++i) {
        WiiPadState* pPad = GetWiiPad(0);
        if (IsWiiPadActive(pPad) && (pPad->trig & 0x1F10)) { break; }
        YieldFiber();
    }
}
inline void CStrapReminder::Draw() {
    mLayout.Draw();
}
inline void CStrapReminder::Calc() {
    mLayout.Calc();
    mFiber.Resume();
}
inline void CStrapReminder::Finish() {
    if (!mCloseQueued) {
        mCloseQueued = true;
        WaitForButton(1200);
        mLayout.QueueMotion("end")->Wait();
    } else {
        mLayout.AppendMotion("loading_out")->Wait();
    }
}
inline void CStrapReminder::Run() {
    YieldFiberStarted();
    mLayout.CreateMotion("begin")->Wait();
    for (int i = 0; i < 60; ++i) { YieldFiber(); }
    WaitForButton(1200);
    if (!mCloseQueued) {
        mCloseQueued = true;
        mLayout.QueueMotion("end");
        mLayout.AppendMotion("loading_in");
        mLayout.AppendMotion("loading_loop");
    }
}
inline void CStrapReminder::Initialize(bool background) {
    const char** pArchives;
    std::tr1::shared_ptr<FileList> files = LoadFileList(reinterpret_cast<void**>(&pArchives), "title2d.arc", 0, background, nullptr);
    mLayout.Load(pArchives, "wii_remote", std::tr1::shared_ptr<CLayout::FontMap>(), background);
    mFiber.Start(std::tr1::bind(&CStrapReminder::Run, this), 0x8000);
}
std::tr1::shared_ptr<IStrapReminder> CStrapReminder::Create(bool background) {
    std::tr1::shared_ptr<CStrapReminder> reminder(new CStrapReminder);
    reminder->Initialize(background);
    return reminder;
}
