#ifndef GAME_IENEMYINFO_H
#define GAME_IENEMYINFO_H

#include <game/core/ICalcDraw.h>
#include <memory>

class CPiiPersonalData;

class IEnemyInfo : public ICalcDraw {
public:
    virtual ~IEnemyInfo() {}
    virtual void Show(const std::tr1::shared_ptr<CPiiPersonalData>& rData, bool flash, int frames) = 0;
    virtual void Hide() = 0;
};

#endif
