#ifndef GAME_DETAIL_CLAYOUTMANAGER_H
#define GAME_DETAIL_CLAYOUTMANAGER_H

#include <game/ILayoutManager.h>
#include <map>
#include <memory>

class CLayoutManager : public ILayoutManager {
public:
    CLayoutManager() : mNextID(1) {}
    virtual void Calc();
    virtual void Draw();
    virtual int Add(const std::tr1::shared_ptr<ICalcDraw>& rLayout);
    virtual std::tr1::shared_ptr<ICalcDraw> Get(int id);
    virtual bool Remove(int id);

private:
    std::map<int, std::tr1::shared_ptr<ICalcDraw> > mLayouts;
    int mNextID;
};

#endif
