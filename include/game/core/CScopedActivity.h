#ifndef GAME_CSCOPEDACTIVITY_H
#define GAME_CSCOPEDACTIVITY_H

namespace NResetMgr {
class CDisable {
public:
    CDisable();
    ~CDisable();
};
}

typedef NResetMgr::CDisable CScopedActivity;

#endif
