#include <game/CPiiListFilterSort.h>
#include <game/CMsgRes.h>
#include <game/CUIPiiListItem.h>
#include <game/CUIList.h>
#include <game/IGameSession.h>
#include <game/CPlayer.h>
#include <game/COyaInfo.h>
#include <game/CUIMenu.h>
#include <game/CUIMenuItem.h>
#include <game/IGameMap.h>
#include <game/IPauseMenu.h>
#include <game/LayoutResources.h>
#include <game/Message.h>
#include <game/Sound.h>
#include <game/Font.h>
#include <game/CFiber.h>
#include <game/SystemMessage.h>
#include <game/IController.h>
#include <algorithm>
#include <vector>

typedef std::tr1::shared_ptr<CUIPiiListItem> PiiItemPtr;
typedef std::vector<PiiItemPtr> PiiItemVector;

template <typename T>
int CompareValue(T a, T b) {
    if (a < b) {
        return -1;
    }
    return b < a;
}

template <typename T>
int Compare(const T& rA, const T& rB) {
    if (rA < rB) {
        return -1;
    }
    return rB < rA;
}

bool CompareBPAscending(const PiiItemPtr& rA, const PiiItemPtr& rB) {
    int bpA = rA->GetPPD()->GetBP();
    int bpB = rB->GetPPD()->GetBP();
    const CPiiPersonalData* pA = rA->GetPPD().get();
    const CPiiPersonalData* pB = rB->GetPPD().get();
    int result = CompareValue(bpA, bpB);
    if (result != 0) {
        return result < 0;
    }
    result = Compare(pA->GetCreationTime(), pB->GetCreationTime());
    if (result != 0) {
        return result < 0;
    }
    return pA->GetId() < pB->GetId();
}

bool CompareBPDescending(const PiiItemPtr& rA, const PiiItemPtr& rB) {
    int bpA = rA->GetPPD()->GetBP();
    int bpB = rB->GetPPD()->GetBP();
    const CPiiPersonalData* pA = rA->GetPPD().get();
    const CPiiPersonalData* pB = rB->GetPPD().get();
    int result = CompareValue(bpA, bpB);
    if (result != 0) {
        return result > 0;
    }
    result = Compare(pA->GetCreationTime(), pB->GetCreationTime());
    if (result != 0) {
        return result > 0;
    }
    return pB->GetId() < pA->GetId();
}

inline int Reverse(int result) {
    return result != 0 ? -result : 0;
}

inline bool CompareBPChain(const CPiiPersonalData* pA, const CPiiPersonalData* pB) {
    int result = CompareValue(pA->GetBP(), pB->GetBP());
    if (result != 0) {
        return result > 0;
    }
    result = CompareValue(pA->GetCreationTime(), pB->GetCreationTime());
    if (result != 0) {
        return result > 0;
    }
    return pB->GetId() < pA->GetId();
}

inline int CompareSpeciesNames(const PiiSpeciesData* pA, const PiiSpeciesData* pB) {
    if (pA == pB) {
        return 0;
    }
    const wchar_t* pNameA = pA->GetName();
    const wchar_t* pNameB = pB->GetName();
    if (pNameA == pNameB) {
        return 0;
    }
    return CompareCollation(pNameA, pNameB);
}

bool CompareFavoriteFirst(const PiiItemPtr& rA, const PiiItemPtr& rB) {
    const CPiiPersonalData* pA = rA->GetPPD().get();
    const CPiiPersonalData* pB = rB->GetPPD().get();
    int result = CompareValue(int(pA->IsSpecHidden()), int(pB->IsSpecHidden()));
    if (result != 0) {
        result = -result;
    } else {
        result = Reverse(CompareValue(int(pA->IsFavorite()), int(pB->IsFavorite())));
    }
    if (result != 0) {
        return result < 0;
    }
    if ((result = CompareValue(pA->GetBP(), pB->GetBP())) != 0 ||
        (result = CompareValue(pA->GetCreationTime(), pB->GetCreationTime())) != 0) {
        return result > 0;
    }
    return pB->GetId() < pA->GetId();
}

