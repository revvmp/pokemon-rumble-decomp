#ifndef GAME_CBATTLEROYALDATA_H
#define GAME_CBATTLEROYALDATA_H

#include <game/SaveData.h>
#include <vector>

class CBattleRoyalData {
public:
    void ExportBattleRoyalData(std::vector<SD_BattleRoyalData::Data>& rOut);
};

#endif
