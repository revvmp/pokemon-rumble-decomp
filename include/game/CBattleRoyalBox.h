#ifndef GAME_CBATTLEROYALBOX_H
#define GAME_CBATTLEROYALBOX_H

#include <memory>
#include <revolution/types.h>
#include <game/SaveData.h>
#include <vector>

class CBattleRoyalData {
public:
    explicit CBattleRoyalData(f32 time = -1.0f) : mBestTime(time) {}
    ~CBattleRoyalData() {}

    bool UpdateBestTime(f32 time);
    f32 GetBestTime() const { return mBestTime; }

private:
    f32 mBestTime;
};

class CBattleRoyalBox {
public:
    static std::tr1::shared_ptr<CBattleRoyalBox> Create(const std::vector<SD_BattleRoyalData::Data>& rData);

    explicit CBattleRoyalBox(const std::vector<SD_BattleRoyalData::Data>& rData);
    std::tr1::shared_ptr<CBattleRoyalData> GetData(int mode, int area) const;
    void ExportBattleRoyalData(std::vector<SD_BattleRoyalData::Data>& rOut);

private:
    std::tr1::shared_ptr<CBattleRoyalData> mData[3][5];
};

std::tr1::shared_ptr<CBattleRoyalData> CreateBattleRoyalData(const f32& rTime);

#endif
