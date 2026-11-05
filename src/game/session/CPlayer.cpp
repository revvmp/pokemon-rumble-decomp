#pragma dont_inline on

#include <game/session/CPlayer.h>
#include <game/pokemon/CPlayerPii.h>
#include <game/input/IController.h>

CPlayer::State::State() : mJoined(false), mPiiBox(0, 1200) {}

CPlayer::CPlayer(int playerNo) : mFlags(0), mPlayerNo(playerNo), mController(nullptr), mRankingScore(0), mAccumulatedHold(0), mCurrentHold(0) {}

CPlayer::~CPlayer() {}

bool CPlayer::Join()
{
    if (mState.mJoined) {
        return true;
    }
    mState.mJoined = true;
    OnStateChanged();
    return true;
}

void CPlayer::OnStateChanged() {}

void CPlayer::Leave()
{
    mState.mJoined = false;
    mState.mPersonalData.reset();
    mState.mPendingPersonalData.reset();
    OnStateChanged();
}

void CPlayer::SaveState()
{
    mSavedState = mState;
}

void CPlayer::RestoreState()
{
    mState = mSavedState;
}

void CPlayer::SetController(IController* pController)
{
    mController = pController;
}

void CPlayer::SetPii(CPlayerPii* pPii)
{
    mPii = pPii->GetSharedPtr();
    mState.mPersonalData = pPii->GetPersonalData();
    ResetPendingPersonalData();
}

namespace {

template <typename T> std::tr1::shared_ptr<T> LockPii(std::tr1::weak_ptr<T> pii)
{
    std::tr1::shared_ptr<T> locked = pii.lock();
    return locked && locked->IsInWorld() ? locked : std::tr1::shared_ptr<T>();
}

}

CPlayerPii* CPlayer::GetPii()
{
    return LockPii(mPii).get();
}

void CPlayer::SetPendingPersonalData(const std::tr1::shared_ptr<CPiiPersonalData>& rPersonalData)
{
    mState.mPendingPersonalData = rPersonalData;
}

void CPlayer::ResetPendingPersonalData()
{
    mState.mPendingPersonalData.reset();
}

void CPlayer::CompletePiiChangeRequest()
{
    ClearFlag(FLAG_PII_CHANGE_REQUESTED);
}

void CPlayer::BeginHoldCollection()
{
    SetFlag(FLAG_COLLECT_HOLD);
    mAccumulatedHold = mController ? mController->GetHold() : 0;
}

void CPlayer::EndHoldCollection()
{
    ClearFlag(FLAG_COLLECT_HOLD);
}

void CPlayer::Calc()
{
    u32 hold = mController ? mController->GetHold() : 0;
    if (IsCollectingHold()) {
        mAccumulatedHold |= hold;
    } else {
        mCurrentHold = hold;
    }
}
