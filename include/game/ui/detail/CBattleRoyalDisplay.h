#ifndef GAME_DETAIL_CBATTLEROYALDISPLAY_H
#define GAME_DETAIL_CBATTLEROYALDISPLAY_H

#include <game/ui/CDisplayPhaseCtrl.h>
#include <game/ui/CLayout.h>
#include <game/ui/IBattleRoyalDisplay.h>
#include <game/world/IGameMap.h>
#include <game/ui/LayoutResources.h>
#include <game/text/Message.h>
#include <game/text/StringFormat.h>
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