bool CompareSpeciesName(const PiiItemPtr& rA, const PiiItemPtr& rB) {
    const PiiSpeciesData* pSpeciesA = rA->GetPPD()->GetSpeciesData();
    const PiiSpeciesData* pSpeciesB = rB->GetPPD()->GetSpeciesData();
    int result = CompareSpeciesNames(pSpeciesA, pSpeciesB);
    if (result != 0) {
        return result < 0;
    }
    return CompareBPDescending(rA, rB);
}

inline bool CompareItemBP(const PiiItemPtr& rA, const PiiItemPtr& rB) {
    return CompareBPChain(rA->GetPPD().get(), rB->GetPPD().get());
}

bool CompareSpeciesOrder(const PiiItemPtr& rA, const PiiItemPtr& rB) {
    const PiiSpeciesData* pSpeciesA = rA->GetPPD()->GetSpeciesData();
    const PiiSpeciesData* pSpeciesB = rB->GetPPD()->GetSpeciesData();
    int result = Compare(pSpeciesA, pSpeciesB);
    if (result != 0) {
        return result < 0;
    }
    return CompareItemBP(rA, rB);
}

bool CompareNewest(const PiiItemPtr& rA, const PiiItemPtr& rB) {
    const CPiiPersonalData* pA = rA->GetPPD().get();
    const CPiiPersonalData* pB = rB->GetPPD().get();
    int result = Compare(pA->GetCreationTime(), pB->GetCreationTime());
    if (result != 0) {
        return result > 0;
    }
    return pB->GetId() < pA->GetId();
}

bool CompareOldest(const PiiItemPtr& rA, const PiiItemPtr& rB) {
    const CPiiPersonalData* pA = rA->GetPPD().get();
    const CPiiPersonalData* pB = rB->GetPPD().get();
    int result = Compare(pA->GetCreationTime(), pB->GetCreationTime());
    if (result != 0) {
        return result < 0;
    }
    return pA->GetId() < pB->GetId();
}

bool IsNameBefore(const wchar_t* const& rNameA, const wchar_t* const& rNameB) {
    return CompareCollation(rNameA, rNameB) < 0;
}

CPiiListFilterSort::CPiiListFilterSort() : mFilterType(FILTER_NONE), mSortType(0), mPlayerIndex(-1) {}

CPiiListFilterSort::~CPiiListFilterSort() {}

#pragma dont_inline on

void CPiiListFilterSort::Filter(std::tr1::shared_ptr<CUIList> pList, PiiItemVector& rItems, int player) {
    typedef PiiItemVector::iterator Iterator;
    Sort(rItems, player);
    std::vector<std::tr1::shared_ptr<IUIListItem> >& rList = pList->GetItems();
    rList.clear();
    switch (mFilterType) {
    default:
        for (Iterator it = rItems.begin(); it != rItems.end(); ++it) {
            rList.push_back(std::tr1::shared_ptr<IUIListItem>(*it));
        }
        break;
    case FILTER_SPECIES_TYPE: {
        u32 mask = 1 << mTypeIndex;
        for (Iterator it = rItems.begin(); it != rItems.end(); ++it) {
            if (mask & (*it)->GetSpeciesTypeMask()) {
                rList.push_back(std::tr1::shared_ptr<IUIListItem>(*it));
            }
        }
        break;
    }
    case FILTER_MOVE_TYPE: {
        u32 mask = 1 << mTypeIndex;
        for (Iterator it = rItems.begin(); it != rItems.end(); ++it) {
            if (mask & (*it)->GetMoveTypeMask()) {
                rList.push_back(std::tr1::shared_ptr<IUIListItem>(*it));
            }
        }
        break;
    }
    case FILTER_PREFIX:
        for (Iterator it = rItems.begin(); it != rItems.end(); ++it) {
            if (mPrefix == (*it)->GetPPD()->GetPrefix()) {
                rList.push_back(std::tr1::shared_ptr<IUIListItem>(*it));
            }
        }
        break;
    case FILTER_ORIGINAL_TRAINER: {
        u32 trainerId = IGameSession::GetInstance()->GetOyaInfo()->GetCount();
        if (mInvertOriginalTrainer == 0) {
            for (Iterator it = rItems.begin(); it != rItems.end(); ++it) {
                if (trainerId == (*it)->GetPPD()->GetOriginalTrainerId()) {
                    rList.push_back(std::tr1::shared_ptr<IUIListItem>(*it));
                }
            }
        } else {
            for (Iterator it = rItems.begin(); it != rItems.end(); ++it) {
                if (trainerId != (*it)->GetPPD()->GetOriginalTrainerId()) {
                    rList.push_back(std::tr1::shared_ptr<IUIListItem>(*it));
                }
            }
        }
        break;
    }
    }
}

