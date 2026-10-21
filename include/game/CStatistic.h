#ifndef GAME_CSTATISTIC_H
#define GAME_CSTATISTIC_H

#include <game/SaveData.h>
#include <vector>

class CStatistic {
public:
    void ExportStatisticValues(std::vector<SD_StatisticValue::Data>& rOut1, std::vector<SD_StatisticValue::Data>& rOut2,
                               std::vector<SD_StatisticValue::Data>& rOut3);
};

#endif
