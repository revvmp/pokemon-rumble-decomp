#ifndef RVL_SDK_CNT_H
#define RVL_SDK_CNT_H

#include <revolution/arc.h>
#include <revolution/dvd.h>
#include <revolution/mem/allocator.h>
#include <revolution/types.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef enum CNTType {
    CNT_TYPE_NAND = 1,
    CNT_TYPE_DVD = 2
} CNTType;

typedef enum CNTResult {
    CNT_RESULT_OK = 0,
    CNT_RESULT_ALLOC_FAILED = -5001,
    CNT_RESULT_FATAL = -5003,
    CNT_RESULT_CLOSE_FAILED = -5005,
    CNT_RESULT_NOT_FOUND = -5009,
    CNT_RESULT_DVD_CANCELED = -5014,
    CNT_RESULT_UNKNOWN = -5063
} CNTResult;

typedef struct CNTHandleNAND {
    ARCHandle ArcHandle;
    s32 FileDescriptor;
    MEMAllocator* allocator;
} CNTHandleNAND;

typedef struct CNTHandleDVD {
    u32 index;
    s32 rootDir;
    s32 currDir;
    u8 reserved[24];
} CNTHandleDVD;

typedef struct CNTHandle {
    union {
        CNTHandleNAND nand;
        CNTHandleDVD dvd;
    };
    u8 type;
} CNTHandle;

typedef struct CNTFileInfoNAND {
    CNTHandleNAND* CntHandle;
    u32 startOffset;
    u32 length;
    s32 readOffset;
} CNTFileInfoNAND;

typedef struct CNTFileInfoDVD {
    DVDFileInfo fileInfo;
    s32 readOffset;
} CNTFileInfoDVD;

typedef struct CNTFileInfo {
    union {
        CNTFileInfoNAND nand;
        CNTFileInfoDVD dvd;
    };
    u8 type;
} CNTFileInfo;

void CNTInit(void);
s32 CNTShutdown(void);
s32 CNTInitHandle(s32 contentNum, CNTHandle* handle, MEMAllocator* allocator);
s32 CNTReleaseHandle(CNTHandle* handle);
s32 CNTOpen(CNTHandle* handle, const char* path, CNTFileInfo* fileInfo);
s32 CNTReadAt(CNTFileInfo* fileInfo, void* buffer, s32 length, s32 offset);
u32 CNTGetLength(CNTFileInfo* fileInfo);
s32 CNTClose(CNTFileInfo* fileInfo);

#ifdef __cplusplus
}
#endif

#endif