void CPiiListFilterSort::Sort(PiiItemVector& rItems, int player) {
    typedef bool (*Comparator)(const PiiItemPtr&, const PiiItemPtr&);
    Comparator comparators[7] = {CompareFavoriteFirst, CompareBPDescending, CompareBPAscending, CompareNewest, CompareOldest,
                                 CompareSpeciesName, CompareSpeciesOrder};
    std::sort(rItems.begin(), rItems.end(), comparators[mSortType]);
    if (mSortType == 0 && player >= 0 && rItems.size() != 0) {
        CPlayer* pPlayer = IGameSession::GetInstance()->GetPlayer(player);
        bool hasPersonalData = pPlayer->IsJoined() && pPlayer->GetPersonalData();
        if (hasPersonalData) {
            typedef PiiItemVector::iterator Iterator;
            PiiItemVector sorted;
            sorted.reserve(rItems.size());
            sorted.push_back(PiiItemPtr(0));
            for (Iterator it = rItems.begin(); it != rItems.end(); ++it) {
                if ((*it)->GetPPD() == pPlayer->GetPersonalData()) {
                    sorted[0] = *it;
                } else {
                    sorted.push_back(*it);
                }
            }
            if (sorted.size() == rItems.size()) {
                rItems.swap(sorted);
            }
        }
    }
}
#pragma dont_inline reset

bool CPiiListFilterSort::ShowSortMenu(std::tr1::shared_ptr<CUIManager> pManager, int player, PiiItemVector& rItems,
                                      std::tr1::function<void(const CPiiListFilterSort*)> onChanged) {
    UIMenuItem items[3] = {{nullptr, 0}, {nullptr, 1}, {nullptr, 2}};
    items[0].mText = GetMessageString(0x600A8, nullptr);
    items[1].mText = GetMessageString(0x600A9, nullptr);
    items[2].mText = GetMessageString(0x600AA, nullptr);
    UIMenuParam param = {nullptr, nullptr, 3, 1, 3, 0, nullptr, "menu"};
    param.mTitle = GetMessageString(0x600A7, nullptr);
    param.mpItems = items;
    param.mpArchives = gSharedLayoutArchives;
    CUIMenuSoundCallback callbacks(0);
    PlaySound(9);

    while (true) {
        IController* pInput = IGameSession::GetInstance()->GetPlayer(player)->GetController();
        int choice = RunUIMenu(param, callbacks, pInput, pManager, GetDefaultFontMap(), true);
        switch (choice) {
        case 0:
            if (ShowFilterMenu(pManager, player, rItems, onChanged)) {
                return true;
            }
            param.mDefaultIndex = 0;
            break;
        case 1:
            if (ShowSortTypeMenu(pManager, player, onChanged)) {
                return true;
            }
            param.mDefaultIndex = 1;
            break;
        case 2:
            GetCurrentGameMap()->GetPauseMenu()->TakeScreenshot();
            return false;
        default:
            return false;
        }
        YieldFiber();
    }
}

bool HasPrefixedItem(const PiiItemVector& rItems) {
    const PiiItemPtr* first = &*rItems.begin();
    const PiiItemPtr* end = first + rItems.size();
    for (const PiiItemPtr* it = first; it != end; ++it) {
        if ((*it)->GetPPD()->GetPrefix() != 0) {
            return true;
        }
    }
    return false;
}

bool HasItemWithPrefix(const PiiItemVector& rItems, int prefix) {
    const PiiItemPtr* first = &*rItems.begin();
    const PiiItemPtr* end = first + rItems.size();
    for (const PiiItemPtr* it = first; it != end; ++it) {
        if ((*it)->GetPPD()->GetPrefix() == prefix) {
            return true;
        }
    }
    return false;
}

