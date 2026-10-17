#include <revolution/cnt.h>
#include <revolution/esp/esp.h>
#include <revolution/nand/nandlogging.h>
#include <revolution/os.h>
#include "MSL_C/ctype.h"
#include <stdio.h>
#include <string.h>

typedef struct FSTEntry {
    u32 isDirAndStringOff;
    u32 parentOrPosition;
    u32 nextEntryOrLength;
} FSTEntry;

#define entryIsDir(fstStart, i) ((((fstStart)[i].isDirAndStringOff & 0xFF000000) == 0) ? FALSE : TRUE)
#define stringOff(fstStart, i) ((fstStart)[i].isDirAndStringOff & 0x00FFFFFF)
#define parentDir(fstStart, i) ((fstStart)[i].parentOrPosition)
#define nextDir(fstStart, i) ((fstStart)[i].nextEntryOrLength)

#define CNT_PATH_MAX 128

static BOOL sCntInitialized;
static const char* sCntVersion = "<< RVL_SDK - CNT \trelease build: Feb 27 2009 10:05:46 (0x4302_145) >>";

static s32 contentConvertErrorCode(s32 errorCode);
static u32 entryToPathDVD(CNTHandleDVD* handle, u32 entry, char* path, u32 maxlen);
static BOOL contentGetCurrentDirDVD(CNTHandleDVD* handle, char* path, u32 maxlen);

void CNTInit(void) {
    if (!sCntInitialized) {
        BOOL enabled;
        ESP_InitLib();
        enabled = OSDisableInterrupts();
        sCntInitialized = TRUE;
        OSRestoreInterrupts(enabled);
        OSRegisterVersion(sCntVersion);
    }
}

s32 CNTShutdown(void) {
    return ESP_CloseLib();
}

static s32 contentInitHandleNAND(s32 contentNum, CNTHandleNAND* handle, MEMAllocator* allocator) {
    ARCHeader header ATTRIBUTE_ALIGN(32);
    ARCHandle arcHandle;
    void* archive;
    u32 headerSize;
    s32 result;
    s32 fd;

    fd = ESP_OpenContentFile((u32)contentNum);
    if (fd < 0) {
        return contentConvertErrorCode(fd);
    }

    result = ESP_ReadContentFile(fd, &header, sizeof(header));
    if (result < 0) {
        return contentConvertErrorCode(result);
    }

    headerSize = OSRoundUp32B(header.fileStart);

    result = ESP_SeekContentFile(fd, 0, 0);
    if (result < 0) {
        return contentConvertErrorCode(result);
    }

    archive = MEMAllocFromAllocator(allocator, headerSize);
    if (archive == NULL) {
        return CNT_RESULT_ALLOC_FAILED;
    }

    result = ESP_ReadContentFile(fd, archive, headerSize);
    if (result < 0) {
        MEMFreeToAllocator(allocator, archive);
        return contentConvertErrorCode(result);
    }

    ARCInitHandle(archive, &arcHandle);
    memcpy(&handle->ArcHandle, &arcHandle, sizeof(ARCHandle));
    handle->FileDescriptor = fd;
    handle->allocator = allocator;
    return CNT_RESULT_OK;
}

static BOOL isSame(const char* path, const char* string) {
    while (*string != '\0') {
        if (tolower(*path++) != tolower(*string++)) {
            return FALSE;
        }
    }

    if (*path == '/' || *path == '\0') {
        return TRUE;
    }

    return FALSE;
}

