#ifndef GAME_CPLAYER_H
#define GAME_CPLAYER_H

#include <game/CPiiBox.h>
#include <game/ICalc.h>
#include <memory>
#include <revolution/types.h>

class BWorldObj;
class CPiiPersonalData;
class CPlayerPii;
class IController;

class CPlayer : public ICalc {
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
    bool IsJoined() const;
    void SetPii(CPlayerPii* pPii);
    CPlayerPii* GetPii();
    IController* GetController();
    void SetPendingPersonalData(const std::tr1::shared_ptr<CPiiPersonalData>& rPersonalData);
    void ResetPendingPersonalData();
    void CompletePiiChangeRequest();
    void BeginHoldCollection();
    void EndHoldCollection();

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
    u32 mReserved;
    u32 mAccumulatedHold;
    u32 mCurrentHold;
};

#endif