bool HasMixedOriginalTrainers(const PiiItemVector& rItems) {
    u32 trainerId = IGameSession::GetInstance()->GetOyaInfo()->GetCount();
    const PiiItemPtr* end = &*rItems.begin() + rItems.size();
    const PiiItemPtr* it = &*rItems.begin();
    bool hasOwn = false;
    bool hasOther = false;
    for (; it != end; ++it) {
        if ((*it)->GetPPD()->GetOriginalTrainerId() == trainerId) {
            hasOwn = true;
            if (hasOther) {
                return true;
            }
        } else {
            hasOther = true;
            if (hasOwn) {
                return true;
            }
        }
    }
    return false;
}

#pragma dont_inline on

bool CPiiListFilterSort::ShowFilterMenu(std::tr1::shared_ptr<CUIManager> pManager, int player, PiiItemVector& rItems,
                                        const std::tr1::function<void(const CPiiListFilterSort*)>& onChanged) {
    UIMenuItem items[5] = {{nullptr, 0}, {nullptr, 1}, {nullptr, 0}, {nullptr, 0}, {nullptr, 2}};
    items[0].mText = GetMessageString(0x600AC, nullptr);
    items[1].mText = GetMessageString(0x600AD, nullptr);
    items[2].mText = GetMessageString(0x600AE, nullptr);
    items[2].mKind = HasPrefixedItem(rItems) ? 3 : -1;
    items[3].mText = GetMessageString(0x600AF, nullptr);
    items[3].mKind = HasMixedOriginalTrainers(rItems) ? 4 : -1;
    items[4].mText = GetMessageString(0x600B0, nullptr);
    UIMenuParam param = {nullptr, nullptr, 5, 1, 5, 0, nullptr, "menu"};
    param.mTitle = GetMessageString(0x600AB, nullptr);
    param.mpItems = items;
    param.mpArchives = gSharedLayoutArchives;
    switch (mFilterType) {
    default:
        param.mDefaultIndex = 2;
        break;
    case FILTER_SPECIES_TYPE:
        param.mDefaultIndex = 0;
        break;
    case FILTER_MOVE_TYPE:
        param.mDefaultIndex = 1;
        break;
    case FILTER_PREFIX:
        param.mDefaultIndex = 3;
        break;
    case FILTER_ORIGINAL_TRAINER:
        param.mDefaultIndex = 4;
        break;
    }
    CUIMenuSoundCallback callbacks(0);

    while (true) {
        IController* pInput = IGameSession::GetInstance()->GetPlayer(player)->GetController();
        int choice = RunUIMenu(param, callbacks, pInput, pManager, GetDefaultFontMap(), true);
        switch (choice) {
        case 0:
            if (ShowTypeFilterMenu(true, pManager, player, rItems, onChanged)) {
                return true;
            }
            param.mDefaultIndex = 0;
            break;
        case 1:
            if (ShowTypeFilterMenu(false, pManager, player, rItems, onChanged)) {
                return true;
            }
            param.mDefaultIndex = 1;
            break;
        case 3:
            if (ShowPrefixFilterMenu(pManager, player, rItems, onChanged)) {
                return true;
            }
            param.mDefaultIndex = 3;
            break;
        case 4:
            if (ShowOriginalTrainerFilterMenu(pManager, player, onChanged)) {
                return true;
            }
            param.mDefaultIndex = 4;
            break;
        case 2:
            mFilterType = FILTER_NONE;
            if (onChanged) {
                onChanged(this);
            }
            return true;
        default:
            return false;
        }
        YieldFiber();
    }
}

#pragma dont_inline reset

static inline bool HasItemWithSpeciesType(PiiItemVector& rItems, int kind) {
    u32 mask = 1 << kind;
    for (PiiItemVector::iterator it = rItems.begin(); it != rItems.end(); ++it) {
        if (mask & (*it)->GetSpeciesTypeMask()) {
            return true;
        }
    }
    return false;
}

static inline bool HasItemWithMoveType(PiiItemVector& rItems, int kind) {
    u32 mask = 1 << kind;
    for (PiiItemVector::iterator it = rItems.begin(); it != rItems.end(); ++it) {
        if (mask & (*it)->GetMoveTypeMask()) {
            return true;
        }
    }
    return false;
}

