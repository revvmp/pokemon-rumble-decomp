#ifndef GAME_CPIILISTFILTERSORT_H
#define GAME_CPIILISTFILTERSORT_H

#include <game/CPiiPersonalData.h>
#include <functional>
#include <memory>
#include <vector>

class CUIManager;
class CUIList;
class CUIPiiListItem;

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

    void Filter(std::tr1::shared_ptr<CUIList> pList, std::vector<std::tr1::shared_ptr<CUIPiiListItem> >& rItems, int player);
    void ShowSortMenu(std::tr1::shared_ptr<CUIManager> pManager, int player, std::vector<std::tr1::shared_ptr<CUIPiiListItem> >& rItems,
                      std::tr1::function<void(const CPiiListFilterSort*)> onChanged);

private:
    void Sort(std::vector<std::tr1::shared_ptr<CUIPiiListItem> >& rItems, int player);

    int mFilterType;
    int mTypeIndex;
    int mPrefix;
    int mInvertOriginalTrainer;
    int mSortType;
    std::tr1::shared_ptr<CPiiPersonalData> mpSelectedPii;
    int mPlayerIndex;
};

#endif