s32 contentConvertPathToEntrynumDVD(CNTHandleDVD* handle, const char* pathPtr) {
    const char* ptr;
    char* stringPtr;
    BOOL isDir;
    s32 length;
    s32 dirLookAt;
    u32 i;
    FSTEntry* FSTEntries;
    char* FSTStringStart;

    dirLookAt = handle->currDir;
    FSTEntries = (FSTEntry*)DVDGetFSTLocation();
    FSTStringStart = (char*)&FSTEntries[nextDir(FSTEntries, 0)];

    while (TRUE) {
        if (*pathPtr == '\0') {
            return dirLookAt;
        } else if (*pathPtr == '/') {
            dirLookAt = handle->rootDir;
            pathPtr++;
            continue;
        } else if (*pathPtr == '.') {
            if (*(pathPtr + 1) == '.') {
                if (*(pathPtr + 2) == '/') {
                    if (dirLookAt == handle->rootDir) {
                        return -1;
                    }
                    dirLookAt = parentDir(FSTEntries, dirLookAt);
                    pathPtr += 3;
                    continue;
                } else if (*(pathPtr + 2) == '\0') {
                    return (s32)parentDir(FSTEntries, dirLookAt);
                }
            } else if (*(pathPtr + 1) == '/') {
                pathPtr += 2;
                continue;
            } else if (*(pathPtr + 1) == '\0') {
                return dirLookAt;
            }
        }

        for (ptr = pathPtr; *ptr != '\0' && *ptr != '/'; ptr++) {
        }

        isDir = (*ptr == '\0') ? FALSE : TRUE;
        length = (s32)(ptr - pathPtr);
        ptr = pathPtr;

        for (i = (u32)dirLookAt + 1; i < nextDir(FSTEntries, dirLookAt);
             i = entryIsDir(FSTEntries, i) ? nextDir(FSTEntries, i) : (i + 1)) {
        dot:
            if (entryIsDir(FSTEntries, i) == FALSE && isDir == TRUE) {
                continue;
            }

            stringPtr = FSTStringStart + stringOff(FSTEntries, i);

            if (*stringPtr == '.' && *(stringPtr + 1) == '\0') {
                i++;
                goto dot;
            }

            if (isSame(ptr, stringPtr) == TRUE) {
                goto next_hier;
            }
        }

        return -1;

    next_hier:
        if (!isDir) {
            return (s32)i;
        }

        dirLookAt = i;
        pathPtr += length + 1;
    }
}

s32 contentInitHandleDVD(s32 contentNum, CNTHandleDVD* handle) {
    char path[CNT_PATH_MAX];
    s32 entry;

    sprintf(path, "/content%d", contentNum);
    entry = DVDConvertPathToEntrynum(path);
    if (entry < 0) {
        OSReport("Warning: CNTInitHandle(): directory '%s' is not found under '/'\n", path);
        return CNT_RESULT_NOT_FOUND;
    }

    handle->index = (u32)contentNum;
    handle->rootDir = entry;
    handle->currDir = entry;
    return CNT_RESULT_OK;
}

s32 contentInitHandleTitleDVD(const char* titleName, s32 contentNum, CNTHandleDVD* handle) {
    char path[CNT_PATH_MAX];
    s32 entry;

    sprintf(path, "/datatitle/%s/content%d", titleName, contentNum);
    entry = DVDConvertPathToEntrynum(path);
    if (entry < 0) {
        OSReport("Warning: CNTInitHandleTitle(): directory '%s' is not found under '/'\n", path);
        return CNT_RESULT_NOT_FOUND;
    }

    handle->index = (u32)contentNum;
    handle->rootDir = entry;
    handle->currDir = entry;
    return CNT_RESULT_OK;
}

s32 contentOpenDVD(CNTHandleDVD* handle, const char* path, CNTFileInfoDVD* fileInfo) {
    s32 entry;

    entry = contentConvertPathToEntrynumDVD(handle, path);
    if (entry < 0) {
        char currentDir[CNT_PATH_MAX];
        contentGetCurrentDirDVD(handle, currentDir, sizeof(currentDir));
        OSReport("CNTOpen(): '%s' was not found under %s.\n", path, currentDir);
        return CNT_RESULT_NOT_FOUND;
    }

    if (!DVDFastOpen(entry, &fileInfo->fileInfo)) {
        return CNT_RESULT_NOT_FOUND;
    }

    fileInfo->readOffset = 0;
    return CNT_RESULT_OK;
}

