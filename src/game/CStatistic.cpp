#include <game/CStatistic.h>
#include <algorithm>
#include <limits.h>

namespace {

void ExpandTo(std::vector<SD_StatisticValue::Data>& rValues, u32 index) {
    u32 oldSize = rValues.size();
    if (index >= oldSize) {
        rValues.resize(index + 1);
        for (u32 i = oldSize; i < index; i++) {
            rValues[i].mValue = 0;
        }
    }
}

}

CStatistic::CStatistic() {}

int CStatistic::Get(int id) {
    if (id < -97) {
        if (id >= -100) {
            u32 index = id + 100;
            if (index < mSystemValues.size()) {
                return mSystemValues[index].mValue;
            }
            return 0;
        }
        if (id == -200) {
            return GetSaveData(-1)->mData.mCounters[0];
        }
        if (id == -199) {
            return GetSaveData(-1)->mData.mCounters[1];
        }
        return 0;
    }
    if (id >= 0) {
        if (static_cast<u32>(id) < mValues.size()) {
            return mValues[id].mValue;
        }
    }
    return 0;
}

int CStatistic::GetCounterStopValue(int id) const {
    std::map<int, int>::iterator it = mCounterStops.find(id);
    if (it != mCounterStops.end()) {
        return it->second;
    }
    return INT_MAX;
}

bool CStatistic::IsWazaUsed(int waza) {
    u32 word = waza / 32;
    u32 mask = 1 << (waza % 32);
    if (word < mWazaBits.size()) {
        return (mWazaBits[word].mValue & mask) != 0;
    }
    return false;
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

void CStatistic::Set(int id, int value) {
    value = std::min<long>(GetCounterStopValue(id), value);
    if (id < -97) {
        if (id >= -100) {
            id += 100;
            ExpandTo(mSystemValues, id);
            mSystemValues[id].mValue = value;
        } else if (id == -200) {
            GetSaveData(-1)->mData.mCounters[0] = value;
        } else if (id == -199) {
            GetSaveData(-1)->mData.mCounters[1] = value;
        }
    } else if (id >= 0) {
        ExpandTo(mValues, id);
        mValues[id].mValue = value;
    }
}
