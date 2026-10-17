#include <stdio.h>
#include <string.h>

#include "revolution/fs.h"
#include "revolution/nand.h"

static BOOL nandInspectPermission(u8 perm);
static void nandSplitPerm(u8 perm, u32* ownerAcc, u32* groupAcc, u32* othersAcc);
static void nandGetStatusCallback(ISFSError result, void* ctxt);
static void nandComposePerm(u8* perm, u32 ownerAcc, u32 groupAcc, u32 othersAcc) NO_INLINE;

static ISFSError nandCreate(const char* path, u8 perm, u8 attr, NANDCommandBlock* block, BOOL asyncFlag, BOOL privilegeFlag) {
    char absPath[NAND_MAX_PATH] = "";
    u32 ownerAcc = 0, groupAcc = 0, othersAcc = 0;

    nandGenerateAbsPath(absPath, path);

    if (!privilegeFlag && nandIsPrivatePath(absPath)) {
        return ISFS_ERROR_ACCESS;
    } else if (!nandInspectPermission(perm)) {
        return ISFS_ERROR_INVALID;
    } else {
        nandSplitPerm(perm, &ownerAcc, &groupAcc, &othersAcc);

        if (asyncFlag) {
            return ISFS_CreateFileAsync((const u8*)absPath, attr, ownerAcc, groupAcc, othersAcc, nandCallback, block);
        } else {
            return ISFS_CreateFile((const u8*)absPath, attr, ownerAcc, groupAcc, othersAcc);
        }
    }
}

s32 NANDCreate(const char* path, u8 perm, u8 attr) {
    if (!nandIsInitialized()) {
        return NAND_RESULT_FATAL_ERROR;
    }

    return nandConvertErrorCode(nandCreate(path, perm, attr, NULL, FALSE, FALSE));
}

s32 NANDPrivateCreate(const char* path, u8 perm, u8 attr) {
    if (!nandIsInitialized()) {
        return NAND_RESULT_FATAL_ERROR;
    }

    return nandConvertErrorCode(nandCreate(path, perm, attr, NULL, FALSE, TRUE));
}

s32 NANDCreateAsync(const char* path, u8 perm, u8 attr, NANDCallback cb, NANDCommandBlock* block) {
    if (!nandIsInitialized()) {
        return NAND_RESULT_FATAL_ERROR;
    }

    block->callback = cb;
    return nandConvertErrorCode(nandCreate(path, perm, attr, block, TRUE, FALSE));
}

s32 NANDPrivateCreateAsync(const char* path, u8 perm, u8 attr, NANDCallback cb, NANDCommandBlock* block) {
    if (!nandIsInitialized()) {
        return NAND_RESULT_FATAL_ERROR;
    }

    block->callback = cb;
    return nandConvertErrorCode(nandCreate(path, perm, attr, block, TRUE, TRUE));
}

static ISFSError nandDelete(const char* path, NANDCommandBlock* block, BOOL asyncFlag, BOOL privilegeFlag) {
    char absPath[NAND_MAX_PATH] = "";

    nandGenerateAbsPath(absPath, path);

    if (!privilegeFlag && nandIsPrivatePath(absPath)) {
        return ISFS_ERROR_ACCESS;
    } else if (asyncFlag) {
        return ISFS_DeleteAsync((const u8*)absPath, nandCallback, block);
    } else {
        return ISFS_Delete((const u8*)absPath);
    }
}

s32 NANDDelete(const char* path) {
    if (!nandIsInitialized()) {
        return NAND_RESULT_FATAL_ERROR;
    }

    return nandConvertErrorCode(nandDelete(path, NULL, FALSE, FALSE));
}

s32 NANDPrivateDelete(const char* path) {
    if (!nandIsInitialized()) {
        return NAND_RESULT_FATAL_ERROR;
    }

    return nandConvertErrorCode(nandDelete(path, NULL, FALSE, TRUE));
}

s32 NANDDeleteAsync(const char* path, NANDCallback cb, NANDCommandBlock* block) {
    if (!nandIsInitialized()) {
        return NAND_RESULT_FATAL_ERROR;
    }

    block->callback = cb;
    return nandConvertErrorCode(nandDelete(path, block, TRUE, FALSE));
}