static s32 contentConvertErrorCode(s32 errorCode) {
    s32 errorTable[] = {
        0,     0,     -1001, -5063, -1002, -5063, -1003, -5063, -1004, -5063, -1005, -5063, -1006, -5063,
        -1007, -5063, -1008, -5063, -1009, -5063, -1010, -5063, -1011, -5063, -1012, -5063, -1013, -5063,
        -1014, -5063, -1015, -5063, -1016, -5000, -1017, -5009, -1018, -5063, -1019, -5063, -1020, -5063,
        -1021, -5063, -1022, -5063, -1023, -5063, -1024, -5008, -1025, -5009, -1026, -5010, -1027, -5063,
        -1028, -5009, -1029, -5010, -1030, -5063, -1031, -5063, -1032, -5063, -1033, -5063, -1034, -5063,
        -1035, -5063, -1036, -5063, -1037, -5063, -1038, -5063, 0,     0,     -102,  -5010, -103,  -5011,
        -114,  -5012, -105,  -5063, -116,  -5013, -101,  -5009, -108,  -5063, -109,  -5000, -107,  -5063,
        -106,  -5009, -115,  -5063, -104,  -5063, -111,  -5063, -117,  -5063, -118,  -5008, -119,  -5127,
        -1,    -5010, -2,    -5063, -3,    -5063, -4,    -5009, -5,    -5063, -6,    -5009, -7,    -5063,
        -8,    -5008, -9,    -5063, -10,   -5063, -11,   -5063, -12,   -5012, -13,   -5063, -14,   -5063,
        -15,   -5063, -16,   -5063, -17,   -5063, -18,   -5063, -19,   -5063, -20,   -5063, -21,   -5063,
        -22,   -5008, -23,   -5063,
    };
    int i = 0;

    if (errorCode >= 0) {
        return errorCode;
    }

    for (; i < (int)(sizeof(errorTable) / sizeof(errorTable[0])); i += 2) {
        if (errorCode == errorTable[i]) {
            if (errorCode == -114 || errorCode == -116 || errorCode == -117 || errorCode == -9 || errorCode == -12) {
                char message[128] ATTRIBUTE_ALIGN(64);
                sprintf(message, "ES error code: %d", errorCode);
                NANDLoggingAddMessageAsync(NULL, message);
            }
            return errorTable[i + 1];
        }
    }

    OSReport("CAUTION!  Unexpected error code [%d] was found.\n", errorCode);
    {
        char message[128] ATTRIBUTE_ALIGN(64);
        sprintf(message, "ES unexpected error code: %d", errorCode);
        NANDLoggingAddMessageAsync(NULL, message);
    }
    return CNT_RESULT_UNKNOWN;
}

static u32 myStrncpy(char* dest, char* src, u32 maxlen) {
    u32 i = maxlen;

    while (i > 0 && *src != '\0') {
        *dest++ = *src++;
        i--;
    }

    return maxlen - i;
}

static u32 entryToPathDVD(CNTHandleDVD* handle, u32 entry, char* path, u32 maxlen) {
    char* name;
    u32 loc;
    FSTEntry* FSTEntries = (FSTEntry*)DVDGetFSTLocation();
    char* FSTStringStart = (char*)&FSTEntries[nextDir(FSTEntries, 0)];

    if (entry == handle->rootDir) {
        return 0;
    }

    name = FSTStringStart + stringOff(FSTEntries, entry);
    loc = entryToPathDVD(handle, parentDir(FSTEntries, entry), path, maxlen);

    if (loc == maxlen) {
        return loc;
    }

    path[loc++] = '/';
    loc += myStrncpy(path + loc, name, maxlen - loc);
    return loc;
}

static BOOL contentConvertEntrynumToPathDVD(CNTHandleDVD* handle, s32 entrynum, char* path, u32 maxlen) {
    u32 loc;
    FSTEntry* FSTEntries = (FSTEntry*)DVDGetFSTLocation();

    loc = entryToPathDVD(handle, (u32)entrynum, path, maxlen);

    if (loc == maxlen) {
        path[maxlen - 1] = '\0';
        return FALSE;
    }

    if (entryIsDir(FSTEntries, entrynum)) {
        if (loc == maxlen - 1) {
            path[loc] = '\0';
            return FALSE;
        }

        path[loc++] = '/';
    }

    path[loc] = '\0';
    return TRUE;
}

static BOOL contentGetCurrentDirDVD(CNTHandleDVD* handle, char* path, u32 maxlen) {
    return contentConvertEntrynumToPathDVD(handle, handle->currDir, path, maxlen);
}

s32 CNTInitHandle(s32 contentNum, CNTHandle* handle, MEMAllocator* allocator) {
    handle->type = CNT_TYPE_NAND;
    return contentInitHandleNAND(contentNum, &handle->nand, allocator);
}

