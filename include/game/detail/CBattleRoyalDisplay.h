#ifndef GAME_DETAIL_CBATTLEROYALDISPLAY_H
#define GAME_DETAIL_CBATTLEROYALDISPLAY_H

#include <game/CDisplayPhaseCtrl.h>
#include <game/CLayout.h>
#include <game/IBattleRoyalDisplay.h>
#include <game/IGameMap.h>
#include <game/LayoutResources.h>
#include <game/Message.h>
#include <game/StringFormat.h>
#include <algorithm>

namespace {
class CBattleRoyalDisplay : public IBattleRoyalDisplay {
public:
    CBattleRoyalDisplay() : mTimePhase(nullptr), mEnemyPhase(nullptr), mEnemyCount(0), mFinished(false) {}
    virtual void Finish();
    virtual void Calc();
    virtual void Draw();
    virtual void Show();
    virtual void Hide();
    virtual void SetEnemyCount(int count);
    virtual void DecEnemyCount();
    virtual int GetEnemyCount();

    void Load(const std::tr1::shared_ptr<CLayout::FontMap>& rFonts, bool background);

private:
    CLayout mLayout;
    CDisplayPhaseCtrl mTimePhase;
    CDisplayPhaseCtrl mEnemyPhase;
    int mEnemyCount;
    bool mFinished;
};
}

#endif
