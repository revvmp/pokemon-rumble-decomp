#ifndef HBM_ANM_CONTROLLER_H
#define HBM_ANM_CONTROLLER_H

#include "revolution/hbm/homebutton/HBMFrameController.h"
#include "nw4hbm/lyt/lyt_animation.h"
#include "nw4hbm/lyt/lyt_group.h"

namespace homebutton {

class GroupAnmController : public FrameController {
public:
    GroupAnmController();
    virtual ~GroupAnmController();

    void do_calc();

    nw4hbm::lyt::Group* mpGroup;
    nw4hbm::lyt::AnimTransform* mpAnimGroup;
};

}

#endif
