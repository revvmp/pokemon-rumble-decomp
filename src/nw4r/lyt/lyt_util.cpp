#include <nw4r/lyt/lyt_group.h>
#include <nw4r/lyt/lyt_pane.h>
#include <nw4r/lyt/lyt_utils.h>

namespace nw4r {
namespace lyt {

void BindAnimation(Group* pGroup, AnimTransform* pTransform, bool recursive, bool disable) {
    PaneList& rPaneList = *pGroup->GetPaneList();
    for (PaneList::Iterator it = rPaneList.GetBeginIter(); it != rPaneList.GetEndIter(); it++) {
        it->mTarget->BindAnimation(pTransform, recursive, disable);
    }
}

}
}
