#include <game/CNoticeDisplay.h>

#include <game/CLayout.h>
#include <game/IUIInput.h>

IUIInput* GetUIInput();
void YieldFiber();

void WaitMotion(std::tr1::shared_ptr<CLayout::CMotionProgram> pMotion, bool waitFinish) {
    while (true) {
        IUIInput* pInput = GetUIInput();
        if (pInput != nullptr && pInput->IsConnected() && (pInput->GetTrig() & 0x20)) {
            return;
        }
        if (pMotion && ((waitFinish && pMotion->IsFinished()) || (!waitFinish && pMotion->IsStarted()))) {
            return;
        }
        YieldFiber();
    }
}

void WaitMotionStart(std::tr1::shared_ptr<CLayout::CMotionProgram> pMotion) {
    WaitMotion(pMotion, false);
}
