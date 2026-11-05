#ifndef GAME_CPIILISTFILTERSORT_H
#define GAME_CPIILISTFILTERSORT_H

#include <game/pokemon/CPiiPersonalData.h>
#include <functional>
#include <memory>
#include <vector>

class CUIManager;
class CUIList;
class BUI;
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
    bool ShowSortMenu(std::tr1::shared_ptr<CUIManager> pManager, int player, std::vector<std::tr1::shared_ptr<CUIPiiListItem> >& rItems,
                      std::tr1::function<void(const CPiiListFilterSort*)> onChanged);

private:
    bool ShowFilterMenu(std::tr1::shared_ptr<CUIManager> pManager, int player, std::vector<std::tr1::shared_ptr<CUIPiiListItem> >& rItems,
                        const std::tr1::function<void(const CPiiListFilterSort*)>& onChanged);
    bool ShowTypeFilterMenu(bool bySpecies, std::tr1::shared_ptr<CUIManager> pManager, int player,
                            std::vector<std::tr1::shared_ptr<CUIPiiListItem> >& rItems,
                            const std::tr1::function<void(const CPiiListFilterSort*)>& onChanged);
    bool ShowPrefixFilterMenu(std::tr1::shared_ptr<CUIManager> pManager, int player, std::vector<std::tr1::shared_ptr<CUIPiiListItem> >& rItems,
                              const std::tr1::function<void(const CPiiListFilterSort*)>& onChanged);
    bool ShowOriginalTrainerFilterMenu(std::tr1::shared_ptr<CUIManager> pManager, int player,
                                       const std::tr1::function<void(const CPiiListFilterSort*)>& onChanged);
    bool ShowSortTypeMenu(std::tr1::shared_ptr<CUIManager> pManager, int player,
                          const std::tr1::function<void(const CPiiListFilterSort*)>& onChanged);
    void PreviewTypeFilter(BUI* pBui, FilterType filterType, const std::tr1::function<void(const CPiiListFilterSort*)>& onChanged);
    void PreviewPrefixFilter(BUI* pBui, const std::tr1::function<void(const CPiiListFilterSort*)>& onChanged);
    void PreviewOriginalTrainerFilter(BUI* pBui, const std::tr1::function<void(const CPiiListFilterSort*)>& onChanged);
    void PreviewSortType(BUI* pBui, const std::tr1::function<void(const CPiiListFilterSort*)>& onChanged);
    void Sort(std::vector<std::tr1::shared_ptr<CUIPiiListItem> >& rItems, int player);

    FilterType mFilterType;
    int mTypeIndex;
    int mPrefix;
    int mInvertOriginalTrainer;
    int mSortType;
    std::tr1::shared_ptr<CPiiPersonalData> mpSelectedPii;
    int mPlayerIndex;
};

#endif
