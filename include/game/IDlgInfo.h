#ifndef GAME_IDLGINFO_H
#define GAME_IDLGINFO_H

#include <game/IDlg.h>
#include <memory>

class IDlgInfo : public IDlg {
public:
    virtual ~IDlgInfo() {}

    static std::tr1::shared_ptr<IDlgInfo> Create(bool flag);
};

#endif
