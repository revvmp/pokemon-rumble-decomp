#ifndef NW4R_LYT_BOUNDING_H
#define NW4R_LYT_BOUNDING_H
#include <nw4r/types_nw4r.h>

#include <nw4r/lyt/lyt_pane.h>

#include <nw4r/lyt/lyt_resources.h>

namespace nw4r {
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
