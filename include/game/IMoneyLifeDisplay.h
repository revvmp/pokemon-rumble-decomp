#ifndef GAME_IMONEYLIFEDISPLAY_H
#define GAME_IMONEYLIFEDISPLAY_H

#include <game/ICalcDraw.h>
#include <memory>

struct MapDesc;

class IMoneyLifeDisplay : public ICalcDraw {
public:
    virtual ~IMoneyLifeDisplay() {}
    virtual void SetForceVisible(bool visible) = 0;
    virtual void Show() = 0;
    virtual void Hide() = 0;

    static std::tr1::shared_ptr<IMoneyLifeDisplay> Create(MapDesc* pMapDesc, bool background);
};

#endif
