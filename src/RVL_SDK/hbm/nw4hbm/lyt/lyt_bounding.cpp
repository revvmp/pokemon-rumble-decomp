#include "nw4hbm/lyt/lyt_bounding.h"
#include "nw4hbm/lyt/lyt_common.h"
#include "nw4hbm/lyt/lyt_drawInfo.h"

namespace nw4hbm {
namespace lyt {

NW4R_UT_RTTI_DEF_DERIVED(Bounding, Pane);

Bounding::Bounding(const res::Bounding* pRes, const ResBlockSet&) : Pane(pRes) {}

Bounding::~Bounding() {}

void Bounding::DrawSelf(const DrawInfo& rInfo) {
    if (rInfo.IsDebugDrawMode()) {
        LoadMtx(rInfo);
        detail::DrawLine(GetVtxPos(), mSize, ut::Color(0x00FF00FF));
    }
}

}
}
