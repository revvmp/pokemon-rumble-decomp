#ifndef GAME_IBATTLEROYALDISPLAY_H
#define GAME_IBATTLEROYALDISPLAY_H

#include <game/ICalcDraw.h>
#include <memory>

class IBattleRoyalDisplay : public ICalcDraw {
public:
    virtual ~IBattleRoyalDisplay() {}
    virtual void Show() = 0;
    virtual void Hide() = 0;
    virtual void Finish() = 0;
    virtual void SetRound(int round) = 0;
    virtual int GetRoundIndex() = 0;
    virtual int GetRound() = 0;

    static std::tr1::shared_ptr<IBattleRoyalDisplay> Create(bool flag);
};

#endif