bool CPiiListFilterSort::ShowTypeFilterMenu(bool bySpecies, std::tr1::shared_ptr<CUIManager> pManager, int player, PiiItemVector& rItems,
                                            const std::tr1::function<void(const CPiiListFilterSort*)>& onChanged) {
    UIMenuItem items[17] = {{nullptr, 0},  {nullptr, 1},  {nullptr, 2},  {nullptr, 3},  {nullptr, 4},  {nullptr, 5},
                            {nullptr, 6},  {nullptr, 7},  {nullptr, 8},  {nullptr, 10}, {nullptr, 11}, {nullptr, 12},
                            {nullptr, 13}, {nullptr, 14}, {nullptr, 15}, {nullptr, 16}, {nullptr, 17}};
    items[0].mText = GetMessageString(0x40000, nullptr);
    items[1].mText = GetMessageString(0x40001, nullptr);
    items[2].mText = GetMessageString(0x40002, nullptr);
    items[3].mText = GetMessageString(0x40003, nullptr);
    items[4].mText = GetMessageString(0x40004, nullptr);
    items[5].mText = GetMessageString(0x40005, nullptr);
    items[6].mText = GetMessageString(0x40006, nullptr);
    items[7].mText = GetMessageString(0x40007, nullptr);
    items[8].mText = GetMessageString(0x40008, nullptr);
    items[9].mText = GetMessageString(0x4000A, nullptr);
    items[10].mText = GetMessageString(0x4000B, nullptr);
    items[11].mText = GetMessageString(0x4000C, nullptr);
    items[12].mText = GetMessageString(0x4000D, nullptr);
    items[13].mText = GetMessageString(0x4000E, nullptr);
    items[14].mText = GetMessageString(0x4000F, nullptr);
    items[15].mText = GetMessageString(0x40010, nullptr);
    items[16].mText = GetMessageString(0x40011, nullptr);
    if (bySpecies) {
        for (int i = 0; i < 17; ++i) {
            if (!HasItemWithSpeciesType(rItems, items[i].mKind)) {
                items[i].mKind = -1;
            }
        }
    } else {
        for (int i = 0; i < 17; ++i) {
            if (!HasItemWithMoveType(rItems, items[i].mKind)) {
                items[i].mKind = -1;
            }
        }
    }
    UIMenuParam param = {nullptr, nullptr, 17, 1, 17, 0, nullptr, "menu"};
    param.mTitle = GetMessageString(bySpecies ? 0x600B1 : 0x600B2, nullptr);
    param.mpItems = items;
    bool filterActive = mFilterType == FILTER_SPECIES_TYPE || mFilterType == FILTER_MOVE_TYPE;
    param.mDefaultIndex = filterActive ? mTypeIndex : -1;
    param.mpArchives = gSharedLayoutArchives;
    CUIMenuSoundCallback callbacks(0);
    if (onChanged) {
        callbacks.mSelectionCallbacks.Add(
            std::tr1::bind(&CPiiListFilterSort::PreviewTypeFilter, this, std::tr1::placeholders::_1, int(bySpecies ? FILTER_SPECIES_TYPE : FILTER_MOVE_TYPE), onChanged));
    }
    IController* pInput = IGameSession::GetInstance()->GetPlayer(player)->GetController();
    u32 choice = RunUIMenu(param, callbacks, pInput, pManager, GetDefaultFontMap(), true);
    if (choice <= 8 || (choice >= 10 && choice <= 17)) {
        mFilterType = bySpecies ? FILTER_SPECIES_TYPE : FILTER_MOVE_TYPE;
        mTypeIndex = choice;
        if (onChanged) {
            onChanged(this);
        }
        return true;
    }
    if (onChanged) {
        onChanged(this);
    }
    return false;
}

void CPiiListFilterSort::PreviewTypeFilter(BUI* pBui, int filterType, const std::tr1::function<void(const CPiiListFilterSort*)>& onChanged) {
    CUIMenu* pList = dynamic_cast<CUIMenu*>(pBui);
    if (pList) {
        std::tr1::shared_ptr<IUIListItem> pItem = pList->GetSelectedItem();
        if (pItem) {
            std::tr1::shared_ptr<CUIMenuItem> pMenuItem = std::tr1::dynamic_pointer_cast<CUIMenuItem>(pItem);
            if (pMenuItem) {
                int savedFilterType = mFilterType;
                int savedTypeIndex = mTypeIndex;
                mFilterType = filterType;
                mTypeIndex = pMenuItem->GetKind();
                if (onChanged) {
                    onChanged(this);
                }
                mFilterType = savedFilterType;
                mTypeIndex = savedTypeIndex;
            }
        }
    }
}

