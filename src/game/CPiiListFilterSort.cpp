#include <game/CPiiListFilterSort.h>
#include <game/CMsgRes.h>
#include <game/CUIPiiListItem.h>
#include <game/CUIList.h>
#include <game/IGameSession.h>
#include <game/CPlayer.h>
#include <game/COyaInfo.h>
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
    const CPiiPersonalData* pA = rA->GetPPD().get();
    const CPiiPersonalData* pB = rB->GetPPD().get();
    int result = CompareValue(pA->GetBP(), pB->GetBP());
    if (result != 0) {
        return result < 0;
    }
    result = CompareValue(pA->GetCreationTime(), pB->GetCreationTime());
    if (result != 0) {
        return result < 0;
    }
    return pA->GetId() < pB->GetId();
}

bool CompareBPDescending(const PiiItemPtr& rA, const PiiItemPtr& rB) {
    const CPiiPersonalData* pA = rA->GetPPD().get();
    const CPiiPersonalData* pB = rB->GetPPD().get();
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

inline int Reverse(int result) {
    return result != 0 ? -result : 0;
}

inline int CompareBPChain(const CPiiPersonalData* pA, const CPiiPersonalData* pB) {
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
    result = CompareValue(pA->GetBP(), pB->GetBP());
    if (result == 0) {
        result = CompareValue(pA->GetCreationTime(), pB->GetCreationTime());
    }
    if (result != 0) {
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

bool CompareSpeciesOrder(const PiiItemPtr& rA, const PiiItemPtr& rB) {
    const PiiSpeciesData* pSpeciesA = rA->GetPPD()->GetSpeciesData();
    const PiiSpeciesData* pSpeciesB = rB->GetPPD()->GetSpeciesData();
    int result = Compare(pSpeciesA, pSpeciesB);
    if (result != 0) {
        return result < 0;
    }
    return CompareBPDescending(rA, rB);
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
                rItems = sorted;
            }
        }
    }
}

#pragma dont_inline reset
