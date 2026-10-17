#include "revolution/hbm/homebutton/HBMAnmController.h"
#include "nw4hbm/lyt/lyt_pane.h"

namespace homebutton {

GroupAnmController::GroupAnmController() : mpGroup(nullptr), mpAnimGroup(nullptr) {}

GroupAnmController::~GroupAnmController() {}

void GroupAnmController::do_calc() {
    bool enable;

    if (mState == eState_Playing) {
        calc();
        enable = true;
        mpAnimGroup->SetFrame(mCurFrame);
    } else {
        enable = false;
    }

    nw4hbm::lyt::detail::PaneLinkList& rPaneList = mpGroup->GetPaneList();

    for (nw4hbm::lyt::detail::PaneLinkList::Iterator it = rPaneList.GetBeginIter(); it != rPaneList.GetEndIter(); ++it) {
        it->mTarget->SetAnimationEnable(mpAnimGroup, enable, false);
    }
}

}
