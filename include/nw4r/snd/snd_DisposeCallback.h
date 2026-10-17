#ifndef NW4R_SND_DISPOSE_CALLBACK_H
#define NW4R_SND_DISPOSE_CALLBACK_H
#include <nw4r/types_nw4r.h>

#include <nw4r/ut/ut_LinkList.h>

namespace nw4r {
namespace snd {
namespace detail {

class DisposeCallback {
public:
    NW4R_UT_LINKLIST_NODE_DECL();

public:
    virtual ~DisposeCallback() {}

    virtual void InvalidateData(const void* pStart,
                                const void* pEnd) = 0;
    virtual void InvalidateWaveData(const void* pStart,
                                    const void* pEnd) = 0;
};

NW4R_UT_LINKLIST_TYPEDEF_DECL(DisposeCallback);

}
}
}

#endif
