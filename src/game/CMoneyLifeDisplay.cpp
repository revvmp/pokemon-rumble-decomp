#include <memory>
#include <game/detail/CMoneyLifeDisplay.h>

std::tr1::shared_ptr<IMoneyLifeDisplay> IMoneyLifeDisplay::Create(MapDesc* __restrict pMapDesc, bool background) {
    std::tr1::shared_ptr<CMoneyLifeDisplay> display(new CMoneyLifeDisplay(pMapDesc));
    if (display->Initialize(background)) return display;
    return std::tr1::shared_ptr<IMoneyLifeDisplay>();
}

namespace {

inline CMoneyLifeDisplay::CMoneyLifeDisplay(const MapDesc* pMapDesc) :
    mMode(GetMoneyLifeDisplayMode(pMapDesc)), mPhase(nullptr), mShownMoney(-1), mShownLife(-1) {}

}
