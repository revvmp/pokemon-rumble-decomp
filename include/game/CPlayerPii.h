#ifndef GAME_CPLAYERPII_H
#define GAME_CPLAYERPII_H

#include <deque>
#include <game/CPii.h>
#include <nw4r/math/math_types.h>

class CPlayerPii : public CPii {
public:
    enum { TYPE = 1 << 7 };

    std::deque<nw4r::math::VEC3>& GetPositionHistory() { return mPositionHistory; }

private:
    int mPlayerNo;
    std::deque<nw4r::math::VEC3> mPositionHistory;
};

#endif
