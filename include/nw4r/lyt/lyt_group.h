#ifndef NW4R_LYT_GROUP_H
#define NW4R_LYT_GROUP_H

#include <nw4r/ut/ut_LinkList.h>

namespace nw4r {
namespace lyt {

class Pane;
namespace res {
struct Group;
}

namespace detail {
struct PaneLink {
    ut::LinkListNode mLink;
    Pane* mTarget;
};
}

typedef ut::LinkList<detail::PaneLink, 0> PaneList;

class Group {
public:
    Group(const res::Group* pResource, Pane* pRootPane);
    virtual ~Group();
    void AppendPane(Pane* pPane);
    void Init();

    PaneList* GetPaneList() { return &mPaneListLink; }
    bool IsUserAllocated() const { return mbUserAllocated; }
    const char* GetName() const { return mName; }

private:
    ut::LinkListNode mLink;
    PaneList mPaneListLink;
    char mName[17];
    bool mbUserAllocated;
    u8 mPadding[2];
};

}
}

#endif
