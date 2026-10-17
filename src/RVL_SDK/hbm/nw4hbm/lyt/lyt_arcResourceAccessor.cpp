#include "nw4hbm/lyt/lyt_arcResourceAccessor.h"

#include <MSL_C/extras.h>
#include <string.h>

namespace {

s32 FindNameResource(ARCHandle* pArcHandle, const char* resName) {
    s32 entryNum = -1;
    ARCDir dir;
    ARCDirEntry dirEntry;

    ARCOpenDir(pArcHandle, ".", &dir);

    while (ARCReadDir(&dir, &dirEntry)) {
        if (dirEntry.isDir) {
            ARCChangeDir(pArcHandle, dirEntry.name);
            entryNum = FindNameResource(pArcHandle, resName);
            ARCChangeDir(pArcHandle, "..");

            if (entryNum != -1) {
                break;
            }
        } else if (stricmp(resName, dirEntry.name) == 0) {
            entryNum = dirEntry.entryNum;
            break;
        }
    }

    ARCCloseDir(&dir);
    return entryNum;
}

void* GetResourceSub(ARCHandle* pArcHandle, const char* resRootDir, u32 resType, const char* name, u32* pSize) {
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

            if (ARCConvertPathToEntrynum(pArcHandle, resTypeStr) != -1 && ARCChangeDir(pArcHandle, resTypeStr)) {
                entryNum = ARCConvertPathToEntrynum(pArcHandle, name);
                ARCChangeDir(pArcHandle, "..");
            }
        }

        ARCChangeDir(pArcHandle, "..");
    }

    if (entryNum != -1) {
        ARCFileInfo arcFileInfo;
        ARCFastOpen(pArcHandle, entryNum, &arcFileInfo);
        void* resPtr = ARCGetStartAddrInMem(&arcFileInfo);

        if (pSize) {
            *pSize = ARCGetLength(&arcFileInfo);
        }

        ARCClose(&arcFileInfo);
        return resPtr;
    }

    return nullptr;
}

}

namespace nw4hbm {
namespace lyt {

ArcResourceAccessor::ArcResourceAccessor() : mArcBuf(nullptr) {}

bool ArcResourceAccessor::Attach(void* archiveStart, const char* resourceRootDirectory) {
    if (!ARCInitHandle(archiveStart, &mArcHandle)) {
        return false;
    }

    mArcBuf = archiveStart;
    strncpy(mResRootDir, resourceRootDirectory, sizeof(mResRootDir) - 1);
    mResRootDir[sizeof(mResRootDir) - 1] = '\0';
    return true;
}

void* ArcResourceAccessor::GetResource(u32 resType, const char* name, u32* pSize) {
    return GetResourceSub(&mArcHandle, mResRootDir, resType, name, pSize);
}

bool ArcResourceLink::Set(void* archiveStart, const char* resourceRootDirectory) {
    if (!ARCInitHandle(archiveStart, &mArcHandle)) {
        return false;
    }

    strncpy(mResRootDir, resourceRootDirectory, sizeof(mResRootDir) - 1);
    mResRootDir[sizeof(mResRootDir) - 1] = '\0';
    return true;
}

ut::Font* ArcResourceAccessor::GetFont(const char* name) {
    return detail::FindFont(&mFontList, name);
}

MultiArcResourceAccessor::MultiArcResourceAccessor() {}

MultiArcResourceAccessor::~MultiArcResourceAccessor() {
    DetachAll();
}

void MultiArcResourceAccessor::Attach(ArcResourceLink* pLink) {
    mArcList.PushBack(pLink);
}

void* MultiArcResourceAccessor::GetResource(u32 resType, const char* name, u32* pSize) {
    for (ArcResourceLinkList::Iterator it = mArcList.GetBeginIter(); it != mArcList.GetEndIter(); ++it) {
        if (void* resPtr = GetResourceSub(it->GetArcHandle(), it->GetResRootDir(), resType, name, pSize)) {
            return resPtr;
        }
    }

    return nullptr;
}

ut::Font* MultiArcResourceAccessor::GetFont(const char* name) {
    return detail::FindFont(&mFontList, name);
}

}
}
