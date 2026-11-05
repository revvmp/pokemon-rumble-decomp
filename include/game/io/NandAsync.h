#ifndef GAME_NANDASYNC_H
#define GAME_NANDASYNC_H

#include <revolution/nand.h>
#include <revolution/os/OSThread.h>

struct NandWaitContext : NANDCommandBlock {
    OSSemaphore mSemaphore;
    s32 mResult;
};

s32 WaitNandCreate(const char* pPath, u8 permission, u8 attribute);
s32 WaitNandOpen(const char* pPath, NANDFileInfo* pFile, u8 access);
s32 WaitNandClose(NANDFileInfo* pFile);
s32 WaitNandWrite(NANDFileInfo* pFile, const void* pData, u32 size);
s32 WaitNandCreateDir(const char* pPath, u8 permission, u8 attribute);
s32 WaitNandDelete(const char* pPath);
s32 WaitNandMove(const char* pPath, const char* pDestination);
s32 WaitNandCheck(u32 blocks, u32 inodes, u32* pAnswer);
s32 WaitNandGetHomeDirUsage(u32* pBlocks, u32* pInodes);

#endif