s32 NANDPrivateDeleteAsync(const char* path, NANDCallback cb, NANDCommandBlock* block) {
    if (!nandIsInitialized()) {
        return NAND_RESULT_FATAL_ERROR;
    }

    block->callback = cb;
    return nandConvertErrorCode(nandDelete(path, block, TRUE, TRUE));
}

s32 NANDRead(NANDFileInfo* info, void* buf, u32 length) {
    if (!nandIsInitialized()) {
        return NAND_RESULT_FATAL_ERROR;
    }

    return nandConvertErrorCode(ISFS_Read(info->fileDescriptor, (u8*)buf, length));
}

s32 NANDReadAsync(NANDFileInfo* info, void* buf, u32 length, NANDCallback cb, NANDCommandBlock* block) {
    if (!nandIsInitialized()) {
        return NAND_RESULT_FATAL_ERROR;
    }

    block->callback = cb;
    return nandConvertErrorCode(ISFS_ReadAsync(info->fileDescriptor, (u8*)buf, length, nandCallback, block));
}

s32 NANDWrite(NANDFileInfo* info, const void* buf, u32 length) {
    if (!nandIsInitialized()) {
        return NAND_RESULT_FATAL_ERROR;
    }

    return nandConvertErrorCode(ISFS_Write(info->fileDescriptor, (const u8*)buf, length));
}

s32 NANDWriteAsync(NANDFileInfo* info, const void* buf, u32 length, NANDCallback cb, NANDCommandBlock* block) {
    if (!nandIsInitialized()) {
        return NAND_RESULT_FATAL_ERROR;
    }

    block->callback = cb;
    return nandConvertErrorCode(ISFS_WriteAsync(info->fileDescriptor, (const u8*)buf, length, nandCallback, block));
}

static ISFSError nandSeek(IOSFd fd, s32 offset, s32 whence, NANDCommandBlock* block, BOOL asyncFlag) {
    u32 isfsWhence = 0xFFFFFFFF;

    switch (whence) {
    case NAND_SEEK_SET:
        isfsWhence = 0;
        break;
    case NAND_SEEK_CUR:
        isfsWhence = 1;
        break;
    case NAND_SEEK_END:
        isfsWhence = 2;
        break;
    default:
        break;
    }

    if (asyncFlag) {
        return ISFS_SeekAsync(fd, offset, isfsWhence, nandCallback, block);
    } else {
        return ISFS_Seek(fd, offset, isfsWhence);
    }
}

s32 NANDSeek(NANDFileInfo* info, s32 offset, s32 whence) {
    if (!nandIsInitialized()) {
        return NAND_RESULT_FATAL_ERROR;
    }

    return nandConvertErrorCode(nandSeek(info->fileDescriptor, offset, whence, NULL, FALSE));
}

s32 NANDSeekAsync(NANDFileInfo* info, s32 offset, s32 whence, NANDCallback cb, NANDCommandBlock* block) {
    if (!nandIsInitialized()) {
        return NAND_RESULT_FATAL_ERROR;
    }

    block->callback = cb;
    return nandConvertErrorCode(nandSeek(info->fileDescriptor, offset, whence, block, TRUE));
}

static ISFSError nandCreateDir(const char* path, u8 perm, u8 attr, NANDCommandBlock* block, BOOL asyncFlag, BOOL privilegeFlag) {
    char absPath[NAND_MAX_PATH] = "";

    nandGenerateAbsPath(absPath, path);

    if (!privilegeFlag && nandIsPrivatePath(absPath)) {
        return ISFS_ERROR_ACCESS;
    } else if (!nandInspectPermission(perm)) {
        return ISFS_ERROR_INVALID;
    } else {
        u32 ownerAcc = 0;
        u32 groupAcc = 0;
        u32 othersAcc = 0;

        nandSplitPerm(perm, &ownerAcc, &groupAcc, &othersAcc);

        if (asyncFlag) {
            return ISFS_CreateDirAsync((const u8*)absPath, attr, ownerAcc, groupAcc, othersAcc, nandCallback, block);
        } else {
            return ISFS_CreateDir((const u8*)absPath, attr, ownerAcc, groupAcc, othersAcc);
        }
    }
}

s32 NANDCreateDir(const char* path, u8 perm, u8 attr) {
    if (!nandIsInitialized()) {
        return NAND_RESULT_FATAL_ERROR;
    }

    return nandConvertErrorCode(nandCreateDir(path, perm, attr, NULL, FALSE, FALSE));
}

