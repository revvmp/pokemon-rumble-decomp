#ifndef GAME_CSTATISTIC_H
#define GAME_CSTATISTIC_H

#include <game/SaveData.h>
#include <memory>
#include <vector>

class CStatistic {
public:
    static std::tr1::shared_ptr<CStatistic> Create(const std::vector<SD_StatisticValue::Data>& rData1, const std::vector<SD_StatisticValue::Data>& rData2,
                                                   const std::vector<SD_StatisticValue::Data>& rData3);
    void Add(int id, int amount);
    void ExportStatisticValues(std::vector<SD_StatisticValue::Data>& rOut1, std::vector<SD_StatisticValue::Data>& rOut2,
                               std::vector<SD_StatisticValue::Data>& rOut3);
};

#endif
