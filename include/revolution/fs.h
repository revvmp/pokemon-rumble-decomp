#ifndef REVOLUTION_FS_H
#define REVOLUTION_FS_H

#include "revolution/types.h"

#ifdef __cplusplus
extern "C" {
#endif

#define ISFS_ERROR_OK 0
#define ISFS_ERROR_INVALID -101
#define ISFS_ERROR_ACCESS -102
#define ISFS_ERROR_CORRUPT -103
#define ISFS_ERROR_NOTREADY -104
#define ISFS_ERROR_EXISTS -105
#define ISFS_ERROR_NOEXISTS -106
#define ISFS_ERROR_MAXFILES -107
#define ISFS_ERROR_MAXBLOCKS -108
#define ISFS_ERROR_MAXFD -109
#define ISFS_ERROR_MAXDEPTH -110
#define ISFS_ERROR_OPENFD -111
#define ISFS_ERROR_BADBLOCK -112
#define ISFS_ERROR_ECC -113
#define ISFS_ERROR_ECC_CRIT -114
#define ISFS_ERROR_NOTEMPTY -115
#define ISFS_ERROR_HMAC -116
#define ISFS_ERROR_UNKNOWN -117
#define ISFS_ERROR_BUSY -118
#define ISFS_ERROR_SHUTDOWN -119

#define ISFS_INODE_NAMELEN 12

typedef s32 IOSFd;
typedef u32 IOSUid;
typedef u16 IOSGid;

typedef s32 ISFSError;
typedef void (*ISFSCallback)(ISFSError result, void* ctxt);

typedef struct ISFSFileStats {
    u32 size;
    u32 offset;
} ISFSFileStats;

ISFSError ISFS_CreateDir(const u8* dname, u32 dirAttr, u32 ownerAcc, u32 groupAcc, u32 othersAcc);
ISFSError ISFS_CreateDirAsync(const u8* dname, u32 dirAttr, u32 ownerAcc, u32 groupAcc, u32 othersAcc, ISFSCallback cb, void* fsCtxt);
ISFSError ISFS_GetAttr(const u8* name, IOSUid* ownerId, IOSGid* groupId, u32* attr, u32* ownerAcc, u32* groupAcc, u32* othersAcc);
ISFSError ISFS_GetAttrAsync(const u8* name, IOSUid* ownerId, IOSGid* groupId, u32* attr, u32* ownerAcc, u32* groupAcc, u32* othersAcc, ISFSCallback cb, void* fsCtxt);
ISFSError ISFS_Delete(const u8* name);
ISFSError ISFS_DeleteAsync(const u8* name, ISFSCallback cb, void* fsCtxt);
ISFSError ISFS_Rename(const u8* oldName, const u8* newName);
ISFSError ISFS_RenameAsync(const u8* oldName, const u8* newName, ISFSCallback cb, void* fsCtxt);
ISFSError ISFS_GetUsageAsync(const u8* dname, u32* nblocks, u32* ninodes, ISFSCallback cb, void* fsCtxt);
ISFSError ISFS_CreateFile(const u8* fname, u32 fileAttr, u32 ownerAcc, u32 groupAcc, u32 othersAcc);
ISFSError ISFS_CreateFileAsync(const u8* fname, u32 fileAttr, u32 ownerAcc, u32 groupAcc, u32 othersAcc, ISFSCallback cb, void* fsCtxt);
ISFSError ISFS_GetFileStats(IOSFd fd, ISFSFileStats* stats);
ISFSError ISFS_Seek(IOSFd fd, s32 offset, u32 whence);
ISFSError ISFS_SeekAsync(IOSFd fd, s32 offset, u32 whence, ISFSCallback cb, void* fsCtxt);
ISFSError ISFS_Read(IOSFd fd, u8* buf, u32 size);
ISFSError ISFS_ReadAsync(IOSFd fd, u8* buf, u32 size, ISFSCallback cb, void* fsCtxt);
ISFSError ISFS_Write(IOSFd fd, const u8* buf, u32 size);
ISFSError ISFS_WriteAsync(IOSFd fd, const u8* buf, u32 size, ISFSCallback cb, void* fsCtxt);

#ifdef __cplusplus
}
#endif

#endif
