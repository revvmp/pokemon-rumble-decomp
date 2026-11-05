#ifndef GAME_DETAIL_CLAYOUTMANAGER_H
#define GAME_DETAIL_CLAYOUTMANAGER_H

#include <game/ui/ILayoutManager.h>
#include <game/ui/CLayout.h>
#include <map>
#include <memory>

class CLayoutManager : public ILayoutManager {
public:
    CLayoutManager() : mNextID(1) {}
    virtual void Calc();
    virtual void Draw();
    virtual int Add(const std::tr1::shared_ptr<CLayout>& rLayout);
    virtual std::tr1::shared_ptr<CLayout> Get(int id);
    virtual bool Remove(int id);

private:
    std::map<int, std::tr1::shared_ptr<CLayout> > mLayouts;
    int mNextID;
};

#endif
