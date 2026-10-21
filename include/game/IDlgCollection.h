#ifndef GAME_IDLGCOLLECTION_H
#define GAME_IDLGCOLLECTION_H

#include <game/IDlg.h>
#include <memory>

class IDlgCollection : public IDlg {
public:
    virtual ~IDlgCollection() {}

    static std::tr1::shared_ptr<IDlgCollection> Create(bool flag);
};

#endif
