#include <game/io/CStatistic.h>
#include <algorithm>
#include <limits.h>

namespace {

void ExpandTo(std::vector<SD_StatisticValue::Data>& rValues, u32 index) {
    u32 oldSize = rValues.size();
    if (oldSize <= index) {
        rValues.resize(index + 1);
        for (u32 i = oldSize; i < index; i++) {
            rValues[i].mValue = 0;
        }
    }
}

}

CStatistic::CStatistic() {}

int CStatistic::Get(int id) {
    if (id < 0) {
        if (-100 <= id && id < -97) {
            u32 index = id + 100;
            if (index < mSystemValues.size()) {
                return mSystemValues[index].mValue;
            }
            return 0;
        }
        if (id == -200) {
            return GetSaveData(-1)->mData.mCounterFirst;
        }
        if (id == -199) {
            return GetSaveData(-1)->mData.mCounterSecond;
        }
        return 0;
    }
    if (static_cast<u32>(id) < mValues.size()) {
        return mValues[id].mValue;
    }
    return 0;
}

void CStatistic::Set(int id, int value) {
    long stop;
    long limited = value;
    stop = GetCounterStopValue(id);
    limited = std::min(stop, limited);
    if (id < 0) {
        if (-100 <= id && id < -97) {
            u32 index = id + 100;
            ExpandTo(mSystemValues, index);
            mSystemValues[index].mValue = limited;
        } else if (id == -200) {
            GetSaveData(-1)->mData.mCounterFirst = limited;
        } else if (id == -199) {
            GetSaveData(-1)->mData.mCounterSecond = limited;
        } else {
            return;
        }
    } else {
        ExpandTo(mValues, id);
        mValues[id].mValue = limited;
    }
}

void CStatistic::Add(int id, int amount) {
    int current = Get(id);
    long limit = INT_MAX;
    long result;
    if (amount > 0) {
        int sum = current + amount;
        if (current < sum) {
            result = sum;
        } else {
            result = limit;
        }
    } else if (amount < 0) {
        int sum = current + amount;
        if (current > sum) {
            result = std::max<long>(sum, limit);
        } else {
            result = INT_MIN;
        }
    } else {
        result = current;
    }
    Set(id, result);
}

void CStatistic::SetCounterStopValue(int id, int value) {
    mCounterStops[id] = value;
}

int CStatistic::GetCounterStopValue(int id) const {
    std::map<int, int>::iterator it = mCounterStops.find(id);
    return it != mCounterStops.end() ? it->second : INT_MAX;
}

bool CStatistic::IsWazaUsed(int waza) {
    u32 word = waza / 32;
    u32 mask = 1 << (waza % 32);
    if (word < mWazaBits.size()) {
        return (mWazaBits[word].mValue & mask) != 0;
    }
    return false;
}

void CStatistic::SetWazaUsed(int waza, bool used) {
    if (waza != 0) {
        u32 word = waza / 32;
        u32 mask = 1 << (waza % 32);
        ExpandTo(mWazaBits, word);
        if (used) {
            mWazaBits[word].mValue |= mask;
        } else {
            mWazaBits[word].mValue = mValues[word].mValue & ~mask;
        }
    }
}

int CStatistic::GetWazaUsedCount() {
    int count = 0;
    for (std::vector<SD_StatisticValue::Data>::iterator it = mWazaBits.begin(); it != mWazaBits.end(); ++it) {
        u32 bits = it->mValue;
        if (bits != 0) {
            for (int i = 0; i < 32; i++) {
                if (bits & (1 << i)) {
                    count++;
                }
            }
        }
    }
    return count;
}

void CStatistic::SaveSnapshot() {
    mSystemSnapshot = mSystemValues;
    mSnapshot = mValues;
    mWazaSnapshot = mWazaBits;
    mCounterSnapshot[0] = GetSaveData(-1)->mData.mCounterFirst;
    mCounterSnapshot[1] = GetSaveData(-1)->mData.mCounterSecond;
}

void CStatistic::RestoreSnapshot() {
    mSystemValues = mSystemSnapshot;
    mValues = mSnapshot;
    mWazaBits = mWazaSnapshot;
    GetSaveData(-1)->mData.mCounterFirst = mCounterSnapshot[0];
    GetSaveData(-1)->mData.mCounterSecond = mCounterSnapshot[1];
}

void CStatistic::ExportStatisticValues(std::vector<SD_StatisticValue::Data>& rOut1, std::vector<SD_StatisticValue::Data>& rOut2,
                                       std::vector<SD_StatisticValue::Data>& rOut3) {
    rOut1 = mSystemValues;
    rOut2 = mValues;
    rOut3 = mWazaBits;
}

std::tr1::shared_ptr<CStatistic> CStatistic::Create(const std::vector<SD_StatisticValue::Data>& rData1, const std::vector<SD_StatisticValue::Data>& rData2,
                                                    const std::vector<SD_StatisticValue::Data>& rData3) {
    std::tr1::shared_ptr<CStatistic> statistic(new CStatistic);
    statistic->mSystemValues = rData1;
    statistic->mValues = rData2;
    statistic->mWazaBits = rData3;
    return statistic;
}
