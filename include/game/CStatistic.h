#ifndef GAME_CSTATISTIC_H
#define GAME_CSTATISTIC_H

#include <game/SaveData.h>
#include <memory>
#include <vector>
#include <map>

class CStatistic {
public:
    static std::tr1::shared_ptr<CStatistic> Create(const std::vector<SD_StatisticValue::Data>& rData1, const std::vector<SD_StatisticValue::Data>& rData2,
                                                   const std::vector<SD_StatisticValue::Data>& rData3);
    CStatistic();
    int Get(int id);
    void Set(int id, int value);
    void Add(int id, int amount);
    void SaveSnapshot();
    void RestoreSnapshot();
    void SetCounterStopValue(int id, int value);
    bool IsWazaUsed(int waza);
    void SetWazaUsed(int waza, bool used);
    int GetWazaUsedCount();
    void ExportStatisticValues(std::vector<SD_StatisticValue::Data>& rOut1, std::vector<SD_StatisticValue::Data>& rOut2,
                               std::vector<SD_StatisticValue::Data>& rOut3);
private:
    std::vector<SD_StatisticValue::Data> mSystemValues;
    std::vector<SD_StatisticValue::Data> mSystemSnapshot;
    std::vector<SD_StatisticValue::Data> mValues;
    std::vector<SD_StatisticValue::Data> mSnapshot;
    std::vector<SD_StatisticValue::Data> mWazaBits;
    std::vector<SD_StatisticValue::Data> mWazaSnapshot;
    u32 mCounterSnapshot[2];
    std::map<int, int> mCounterStops;
};

#endif
