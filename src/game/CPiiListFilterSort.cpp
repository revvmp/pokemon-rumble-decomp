#include <game/CPiiListFilterSort.h>
#include <game/CMsgRes.h>
#include <game/CUIPiiListItem.h>
#include <vector>

typedef std::vector<std::tr1::shared_ptr<CUIPiiListItem> > PiiItemVector;

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