#pragma dont_inline on

bool CPiiListFilterSort::ShowPrefixFilterMenu(std::tr1::shared_ptr<CUIManager> pManager, int player, PiiItemVector& rItems,
                                              const std::tr1::function<void(const CPiiListFilterSort*)>& onChanged) {
    std::vector<UIMenuItem> items;
    for (int prefix = 0; prefix < 44; ++prefix) {
        if (HasItemWithPrefix(rItems, prefix)) {
            UIMenuItem item;
            item.mText = GetMessageString(0x6001D + prefix, nullptr);
            item.mKind = prefix;
            if (prefix == 0) {
                item.mText = GetMessageString(0x600B4, nullptr);
            }
            items.push_back(item);
        }
    }
    std::sort(items.begin(), items.end(), IsNameBefore);
    int itemCount = items.size();
    int rowCount = (itemCount + 10) / 11;
    int columnCount = itemCount < 11 ? itemCount : 11;
    UIMenuParam param = {nullptr, nullptr, 0, 0, 0, 0, nullptr, "menu"};
    param.mTitle = GetMessageString(0x600B3, nullptr);
    param.mpItems = &items[0];
    param.mItemCount = items.size();
    param.mColumnCount = rowCount;
    param.mRowCount = columnCount;
    param.mDefaultIndex = mFilterType == FILTER_PREFIX ? mPrefix : -1;
    param.mpArchives = gSharedLayoutArchives;
    CUIMenuSoundCallback callbacks(0);
    if (onChanged) {
        callbacks.mSelectionCallbacks.Add(std::tr1::bind(&CPiiListFilterSort::PreviewPrefixFilter, this, std::tr1::placeholders::_1, onChanged));
    }
    IController* pInput = IGameSession::GetInstance()->GetPlayer(player)->GetController();
    int choice = RunUIMenu(param, callbacks, pInput, pManager, GetDefaultFontMap(), true);
    if (choice >= 0) {
        if (choice < 44) {
            mFilterType = FILTER_PREFIX;
            mPrefix = choice;
            if (onChanged) {
                onChanged(this);
            }
            return true;
        }
    }
    if (onChanged) {
        onChanged(this);
    }
    return false;
}

#pragma dont_inline reset

void CPiiListFilterSort::PreviewPrefixFilter(BUI* pBui, const std::tr1::function<void(const CPiiListFilterSort*)>& onChanged) {
    CUIMenu* pMenu = dynamic_cast<CUIMenu*>(pBui);
    if (pMenu) {
        std::tr1::shared_ptr<IUIListItem> pItem = pMenu->GetSelectedItem();
        if (pItem) {
            std::tr1::shared_ptr<CUIMenuItem> pMenuItem = std::tr1::dynamic_pointer_cast<CUIMenuItem>(pItem);
            if (pMenuItem) {
                int savedFilterType = mFilterType;
                int savedPrefix = mPrefix;
                mFilterType = FILTER_PREFIX;
                mPrefix = pMenuItem->GetKind();
                if (onChanged) {
                    onChanged(this);
                }
                mFilterType = savedFilterType;
                mPrefix = savedPrefix;
            }
        }
    }
}

bool CPiiListFilterSort::ShowOriginalTrainerFilterMenu(std::tr1::shared_ptr<CUIManager> pManager, int player,
                                                       const std::tr1::function<void(const CPiiListFilterSort*)>& onChanged) {
    UIMenuItem items[2] = {{nullptr, 0}, {nullptr, 1}};
    items[0].mText = GetMessageString(0x600B6, nullptr);
    items[1].mText = GetMessageString(0x600B7, nullptr);
    UIMenuParam param = {nullptr, nullptr, 2, 1, 2, 0, nullptr, "menu"};
    param.mTitle = GetMessageString(0x600B5, nullptr);
    param.mpItems = items;
    param.mDefaultIndex = mFilterType == FILTER_ORIGINAL_TRAINER ? mInvertOriginalTrainer : -1;
    param.mpArchives = gSharedLayoutArchives;
    CUIMenuSoundCallback callbacks(0);
    if (onChanged) {
        callbacks.mSelectionCallbacks.Add(
            std::tr1::bind(&CPiiListFilterSort::PreviewOriginalTrainerFilter, this, std::tr1::placeholders::_1, onChanged));
    }
    IController* pInput = IGameSession::GetInstance()->GetPlayer(player)->GetController();
    u32 choice = RunUIMenu(param, callbacks, pInput, pManager, GetDefaultFontMap(), true);
    if (choice <= 1) {
        mFilterType = FILTER_ORIGINAL_TRAINER;
        mInvertOriginalTrainer = choice;
        if (onChanged) {
            onChanged(this);
        }
        return true;
    }
    if (onChanged) {
        onChanged(this);
    }
    return false;
}

