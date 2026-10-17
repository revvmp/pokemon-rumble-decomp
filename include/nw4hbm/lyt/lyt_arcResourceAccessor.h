#ifndef NW4HBM_LYT_ARC_RESOURCE_ACCESSOR_H
#define NW4HBM_LYT_ARC_RESOURCE_ACCESSOR_H
#include "nw4hbm/types_nw4hbm.h"

#include "nw4hbm/lyt/lyt_resourceAccessor.h"
#include "nw4hbm/ut/ut_LinkList.h"

#include "revolution/arc.h"

#include <string.h>

namespace nw4hbm {
namespace lyt {

class FontRefLink {
public:
    FontRefLink();

    const char* GetFontName() const { return mFontName; }
    ut::Font* GetFont() const { return mpFont; }

public:
    NW4R_UT_LINKLIST_NODE_DECL();

private:
    char mFontName[128];
    ut::Font* mpFont;
};

NW4R_UT_LINKLIST_TYPEDEF_DECL(FontRefLink);

class ArcResourceLink {
public:
    bool Set(void* archiveStart, const char* resourceRootDirectory);

    ARCHandle* GetArcHandle() { return &mArcHandle; }
    const char* GetResRootDir() const { return mResRootDir; }

public:
    NW4R_UT_LINKLIST_NODE_DECL();

private:
    ARCHandle mArcHandle;
    char mResRootDir[128];
};

NW4R_UT_LINKLIST_TYPEDEF_DECL(ArcResourceLink);

class ArcResourceAccessor : public ResourceAccessor {
public:
    ArcResourceAccessor();
    virtual ~ArcResourceAccessor() {}

    virtual void* GetResource(u32 resType, const char* name, u32* pSize);
    virtual ut::Font* GetFont(const char* name);

    bool Attach(void* archiveStart, const char* resourceRootDirectory);

private:
    ARCHandle mArcHandle;
    void* mArcBuf;
    FontRefLinkList mFontList;
    char mResRootDir[128];
};

class MultiArcResourceAccessor : public ResourceAccessor {
public:
    MultiArcResourceAccessor();
    virtual ~MultiArcResourceAccessor();

    virtual void* GetResource(u32 resType, const char* name, u32* pSize);
    virtual ut::Font* GetFont(const char* name);

    void Attach(ArcResourceLink* pLink);
    void DetachAll() { mArcList.Clear(); }

private:
    ArcResourceLinkList mArcList;
    FontRefLinkList mFontList;
};

namespace detail {

inline ut::Font* FindFont(FontRefLinkList* pFontRefList, const char* name) {
    for (FontRefLinkList::Iterator it = pFontRefList->GetBeginIter(); it != pFontRefList->GetEndIter(); ++it) {
        if (strcmp(name, it->GetFontName()) == 0) {
            return it->GetFont();
        }
    }

    return nullptr;
}

}

}
}

#endif
