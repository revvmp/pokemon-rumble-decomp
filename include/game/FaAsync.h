#ifndef GAME_FAASYNC_H
#define GAME_FAASYNC_H

#include <revolution/fa.h>
#include <revolution/os/OSThread.h>

struct FaWaitContext : FACommandBlock {
    OSSemaphore mSemaphore;
    s32 mResult;
};

s32 WaitFaAttach(u8 device, s8* pNandPath, u32 nandSize, FADrvTbl* pDrivers);
s32 WaitFaMount(s8 drive);
s32 WaitFaUnmount(s8 drive, u32 mode);
s32 WaitFaCreate(const char* pPath, s32 mode, FAFile** ppFile);
s32 WaitFaFwrite(void* pBuffer, u32 size, u32 count, FAFile* pFile, u32* pWritten);
s32 WaitFaFsfirst(const char* pPath, u8 attributes, FADta* pDta);
s32 WaitFaFsnext(FADta* pDta);
s32 WaitFaRemove(const char* pPath);
s32 WaitFaMkdir(const char* pPath);
s32 WaitFaRmdir(const char* pPath);

#endif
