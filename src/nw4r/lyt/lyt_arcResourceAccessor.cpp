#include <nw4r/lyt/lyt_arcResourceAccessor.h>
#include <string.h>

extern "C" int stricmp(const char* pLeft, const char* pRight);

namespace {

NO_INLINE s32 FindNameResource(ARCHandle *pArcHandle, const char *resName) {
    s32 entryNum = -1;
    ARCDir dir;
    ARCDirEntry dirEntry;
    BOOL bSuccess = ARCOpenDir(pArcHandle, ".", &dir);

    while (ARCReadDir(&dir, &dirEntry)) {
        if (dirEntry.isDir) {
            ARCChangeDir(pArcHandle, dirEntry.name);
            entryNum = FindNameResource(pArcHandle, resName);
            ARCChangeDir(pArcHandle, "..");

            if (entryNum != -1) {
                break;
            }
        } else {
            if (stricmp(resName, dirEntry.name) == 0) {
                entryNum = dirEntry.entryNum;
                break;
            }
        }
    }
    bSuccess = ARCCloseDir(&dir);
    return entryNum;
}

void *GetResourceSub(ARCHandle *pArcHandle, const char *resRootDir, u32 resType, const char *name, u32 *pSize) {
    s32 entryNum = -1;
    if (ARCConvertPathToEntrynum(pArcHandle, resRootDir) != -1 && ARCChangeDir(pArcHandle, resRootDir)) {
        if (resType == 0) {
            entryNum = FindNameResource(pArcHandle, name);
        } else {
            char resTypeStr[5];
            resTypeStr[0] = resType >> 24;
            resTypeStr[1] = resType >> 16;
            resTypeStr[2] = resType >> 8;
            resTypeStr[3] = resType;
            resTypeStr[4] = '\0';

            if (ARCConvertPathToEntrynum(pArcHandle, resTypeStr) != -1) {
                if (ARCChangeDir(pArcHandle, resTypeStr)) {
                    entryNum = ARCConvertPathToEntrynum(pArcHandle, name);
                    ARCChangeDir(pArcHandle, "..");
                }
            }
        }
        ARCChangeDir(pArcHandle, "..");
    }
    if (entryNum != -1) {
        ARCFileInfo arcFileInfo;
        ARCFastOpen(pArcHandle, entryNum, &arcFileInfo);
        void *resPointer = ARCGetStartAddrInMem(&arcFileInfo);
        if (pSize != nullptr) {
            *pSize = ARCGetLength(&arcFileInfo);
        }
        ARCClose(&arcFileInfo);
        return resPointer;
    }
    return nullptr;
}

}

namespace nw4r {
namespace lyt {

namespace detail {

inline ut::Font* FindFont(ut::LinkList<FontRefLink, 0>* pList, const char* pName) {
    ut::LinkList<FontRefLink, 0>::Iterator it = pList->GetBeginIter();
    for (; it != pList->GetEndIter(); it++) {
        if (strcmp(pName, it->GetFontName()) == 0) {
            return it->GetFont();
        }
    }
    return nullptr;
}

}

FontRefLink::FontRefLink() : mFont(nullptr) {}

void FontRefLink::Set(const char *name, ut::Font *pFont) {
    strcpy(this->mFontName, name);
    this->mFont = pFont;
}

bool ArcResourceLink::Set(void *pFile, const char *name) {
    if (!ARCInitHandle(pFile, &this->mArcHandle)) {
        return false;
    }
    strncpy(this->mResRootDir, name, 127);
    this->mResRootDir[127] = '\0';
    return true;
}

MultiArcResourceAccessor::MultiArcResourceAccessor() {}

MultiArcResourceAccessor::~MultiArcResourceAccessor() {
    this->mArcList.Clear();
}

void MultiArcResourceAccessor::Attach(ArcResourceLink *pArcResource) {
    this->mArcList.PushBack(pArcResource);
}

void *MultiArcResourceAccessor::GetResource(u32 resType, const char *name, u32 *pSize) {
    void *pResource;
    ut::LinkList<ArcResourceLink, 0>::Iterator it = this->mArcList.GetBeginIter();
    for (; it != mArcList.GetEndIter(); it++) {
        pResource = GetResourceSub(it->GetArcHandle(), it->GetResRootDir(), resType, name, pSize);
        if (pResource != nullptr) {
            return pResource;
        }
    }

    return nullptr;
}

void MultiArcResourceAccessor::RegistFont(FontRefLink *pLink) {
    this->mFontList.PushBack(pLink);
}

void MultiArcResourceAccessor::UnregistFont(FontRefLink* pLink) {
    mFontList.Erase(pLink);
}

ut::Font *MultiArcResourceAccessor::GetFont(const char *name) {
    return detail::FindFont(&this->mFontList, name);
}

}
}
