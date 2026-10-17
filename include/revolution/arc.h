#ifndef REVOLUTION_ARC_H
#define REVOLUTION_ARC_H

#include "revolution/types.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct ARCHeader {
    u32 magic;
    s32 fstStart;
    s32 fstSize;
    s32 fileStart;
    s32 reserved[4];
} ARCHeader;

typedef struct ARCHandle {
    void* archiveStartAddr;
    void* FSTStart;
    void* fileStart;
    u32 entryNum;
    char* FSTStringStart;
    u32 FSTLength;
    u32 currDir;
} ARCHandle;

typedef struct ARCFileInfo {
    ARCHandle* handle;
    u32 startOffset;
    u32 length;
} ARCFileInfo;

typedef struct ARCDir {
    ARCHandle* handle;
    u32 entryNum;
    u32 location;
    u32 next;
} ARCDir;

typedef struct ARCDirEntry {
    ARCHandle* handle;
    u32 entryNum;
    BOOL isDir;
    char* name;
} ARCDirEntry;

BOOL ARCInitHandle(void* arcStart, ARCHandle* handle);
BOOL ARCOpen(ARCHandle* handle, const char* fileName, ARCFileInfo* af);
BOOL ARCFastOpen(ARCHandle* handle, s32 entrynum, ARCFileInfo* af);
s32 ARCConvertPathToEntrynum(ARCHandle* handle, const char* pathPtr);
BOOL ARCGetCurrentDir(ARCHandle* handle, char* path, u32 maxlen);
void* ARCGetStartAddrInMem(ARCFileInfo* af);
u32 ARCGetLength(ARCFileInfo* af);
BOOL ARCClose(ARCFileInfo* af);
BOOL ARCChangeDir(ARCHandle* handle, const char* dirName);
BOOL ARCOpenDir(ARCHandle* handle, const char* dirName, ARCDir* dir);
BOOL ARCReadDir(ARCDir* dir, ARCDirEntry* dirent);
BOOL ARCCloseDir(ARCDir* dir);

#ifdef __cplusplus
}
#endif

#endif
