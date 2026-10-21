#ifndef GAME_ILAYOUTMANAGER_H
#define GAME_ILAYOUTMANAGER_H

#include <game/ICalcDraw.h>
#include <memory>

class ILayoutManager : public ICalcDraw {
public:
    virtual ~ILayoutManager() {}

    static std::tr1::shared_ptr<ILayoutManager> Create();
};

#endif