void CPiiListFilterSort::PreviewOriginalTrainerFilter(BUI* pBui, const std::tr1::function<void(const CPiiListFilterSort*)>& onChanged) {
    CUIMenu* pMenu = dynamic_cast<CUIMenu*>(pBui);
    if (pMenu) {
        std::tr1::shared_ptr<IUIListItem> pItem = pMenu->GetSelectedItem();
        if (pItem) {
            std::tr1::shared_ptr<CUIMenuItem> pMenuItem = std::tr1::dynamic_pointer_cast<CUIMenuItem>(pItem);
            if (pMenuItem) {
                int savedFilterType = mFilterType;
                int savedInvert = mInvertOriginalTrainer;
                mFilterType = FILTER_ORIGINAL_TRAINER;
                mInvertOriginalTrainer = pMenuItem->GetKind();
                if (onChanged) {
                    onChanged(this);
                }
                mFilterType = savedFilterType;
                mInvertOriginalTrainer = savedInvert;
            }
        }
    }
}

bool CPiiListFilterSort::ShowSortTypeMenu(std::tr1::shared_ptr<CUIManager> pManager, int player,
                                          const std::tr1::function<void(const CPiiListFilterSort*)>& onChanged) {
    UIMenuItem items[7] = {{nullptr, 1}, {nullptr, 2}, {nullptr, 3}, {nullptr, 4}, {nullptr, 5}, {nullptr, 6}, {nullptr, 0}};
    items[0].mText = GetMessageString(0x600BA, nullptr);
    items[1].mText = GetMessageString(0x600BB, nullptr);
    items[2].mText = GetMessageString(0x600BC, nullptr);
    items[3].mText = GetMessageString(0x600BD, nullptr);
    items[4].mText = GetMessageString(0x600BE, nullptr);
    items[5].mText = GetMessageString(0x600BF, nullptr);
    items[6].mText = GetMessageString(0x600B9, nullptr);
    UIMenuParam param = {nullptr, nullptr, 7, 1, 7, 0, nullptr, "menu"};
    param.mTitle = GetMessageString(0x600B8, nullptr);
    param.mpItems = items;
    param.mDefaultIndex = mSortType;
    param.mpArchives = gSharedLayoutArchives;
    CUIMenuSoundCallback callbacks(0);
    if (onChanged) {
        callbacks.mSelectionCallbacks.Add(std::tr1::bind(&CPiiListFilterSort::PreviewSortType, this, std::tr1::placeholders::_1, onChanged));
    }
    IController* pInput = IGameSession::GetInstance()->GetPlayer(player)->GetController();
    u32 choice = RunUIMenu(param, callbacks, pInput, pManager, GetDefaultFontMap(), true);
    if (choice <= 6) {
        mSortType = choice;
        if (onChanged) {
            onChanged(this);
        }
        return true;
    }
    if (onChanged) {
        onChanged(this);
    }
    return false;
}

void CPiiListFilterSort::PreviewSortType(BUI* pBui, const std::tr1::function<void(const CPiiListFilterSort*)>& onChanged) {
    CUIMenu* pMenu = dynamic_cast<CUIMenu*>(pBui);
    if (pMenu) {
        std::tr1::shared_ptr<IUIListItem> pItem = pMenu->GetSelectedItem();
        if (pItem) {
            std::tr1::shared_ptr<CUIMenuItem> pMenuItem = std::tr1::dynamic_pointer_cast<CUIMenuItem>(pItem);
            if (pMenuItem) {
                int savedSortType = mSortType;
                mSortType = pMenuItem->GetKind();
                if (onChanged) {
                    onChanged(this);
                }
                mSortType = savedSortType;
            }
        }
    }
}

#pragma dont_inline reset
