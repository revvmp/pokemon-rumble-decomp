#ifndef GAME_CBATTLEROYALBOX_H
#define GAME_CBATTLEROYALBOX_H

#include <memory>
#include <revolution/types.h>

class CBattleRoyalData {
public:
    bool UpdateBestTime(f32 time);

private:
    f32 mBestTime;
};

class CBattleRoyalBox {
public:
    std::tr1::shared_ptr<CBattleRoyalData> GetData(int mode, int area);
};

#endif
