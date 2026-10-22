#ifndef GAME_ILAYOUTMANAGER_H
#define GAME_ILAYOUTMANAGER_H

#include <game/ICalcDraw.h>
#include <memory>

class ILayoutManager : public ICalcDraw {
public:
    virtual ~ILayoutManager() {}

    virtual int Add(const std::tr1::shared_ptr<ICalcDraw>& rLayout) = 0;
    virtual std::tr1::shared_ptr<ICalcDraw> Get(int id) = 0;
    virtual bool Remove(int id) = 0;

    static std::tr1::shared_ptr<ILayoutManager> Create();
};

#endif
