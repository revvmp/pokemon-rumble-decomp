#include <game/CDisplayPhaseCtrl.h>
#include <game/IGameMap.h>

CDisplayPhaseCtrl::CDisplayPhaseCtrl(CLayout* pLayout) :
    mPhase(PHASE_HIDDEN), mShowRequested(false), mForceVisible(false), mpLayout(pLayout), mpPane(nullptr) {}

CDisplayPhaseCtrl::~CDisplayPhaseCtrl() {}

void CDisplayPhaseCtrl::SetTarget(CLayout* pLayout, nw4r::lyt::Pane* pPane) {
    mpLayout = pLayout;
    mpPane = pPane;
}

void CDisplayPhaseCtrl::SetTarget(CLayout* pLayout, const char* pPaneName) {
    mpLayout = pLayout;
    mpPane = pLayout->FindPane(pPaneName);
}

void CDisplayPhaseCtrl::SetForceVisible(bool visible) { mForceVisible = visible; }
void CDisplayPhaseCtrl::Show() { mShowRequested = true; }
void CDisplayPhaseCtrl::Hide() { mShowRequested = false; }
bool CDisplayPhaseCtrl::IsVisible() const { return mPhase != PHASE_HIDDEN; }

void CDisplayPhaseCtrl::Begin() {
    switch (mPhase) {
    case PHASE_HIDDEN:
        mPhase = PHASE_BEGIN;
        mMotion = mpLayout->CreateMotion("begin", mpPane, 0.0f, 1.0f);
        mpLayout->QueueMotion("loop", mpPane, 0.0f, 1.0f);
        break;
    case PHASE_BEGIN_HIDE:
        mPhase = PHASE_BEGIN;
        break;
    case PHASE_END:
        mPhase = PHASE_END_SHOW;
        break;
    }
}

void CDisplayPhaseCtrl::End() {
    switch (mPhase) {
    case PHASE_BEGIN:
        mPhase = PHASE_BEGIN_HIDE;
        break;
    case PHASE_VISIBLE:
        mPhase = PHASE_END;
        mMotion = mpLayout->CreateMotion("end", mpPane, 0.0f, 1.0f);
        break;
    case PHASE_END_SHOW:
        mPhase = PHASE_END;
        break;
    }
}

void CDisplayPhaseCtrl::Calc() {
    if ((!GetCurrentGameMap()->IsPausing() && mShowRequested) || mForceVisible) {
        Begin();
    } else {
        End();
    }
    Phase phase = mPhase;
    switch (phase) {
    case PHASE_BEGIN:
    case PHASE_BEGIN_HIDE:
        if (mMotion->IsEnded()) {
            mMotion.reset();
            mPhase = PHASE_VISIBLE;
            if (phase == PHASE_BEGIN_HIDE) {
                End();
            }
        }
        break;
    case PHASE_END:
    case PHASE_END_SHOW:
        if (mMotion->IsEnded()) {
            mMotion.reset();
            mPhase = PHASE_HIDDEN;
            if (phase == PHASE_END_SHOW) {
                Begin();
            }
        }
        break;
    }
}
