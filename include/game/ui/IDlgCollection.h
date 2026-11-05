#ifndef GAME_IDLGCOLLECTION_H
#define GAME_IDLGCOLLECTION_H

#include <game/ui/IDlg.h>
#include <memory>

class IDlgCollection : public IDlg {
public:
    virtual ~IDlgCollection() {}
    virtual bool IsShown() = 0;
    virtual bool Show(int player) = 0;

    static std::tr1::shared_ptr<IDlgCollection> Create(bool flag);
};

#endif
