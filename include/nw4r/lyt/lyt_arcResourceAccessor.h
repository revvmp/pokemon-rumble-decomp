#ifndef NW4R_LYT_ARC_RESOURCE_ACCESSOR_H
#define NW4R_LYT_ARC_RESOURCE_ACCESSOR_H

#include <nw4r/lyt/lyt_resourceAccessor.h>
#include <nw4r/ut/ut_LinkList.h>
#include <revolution/arc.h>

namespace nw4r {
namespace lyt {

class FontRefLink {
public:
    FontRefLink();
    ~FontRefLink() {}

    void Set(const char* pName, ut::Font* pFont);
    ut::Font* GetFont() const { return mFont; }
    const char* GetFontName() const { return mFontName; }

private:
    ut::LinkListNode mNode;
    char mFontName[128];
    ut::Font* mFont;
};

class ArcResourceLink {
public:
    bool Set(void* pFile, const char* pName);
    const char* GetResRootDir() const { return mResRootDir; }
    ARCHandle* GetArcHandle() { return &mArcHandle; }

private:
    ut::LinkListNode mLink;
    ARCHandle mArcHandle;
    char mResRootDir[128];
};

class MultiArcResourceAccessor : public ResourceAccessor {
public:
    MultiArcResourceAccessor();
    virtual ~MultiArcResourceAccessor();
    virtual void* GetResource(u32 type, const char* pName, u32* pSize);
    virtual ut::Font* GetFont(const char* pName);

    void Attach(ArcResourceLink* pArcResource);
    void RegistFont(FontRefLink* pLink);
    void UnregistFont(FontRefLink* pLink);

private:
    ut::LinkList<ArcResourceLink, 0> mArcList;
    ut::LinkList<FontRefLink, 0> mFontList;
};

}
}

#endif
