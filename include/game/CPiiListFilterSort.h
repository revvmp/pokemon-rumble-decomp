#ifndef GAME_CPIILISTFILTERSORT_H
#define GAME_CPIILISTFILTERSORT_H

#include <game/CPiiPersonalData.h>
#include <memory>

class CPiiListFilterSort {
public:
    enum FilterType {
        FILTER_NONE,
        FILTER_SPECIES_TYPE,
        FILTER_MOVE_TYPE,
        FILTER_PREFIX,
        FILTER_ORIGINAL_TRAINER
    };

    CPiiListFilterSort();
    ~CPiiListFilterSort();

private:
    int mFilterType;
    int mTypeIndex;
    int mPrefix;
    int mInvertOriginalTrainer;
    int mSortType;
    std::tr1::shared_ptr<CPiiPersonalData> mpSelectedPii;
    int mPlayerIndex;
};

#endif