s32 NANDCreateDirAsync(const char* path, u8 perm, u8 attr, NANDCallback cb, NANDCommandBlock* block) {
    if (!nandIsInitialized()) {
        return NAND_RESULT_FATAL_ERROR;
    }

    block->callback = cb;
    return nandConvertErrorCode(nandCreateDir(path, perm, attr, block, TRUE, FALSE));
}

s32 NANDPrivateCreateDirAsync(const char* path, u8 perm, u8 attr, NANDCallback cb, NANDCommandBlock* block) {
    if (!nandIsInitialized()) {
        return NAND_RESULT_FATAL_ERROR;
    }

    block->callback = cb;
    return nandConvertErrorCode(nandCreateDir(path, perm, attr, block, TRUE, TRUE));
}

static ISFSError nandMove(const char* path, const char* destDir, NANDCommandBlock* block, BOOL asyncFlag, BOOL privilegeFlag) {
    char absOldPath[NAND_MAX_PATH] = "";
    char absNewPath[NAND_MAX_PATH] = "";
    char relativeName[ISFS_INODE_NAMELEN + 1] = "";

    nandGenerateAbsPath(absOldPath, path);
    nandGetRelativeName(relativeName, absOldPath);
    nandGenerateAbsPath(absNewPath, destDir);

    if (strcmp(absNewPath, "/") == 0) {
        sprintf(absNewPath, "/%s", relativeName);
    } else {
        strcat(absNewPath, "/");
        strcat(absNewPath, relativeName);
    }

    if (!privilegeFlag && (nandIsPrivatePath(absOldPath) || nandIsPrivatePath(absNewPath))) {
        return ISFS_ERROR_ACCESS;
    } else if (asyncFlag) {
        return ISFS_RenameAsync((const u8*)absOldPath, (const u8*)absNewPath, nandCallback, block);
    } else {
        return ISFS_Rename((const u8*)absOldPath, (const u8*)absNewPath);
    }
}

s32 NANDMoveAsync(const char* path, const char* destDir, NANDCallback cb, NANDCommandBlock* block) {
    if (!nandIsInitialized()) {
        return NAND_RESULT_FATAL_ERROR;
    }

    block->callback = cb;
    return nandConvertErrorCode(nandMove(path, destDir, block, TRUE, FALSE));
}

static ISFSError nandGetFileStatus(IOSFd fd, u32* length, u32* pos) {
    ISFSFileStats stats ATTRIBUTE_ALIGN(32);
    ISFSError result = ISFS_GetFileStats(fd, &stats);

    if (result == ISFS_ERROR_OK) {
        if (length) {
            *length = stats.size;
        }

        if (pos) {
            *pos = stats.offset;
        }
    }

    return result;
}

s32 NANDGetLength(NANDFileInfo* info, u32* length) {
    if (!nandIsInitialized()) {
        return NAND_RESULT_FATAL_ERROR;
    }

    return nandConvertErrorCode(nandGetFileStatus(info->fileDescriptor, length, NULL));
}

s32 NANDTell(NANDFileInfo* info, u32* pos) {
    if (!nandIsInitialized()) {
        return NAND_RESULT_FATAL_ERROR;
    }

    return nandConvertErrorCode(nandGetFileStatus(info->fileDescriptor, NULL, pos));
}

static void nandComposePerm(u8* perm, u32 ownerAcc, u32 groupAcc, u32 othersAcc) {
    u32 composed = 0;

    if (ownerAcc & NAND_ACCESS_READ) {
        composed |= NAND_PERM_RUSR;
    }

    if (ownerAcc & NAND_ACCESS_WRITE) {
        composed |= NAND_PERM_WUSR;
    }

    if (groupAcc & NAND_ACCESS_READ) {
        composed |= NAND_PERM_RGRP;
    }

    if (groupAcc & NAND_ACCESS_WRITE) {
        composed |= NAND_PERM_WGRP;
    }

    if (othersAcc & NAND_ACCESS_READ) {
        composed |= NAND_PERM_ROTH;
    }

    if (othersAcc & NAND_ACCESS_WRITE) {
        composed |= NAND_PERM_WOTH;
    }

    *perm = (u8)composed;
}

