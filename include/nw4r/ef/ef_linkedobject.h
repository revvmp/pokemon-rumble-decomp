#ifndef NW4R_EF_LINKED_OBJECT_H
#define NW4R_EF_LINKED_OBJECT_H
#include <nw4r/types_nw4r.h>

#include <nw4r/math/math_types.h>
#include <nw4r/ut/ut_list.h>

namespace nw4r {
namespace ef {

class LinkedObject {
public:
    ut::Link mMemoryLink;
    u32 mObjectID;

public:
    u16 GetObjectIdx() const {
        return mObjectID;
    }
};

}
}

#endif