s32 CNTReleaseHandle(CNTHandle* handle) {
    switch (handle->type) {
    case CNT_TYPE_NAND:
        MEMFreeToAllocator(handle->nand.allocator, handle->nand.ArcHandle.archiveStartAddr);
        return contentConvertErrorCode(ESP_CloseContentFile(handle->nand.FileDescriptor));
    case CNT_TYPE_DVD:
        return CNT_RESULT_OK;
    default:
        return CNT_RESULT_UNKNOWN;
    }
}

s32 CNTOpen(CNTHandle* handle, const char* path, CNTFileInfo* fileInfo) {
    s32 entry;
    ARCFileInfo arcFileInfo;

    switch (handle->type) {
    case CNT_TYPE_NAND:
        fileInfo->type = CNT_TYPE_NAND;
        entry = ARCConvertPathToEntrynum(&handle->nand.ArcHandle, path);
        if (entry < 0) {
            char currentDir[CNT_PATH_MAX];
            if (ARCGetCurrentDir(&handle->nand.ArcHandle, currentDir, sizeof(currentDir))) {
                if (strncmp(currentDir, "/./", 3) == 0) {
                    char* dest = currentDir;
                    char* src = currentDir + 2;
                    while (*src != '\0') {
                        *dest++ = *src++;
                    }
                    *dest = '\0';
                }
            }
            OSReport("CNTOpen(): '%s' was not found under %s.\n", path, currentDir);
            return CNT_RESULT_NOT_FOUND;
        }

        if (!ARCFastOpen(&handle->nand.ArcHandle, entry, &arcFileInfo)) {
            return CNT_RESULT_NOT_FOUND;
        }

        fileInfo->nand.CntHandle = &handle->nand;
        fileInfo->nand.startOffset = arcFileInfo.startOffset;
        fileInfo->nand.length = arcFileInfo.length;
        fileInfo->nand.readOffset = 0;
        return CNT_RESULT_OK;

    case CNT_TYPE_DVD:
        fileInfo->type = CNT_TYPE_DVD;
        return contentOpenDVD(&handle->dvd, path, &fileInfo->dvd);
    default:
        return CNT_RESULT_UNKNOWN;
    }
}

static s32 contentReadDVD(CNTFileInfoDVD* fileInfo, void* buffer, s32 length, s32 offset) {
    s32 result = DVDReadPrio(&fileInfo->fileInfo, buffer, length, offset + fileInfo->readOffset, 2);
    if (result < 0) {
        switch (result) {
        case -3:
            return CNT_RESULT_DVD_CANCELED;
        default:
            return CNT_RESULT_FATAL;
        }
    }
    return result;
}

s32 CNTReadAt(CNTFileInfo* fileInfo, void* buffer, s32 length, s32 offset) {
    s32 result;
    s32 position;

    switch (fileInfo->type) {
    case CNT_TYPE_NAND:
        position = fileInfo->nand.readOffset + offset;
        if (position < 0 || position > fileInfo->nand.length) {
            return CNT_RESULT_NOT_FOUND;
        }

        result = ESP_SeekContentFile(fileInfo->nand.CntHandle->FileDescriptor, (s32)(fileInfo->nand.startOffset + position), 0);
        if (result < 0) {
            return contentConvertErrorCode(result);
        }

        return contentConvertErrorCode(ESP_ReadContentFile(fileInfo->nand.CntHandle->FileDescriptor, buffer, (u32)length));

    case CNT_TYPE_DVD:
        return contentReadDVD(&fileInfo->dvd, buffer, length, offset);

    default:
        return CNT_RESULT_UNKNOWN;
    }
}

u32 CNTGetLength(CNTFileInfo* fileInfo) {
    switch (fileInfo->type) {
    case CNT_TYPE_NAND:
        return fileInfo->nand.length;
    case CNT_TYPE_DVD:
        return fileInfo->dvd.fileInfo.length;
    default:
        return 0;
    }
}

s32 CNTClose(CNTFileInfo* fileInfo) {
    switch (fileInfo->type) {
    case CNT_TYPE_NAND:
        return CNT_RESULT_OK;
    case CNT_TYPE_DVD:
        if (!DVDClose(&fileInfo->dvd.fileInfo)) {
            return CNT_RESULT_CLOSE_FAILED;
        }
        return CNT_RESULT_OK;
    default:
        return CNT_RESULT_UNKNOWN;
    }
}
