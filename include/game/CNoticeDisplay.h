#ifndef GAME_CNOTICEDISPLAY_H
#define GAME_CNOTICEDISPLAY_H

#include <game/ICalcDraw.h>
#include <memory>
#include <revolution/types.h>

struct PiiSpeciesData;

class INoticeDisplay : public ICalcDraw {
public:
    virtual ~INoticeDisplay() {}
    virtual bool IsShown() const = 0;
    virtual void Show(int type) = 0;
    virtual void Close() = 0;
    virtual void AddEnemy(const PiiSpeciesData* pSpecies, int value) = 0;
    virtual void SetWinner(int winner) = 0;
    virtual void SetWinnerParam(f32 param) = 0;
    virtual int GetType() const = 0;
};

std::tr1::shared_ptr<INoticeDisplay> CreateNoticeDisplay(int);

#endif
