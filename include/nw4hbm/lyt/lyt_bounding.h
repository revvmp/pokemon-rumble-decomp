#ifndef NW4HBM_LYT_BOUNDING_H
#define NW4HBM_LYT_BOUNDING_H
#include "nw4hbm/types_nw4hbm.h"

#include "nw4hbm/lyt/lyt_pane.h"

#include "nw4hbm/ut.h"

namespace nw4hbm {
namespace lyt {

struct ResBlockSet;

namespace res {

struct Bounding : Pane {
    static const u32 SIGNATURE = FOURCC('b', 'n', 'd', '1');
};

}

class Bounding : public Pane {
public:
    NW4R_UT_RTTI_DECL(Bounding);

public:
    Bounding(const res::Bounding* pRes, const ResBlockSet& rBlockSet);
    virtual ~Bounding();

    virtual void DrawSelf(const DrawInfo& rInfo);
};

}
}

#endif
