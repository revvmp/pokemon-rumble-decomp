#ifndef NW4HBM_LYT_GROUP_H
#define NW4HBM_LYT_GROUP_H
#include "nw4hbm/types_nw4hbm.h"

#include "nw4hbm/lyt/lyt_resources.h"

#include "nw4hbm/ut.h"

namespace nw4hbm {
namespace lyt {

class Pane;

namespace detail {

struct PaneLink {
    NW4R_UT_LINKLIST_NODE_DECL();
    Pane* mTarget;
};

NW4R_UT_LINKLIST_TYPEDEF_DECL(PaneLink);

}

namespace res {

struct Group {
    static const u32 SIGNATURE = FOURCC('g', 'r', 'p', '1');

    DataBlockHeader blockHeader;
    char name[NW4R_LYT_RES_NAME_LEN];
    u16 paneNum;
    u8 PADDING_0x1A[0x1C - 0x1A];
};

}

class Group {
public:
    Group(const res::Group* pRes, Pane* pRootPane);
    virtual ~Group();

    void AppendPane(Pane* pPane);

    detail::PaneLinkList& GetPaneList() {
        return mPaneLinkList;
    }

    const char* GetName() const {
        return mName;
    }

    bool IsUserAllocated() const {
        return mbUserAllocated;
    }

public:
    NW4R_UT_LINKLIST_NODE_DECL();

protected:
    detail::PaneLinkList mPaneLinkList;
    char mName[NW4R_LYT_RES_NAME_LEN + 1];
    bool mbUserAllocated;
    u8 PADDING_0x2A[0x2C - 0x2A];

private:
    void Init();
};

NW4R_UT_LINKLIST_TYPEDEF_DECL(Group);

class GroupContainer {
public:
    GroupContainer() {}
    ~GroupContainer();

    void AppendGroup(Group* pGroup);
    Group* FindGroupByName(const char* pName);

    GroupList& GetGroupList() {
        return mGroupList;
    }

protected:
    GroupList mGroupList;
};

}
}

#endif