static void nandSplitPerm(u8 perm, u32* ownerAcc, u32* groupAcc, u32* othersAcc) {
    *ownerAcc = 0;
    *groupAcc = 0;
    *othersAcc = 0;

    if (perm & NAND_PERM_RUSR) {
        *ownerAcc |= NAND_ACCESS_READ;
    }

    if (perm & NAND_PERM_WUSR) {
        *ownerAcc |= NAND_ACCESS_WRITE;
    }

    if (perm & NAND_PERM_RGRP) {
        *groupAcc |= NAND_ACCESS_READ;
    }

    if (perm & NAND_PERM_WGRP) {
        *groupAcc |= NAND_ACCESS_WRITE;
    }

    if (perm & NAND_PERM_ROTH) {
        *othersAcc |= NAND_ACCESS_READ;
    }

    if (perm & NAND_PERM_WOTH) {
        *othersAcc |= NAND_ACCESS_WRITE;
    }
}

static ISFSError nandGetStatus(const char* path, NANDStatus* stat, NANDCommandBlock* block, BOOL asyncFlag, BOOL privilegeFlag) {
    char absPath[NAND_MAX_PATH] = "";

    nandGenerateAbsPath(absPath, path);

    if (!privilegeFlag && nandIsUnderPrivatePath(absPath)) {
        return ISFS_ERROR_ACCESS;
    } else if (asyncFlag) {
        return ISFS_GetAttrAsync((const u8*)absPath, &stat->ownerId, &stat->groupId, &block->attr, &block->ownerAcc, &block->groupAcc,
                                 &block->othersAcc, nandGetStatusCallback, block);
    } else {
        u32 attr = 0;
        u32 ownerAcc = 0;
        u32 groupAcc = 0;
        u32 othersAcc = 0;
        ISFSError result = ISFS_GetAttr((const u8*)absPath, &stat->ownerId, &stat->groupId, &attr, &ownerAcc, &groupAcc, &othersAcc);

        if (result == ISFS_ERROR_OK) {
            nandComposePerm(&stat->permission, ownerAcc, groupAcc, othersAcc);
            stat->attribute = (u8)attr;
        }

        return result;
    }
}

static void nandGetStatusCallback(ISFSError result, void* ctxt) {
    NANDCommandBlock* block = (NANDCommandBlock*)ctxt;

    if (result == ISFS_ERROR_OK) {
        NANDStatus* stat = (NANDStatus*)block->status;
        stat->attribute = (u8)block->attr;
        nandComposePerm(&stat->permission, block->ownerAcc, block->groupAcc, block->othersAcc);
    }

    block->callback(nandConvertErrorCode(result), block);
}

s32 NANDGetStatus(const char* path, NANDStatus* stat) {
    if (!nandIsInitialized()) {
        return NAND_RESULT_FATAL_ERROR;
    }

    return nandConvertErrorCode(nandGetStatus(path, stat, NULL, FALSE, FALSE));
}

s32 NANDPrivateGetStatus(const char* path, NANDStatus* stat) {
    if (!nandIsInitialized()) {
        return NAND_RESULT_FATAL_ERROR;
    }

    return nandConvertErrorCode(nandGetStatus(path, stat, NULL, FALSE, TRUE));
}

s32 NANDPrivateGetStatusAsync(const char* path, NANDStatus* stat, NANDCallback cb, NANDCommandBlock* block) {
    if (!nandIsInitialized()) {
        return NAND_RESULT_FATAL_ERROR;
    }

    block->callback = cb;
    block->status = stat;
    return nandConvertErrorCode(nandGetStatus(path, stat, block, TRUE, TRUE));
}

static BOOL nandInspectPermission(u8 perm) {
    if (perm & NAND_PERM_RUSR) {
        return TRUE;
    } else {
        return FALSE;
    }
}

s32 NANDGetHomeDirUsageAsync(u32* blocks, u32* inodes, NANDCallback cb, NANDCommandBlock* block) {
    if (!nandIsInitialized()) {
        return NAND_RESULT_FATAL_ERROR;
    }

    block->callback = cb;
    return nandConvertErrorCode(ISFS_GetUsageAsync((const u8*)nandGetHomeDir(), blocks, inodes, nandCallback, block));
}
