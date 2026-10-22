#include <game/CPiiListFilterSort.h>
#include <game/CMsgRes.h>
#include <game/CUIPiiListItem.h>
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
    for (PiiItemVector::const_iterator it = rItems.begin(); it != rItems.end(); ++it) {
        if ((*it)->GetPPD()->GetPrefix() != 0) {
            return true;
        }
    }
    return false;
}

bool HasItemWithPrefix(const PiiItemVector& rItems, int prefix) {
    for (PiiItemVector::const_iterator it = rItems.begin(); it != rItems.end(); ++it) {
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
