#ifndef GAME_ICOMBODISPLAY_H
#define GAME_ICOMBODISPLAY_H

#include <game/core/ICalcDraw.h>
#include <memory>

class IComboDisplay : public ICalcDraw {
public:
    virtual ~IComboDisplay() {}
    virtual int GetComboGauge() const = 0;
    virtual void SetComboGauge(int gauge) = 0;
    virtual int GetComboCount() const = 0;
    virtual void SetComboCount(int count) = 0;
    virtual void ShowComboCount() = 0;
};

std::tr1::shared_ptr<IComboDisplay> CreateComboDisplay(bool background);

#endif
