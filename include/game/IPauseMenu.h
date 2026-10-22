#ifndef GAME_IPAUSEMENU_H
#define GAME_IPAUSEMENU_H

#include <game/ICalcDraw.h>
#include <memory>

struct MapDesc;

class IPauseMenu : public ICalcDraw {
public:
    virtual ~IPauseMenu() {}
    virtual bool IsOpen() = 0;
    virtual bool TakeScreenshot() = 0;

    static std::tr1::shared_ptr<IPauseMenu> Create(const MapDesc* pDesc, bool hideBackground, int background);
};

#endif
