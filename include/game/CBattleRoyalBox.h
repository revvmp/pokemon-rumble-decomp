#ifndef GAME_CBATTLEROYALBOX_H
#define GAME_CBATTLEROYALBOX_H

#include <memory>
#include <revolution/types.h>
#include <game/SaveData.h>
#include <vector>

class CBattleRoyalData {
public:
    bool UpdateBestTime(f32 time);
    f32 GetBestTime() const { return mBestTime; }

private:
    f32 mBestTime;
};

class CBattleRoyalBox {
public:
    std::tr1::shared_ptr<CBattleRoyalData> GetData(int mode, int area);
    void ExportBattleRoyalData(std::vector<SD_BattleRoyalData::Data>& rOut);
};

#endif
