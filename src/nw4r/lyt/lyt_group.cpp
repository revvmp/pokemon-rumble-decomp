#include <nw4r/lyt/lyt_common.h>
#include <nw4r/lyt/lyt_group.h>
#include <nw4r/lyt/lyt_pane.h>
#include <nw4r/lyt/lyt_layout.h>
#include <nw4r/lyt/lyt_resources.h>

#include <string.h>

namespace nw4r {
namespace lyt {

Group::Group(const res::Group* pRes, Pane* pRootPane) {
    Init();

    strncpy(mName, pRes->name, 16);
    mName[16] = '\0';

    const char* pNameBase =
        detail::ConvertOffsToPtr<char>(pRes, sizeof(res::Group));

    for (int i = 0; i < pRes->paneNum; i++) {
        Pane* pResult = pRootPane->FindPaneByName(
            pNameBase + i * 16, true);

        if (pResult != nullptr) {
            AppendPane(pResult);
        }
    }
}

void Group::Init() {
    mbUserAllocated = false;
}

Group::~Group() {
    NW4R_UT_LINKLIST_FOREACH_SAFE (it, mPaneListLink, {
        mPaneListLink.Erase(it);
        Layout::DeleteObj(&*it);
    })
}

void Group::AppendPane(Pane* pPane) {
    detail::PaneLink* pLink = Layout::NewObj<detail::PaneLink>();

    if (pLink != nullptr) {
        pLink->mTarget = pPane;
        mPaneListLink.PushBack(pLink);
    }
}

GroupContainer::~GroupContainer() {
    NW4R_UT_LINKLIST_FOREACH_SAFE (it, mGroupList, {
        mGroupList.Erase(it);

        if (!it->IsUserAllocated()) {
            Layout::DeleteObj(&*it);
        }
    })
}

void GroupContainer::AppendGroup(Group* pGroup) {
    mGroupList.PushBack(pGroup);
}

Group* GroupContainer::FindGroupByName(const char* pName) {
    NW4R_UT_LINKLIST_FOREACH (it, mGroupList, {
        if (detail::EqualsResName(it->GetName(), pName)) {
            return &*it;
        }
    })

    return nullptr;
}

}
}
