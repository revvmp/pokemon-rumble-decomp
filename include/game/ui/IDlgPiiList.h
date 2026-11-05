#ifndef GAME_IDLGPIILIST_H
#define GAME_IDLGPIILIST_H

#include <game/ui/IDlg.h>
#include <memory>

class IDlgPiiList : public IDlg {
public:
    virtual ~IDlgPiiList() {}
    virtual int GetType() = 0;
    virtual bool Show(int player, int type) = 0;

    static std::tr1::shared_ptr<IDlgPiiList> Create(bool flag);
};

#endif
