#ifndef REVOLUTION_NAND_H
#define REVOLUTION_NAND_H

#include "revolution/fs.h"
#include "revolution/types.h"

#ifdef __cplusplus
extern "C" {
#endif

#define NAND_RESULT_OK 0
#define NAND_RESULT_ACCESS (-1)
#define NAND_RESULT_ALLOC_FAILED (-2)
#define NAND_RESULT_BUSY (-3)
#define NAND_RESULT_CORRUPT (-4)
#define NAND_RESULT_ECC_CRIT (-5)
#define NAND_RESULT_EXISTS (-6)
#define NAND_RESULT_INVALID (-8)
#define NAND_RESULT_MAXBLOCKS (-9)
#define NAND_RESULT_MAXFD (-10)
#define NAND_RESULT_MAXFILES (-11)
#define NAND_RESULT_NOEXISTS (-12)
#define NAND_RESULT_NOTEMPTY (-13)
#define NAND_RESULT_OPENFD (-14)
#define NAND_RESULT_AUTHENTICATION (-15)
#define NAND_RESULT_MAXDEPTH (-16)
#define NAND_RESULT_UNKNOWN (-64)
#define NAND_RESULT_FATAL_ERROR (-128)

#define NAND_MAX_PATH 64

#define NAND_SEEK_SET 0
#define NAND_SEEK_CUR 1
#define NAND_SEEK_END 2

#define NAND_ACCESS_READ 1
#define NAND_ACCESS_WRITE 2

#define NAND_PERM_RUSR (NAND_ACCESS_READ << 4)
#define NAND_PERM_WUSR (NAND_ACCESS_WRITE << 4)
#define NAND_PERM_RGRP (NAND_ACCESS_READ << 2)
#define NAND_PERM_WGRP (NAND_ACCESS_WRITE << 2)
#define NAND_PERM_ROTH (NAND_ACCESS_READ << 0)
#define NAND_PERM_WOTH (NAND_ACCESS_WRITE << 0)

struct NANDCommandBlock;

typedef void (*NANDCallback)(s32 result, struct NANDCommandBlock* block);

typedef struct NANDFileInfo {
    IOSFd fileDescriptor;
    IOSFd origFd;
    char origPath[NAND_MAX_PATH];
    char tmpPath[NAND_MAX_PATH];
    u8 accType;
    u8 stage;
    u8 mark;
} NANDFileInfo;

typedef struct NANDCommandBlock {
    void* userData;
    NANDCallback callback;
    void* fileInfo;
    void* bytes;
    void* inodes;
    void* status;
    u32 ownerId;
    u16 groupId;
    u8 nextStage;
    u32 attr;
    u32 ownerAcc;
    u32 groupAcc;
    u32 othersAcc;
    u32 num;
    char absPath[NAND_MAX_PATH];
    u32* length;
    u32* pos;
    int state;
    void* copyBuf;
    u32 bufLength;
    u8* type;
    u32 uniqNo;
    u32 reqBlocks;
    u32 reqInodes;
    u32* answer;
    u32 homeBlocks;
    u32 homeInodes;
    u32 userBlocks;
    u32 userInodes;
    u32 workBlocks;
    u32 workInodes;
    const char** dir;
    BOOL simpleFlag;
} NANDCommandBlock;

typedef struct NANDStatus {
    IOSUid ownerId;
    IOSGid groupId;
    u8 attribute;
    u8 permission;
} NANDStatus;

s32 NANDCreate(const char* path, u8 perm, u8 attr);
s32 NANDPrivateCreate(const char* path, u8 perm, u8 attr);
s32 NANDCreateAsync(const char* path, u8 perm, u8 attr, NANDCallback cb, NANDCommandBlock* block);
s32 NANDPrivateCreateAsync(const char* path, u8 perm, u8 attr, NANDCallback cb, NANDCommandBlock* block);
s32 NANDDelete(const char* path);
s32 NANDPrivateDelete(const char* path);
s32 NANDDeleteAsync(const char* path, NANDCallback cb, NANDCommandBlock* block);
s32 NANDPrivateDeleteAsync(const char* path, NANDCallback cb, NANDCommandBlock* block);
s32 NANDOpen(const char* path, NANDFileInfo* info, u8 accType);
s32 NANDClose(NANDFileInfo* info);
s32 NANDRead(NANDFileInfo* info, void* buf, u32 length);
s32 NANDReadAsync(NANDFileInfo* info, void* buf, u32 length, NANDCallback cb, NANDCommandBlock* block);
s32 NANDWrite(NANDFileInfo* info, const void* buf, u32 length);
s32 NANDWriteAsync(NANDFileInfo* info, const void* buf, u32 length, NANDCallback cb, NANDCommandBlock* block);
s32 NANDSeek(NANDFileInfo* info, s32 offset, s32 whence);
s32 NANDSeekAsync(NANDFileInfo* info, s32 offset, s32 whence, NANDCallback cb, NANDCommandBlock* block);
s32 NANDCreateDir(const char* path, u8 perm, u8 attr);
s32 NANDCreateDirAsync(const char* path, u8 perm, u8 attr, NANDCallback cb, NANDCommandBlock* block);
s32 NANDPrivateCreateDirAsync(const char* path, u8 perm, u8 attr, NANDCallback cb, NANDCommandBlock* block);
s32 NANDMoveAsync(const char* path, const char* destDir, NANDCallback cb, NANDCommandBlock* block);
s32 NANDGetLength(NANDFileInfo* info, u32* length);
s32 NANDTell(NANDFileInfo* info, u32* pos);
s32 NANDGetStatus(const char* path, NANDStatus* stat);
s32 NANDPrivateGetStatus(const char* path, NANDStatus* stat);
s32 NANDPrivateGetStatusAsync(const char* path, NANDStatus* stat, NANDCallback cb, NANDCommandBlock* block);
s32 NANDGetHomeDirUsageAsync(u32* blocks, u32* inodes, NANDCallback cb, NANDCommandBlock* block);

BOOL nandIsInitialized(void);
s32 nandConvertErrorCode(ISFSError result);
BOOL nandIsPrivatePath(const char* path);
BOOL nandIsUnderPrivatePath(const char* path);
void nandGenerateAbsPath(char* absPath, const char* path);
void nandGetRelativeName(char* name, const char* path);
const char* nandGetHomeDir(void);
void nandCallback(ISFSError result, void* ctxt);

#ifdef __cplusplus
}
#endif

#endif
