#include "nw4hbm/lyt/lyt_group.h"
#include "nw4hbm/lyt/lyt_common.h"
#include "nw4hbm/lyt/lyt_layout.h"
#include "nw4hbm/lyt/lyt_pane.h"
#include "nw4hbm/lyt/lyt_types.h"

#include <string.h>

inline void* operator new(size_t, void* p) { return p; }

namespace nw4hbm {
namespace lyt {

inline void Group::Init() {
    mbUserAllocated = false;
}

inline void Group::AppendPane(Pane* pPane) {
    if (void* pMem = Layout::AllocMemory(sizeof(detail::PaneLink))) {
        detail::PaneLink* pPaneLink = new (pMem) detail::PaneLink();
        pPaneLink->mTarget = pPane;
        mPaneLinkList.PushBack(pPaneLink);
    }
}

Group::Group(const res::Group* pRes, Pane* pRootPane) {
    Init();
    memcpy(mName, pRes->name, sizeof(mName));

    const char* paneNames = detail::ConvertOffsToPtr<char>(pRes, sizeof(*pRes));

    for (int i = 0; i < pRes->paneNum; i++) {
        Pane* pFindPane = pRootPane->FindPaneByName(paneNames + i * static_cast<int>(sizeof(pRes->name)), true);

        if (pFindPane) {
            AppendPane(pFindPane);
        }
    }
}

Group::~Group() {
    for (detail::PaneLinkList::Iterator it = mPaneLinkList.GetBeginIter(); it != mPaneLinkList.GetEndIter();) {
        detail::PaneLinkList::Iterator currIt = it++;
        mPaneLinkList.Erase(currIt);
        Layout::FreeMemory(&*currIt);
    }
}

GroupContainer::~GroupContainer() {
    for (GroupList::Iterator it = mGroupList.GetBeginIter(); it != mGroupList.GetEndIter();) {
        GroupList::Iterator currIt = it++;
        mGroupList.Erase(currIt);

        if (!currIt->IsUserAllocated()) {
            currIt->~Group();
            Layout::FreeMemory(&*currIt);
        }
    }
}

void GroupContainer::AppendGroup(Group* pGroup) {
    mGroupList.PushBack(pGroup);
}

Group* GroupContainer::FindGroupByName(const char* pName) {
    for (GroupList::Iterator it = mGroupList.GetBeginIter(); it != mGroupList.GetEndIter(); ++it) {
        if (detail::EqualsPaneName(it->GetName(), pName)) {
            return &*it;
        }
    }

    return nullptr;
}

}
}
