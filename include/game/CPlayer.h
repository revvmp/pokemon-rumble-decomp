#ifndef GAME_CPLAYER_H
#define GAME_CPLAYER_H

#include <game/CPiiBox.h>
#include <game/ICalc.h>
#include <game/IController.h>
#include <memory>
#include <revolution/types.h>

class BWorldObj;
class CPiiPersonalData;
class CPlayerPii;
class IController;

class CMultiRanking;

class CPlayer : public ICalc {
    friend class CMultiRanking;
public:
    enum Flag {
        FLAG_COLLECT_HOLD = 1 << 0,
        FLAG_PII_CHANGE_REQUESTED = 1 << 1
    };

    explicit CPlayer(int playerNo);
    virtual ~CPlayer();
    virtual void Calc();

    bool Join();
    void Leave();
    void SaveState();
    void RestoreState();
    void SetController(IController* pController);
    bool IsJoined() const { return mState.mJoined; }
    std::tr1::shared_ptr<CPiiPersonalData> GetPersonalData() const { return mState.mPersonalData; }
    std::tr1::shared_ptr<CPiiPersonalData> GetPendingPersonalData() const { return mState.mPendingPersonalData; }
    void ResetRankingScore() { mRankingScore = 0; }
    bool HasJoinedFlag() const { return mState.mJoined; }
    CPiiBox& GetPiiBox() { return mState.mPiiBox; }
    int GetPlayerNo() const { return mPlayerNo; }
    void SetPii(CPlayerPii* pPii);
    CPlayerPii* GetPii();
    IController* GetController() { return mController; }
    void SetPendingPersonalData(const std::tr1::shared_ptr<CPiiPersonalData>& rPersonalData);
    void ResetPendingPersonalData();
    void CompletePiiChangeRequest();
    void IncrementDefeatCount() { ++mRankingScore; }
    void BeginHoldCollection();
    void EndHoldCollection();
    void RequestPiiChange() { SetFlag(FLAG_PII_CHANGE_REQUESTED); }
    bool IsPiiChangeRequested() const { return IsFlagSet(FLAG_PII_CHANGE_REQUESTED); }

    bool IsHoldCollectionEnabled() const { return IsCollectingHold(); }
    u32 GetCollectedHold() const { return mAccumulatedHold; }
    u32 GetLastHold() const { return mCurrentHold; }
private:
    struct State {
        State();

        bool mJoined;
        CPiiBox mPiiBox;
        std::tr1::shared_ptr<CPiiPersonalData> mPersonalData;
        std::tr1::shared_ptr<CPiiPersonalData> mPendingPersonalData;
    };

    void OnStateChanged();
    void SetFlag(u32 mask) { mFlags |= mask; }
    void ClearFlag(u32 mask) { mFlags &= ~mask; }
    bool IsFlagSet(u32 mask) const { return mFlags & mask; }
    bool IsCollectingHold() const { return IsFlagSet(FLAG_COLLECT_HOLD); }

    u32 mFlags;
    int mPlayerNo;
    IController* mController;
    State mState;
    State mSavedState;
    std::tr1::weak_ptr<CPlayerPii> mPii;
    int mRankingScore;
    u32 mAccumulatedHold;
    u32 mCurrentHold;
};

#pragma scheduling off
inline std::tr1::shared_ptr<CPiiPersonalData> CPlayer::GetPersonalData() const {
    return mState.mPersonalData;
}

inline std::tr1::shared_ptr<CPiiPersonalData> CPlayer::GetPendingPersonalData() const {
    return mState.mPendingPersonalData;
}
#pragma scheduling reset

#endif
