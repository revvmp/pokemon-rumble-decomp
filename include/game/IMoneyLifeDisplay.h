#ifndef GAME_IMONEYLIFEDISPLAY_H
#define GAME_IMONEYLIFEDISPLAY_H

#include <game/ICalcDraw.h>
#include <memory>

struct MapDesc;

class IMoneyLifeDisplay : public ICalcDraw {
public:
    virtual ~IMoneyLifeDisplay() {}

    static std::tr1::shared_ptr<IMoneyLifeDisplay> Create(MapDesc* pMapDesc, bool flag);
};

#endif
