#include <game/NandAsync.h>

#include <game/CScopedActivity.h>

void YieldFiber();

static void CompleteNandOperation(s32 result, NANDCommandBlock* pBlock) {
    NandWaitContext* pContext = static_cast<NandWaitContext*>(pBlock);
    pContext->mResult = result;
    OSSignalSemaphore(&pContext->mSemaphore);
}

s32 WaitNandCreate(const char* pPath, u8 permission, u8 attribute) {
    CScopedActivity activity;
    NandWaitContext context;
    OSInitSemaphore(&context.mSemaphore, 0);
    s32 result = NANDCreateAsync(pPath, permission, attribute, CompleteNandOperation, &context);
    if (result == 0) {
        try {
            while (!OSTryWaitSemaphore(&context.mSemaphore)) {
                YieldFiber();
            }
            result = context.mResult;
        } catch (...) {
            OSWaitSemaphore(&context.mSemaphore);
            throw;
        }
    }
    return result;
}

s32 WaitNandOpen(const char* pPath, NANDFileInfo* pFile, u8 access) {
    CScopedActivity activity;
    NandWaitContext context;
    OSInitSemaphore(&context.mSemaphore, 0);
    s32 result = NANDOpenAsync(pPath, pFile, access, CompleteNandOperation, &context);
    if (result == 0) {
        try {
            while (!OSTryWaitSemaphore(&context.mSemaphore)) {
                YieldFiber();
            }
            result = context.mResult;
        } catch (...) {
            OSWaitSemaphore(&context.mSemaphore);
            throw;
        }
    }
    return result;
}

s32 WaitNandClose(NANDFileInfo* pFile) {
    CScopedActivity activity;
    NandWaitContext context;
    OSInitSemaphore(&context.mSemaphore, 0);
    s32 result = NANDCloseAsync(pFile, CompleteNandOperation, &context);
    if (result == 0) {
        try {
            while (!OSTryWaitSemaphore(&context.mSemaphore)) {
                YieldFiber();
            }
            result = context.mResult;
        } catch (...) {
            OSWaitSemaphore(&context.mSemaphore);
            throw;
        }
    }
    return result;
}

s32 WaitNandWrite(NANDFileInfo* pFile, const void* pData, u32 size) {
    CScopedActivity activity;
    NandWaitContext context;
    OSInitSemaphore(&context.mSemaphore, 0);
    s32 result = NANDWriteAsync(pFile, pData, size, CompleteNandOperation, &context);
    if (result == 0) {
        try {
            while (!OSTryWaitSemaphore(&context.mSemaphore)) {
                YieldFiber();
            }
            result = context.mResult;
        } catch (...) {
            OSWaitSemaphore(&context.mSemaphore);
            throw;
        }
    }
    return result;
}

s32 WaitNandCreateDir(const char* pPath, u8 permission, u8 attribute) {
    CScopedActivity activity;
    NandWaitContext context;
    OSInitSemaphore(&context.mSemaphore, 0);
    s32 result = NANDCreateDirAsync(pPath, permission, attribute, CompleteNandOperation, &context);
    if (result == 0) {
        try {
            while (!OSTryWaitSemaphore(&context.mSemaphore)) {
                YieldFiber();
            }
            result = context.mResult;
        } catch (...) {
            OSWaitSemaphore(&context.mSemaphore);
            throw;
        }
    }
    return result;
}

s32 WaitNandDelete(const char* pPath) {
    CScopedActivity activity;
    NandWaitContext context;
    OSInitSemaphore(&context.mSemaphore, 0);
    s32 result = NANDDeleteAsync(pPath, CompleteNandOperation, &context);
    if (result == 0) {
        try {
            while (!OSTryWaitSemaphore(&context.mSemaphore)) {
                YieldFiber();
            }
            result = context.mResult;
        } catch (...) {
            OSWaitSemaphore(&context.mSemaphore);
            throw;
        }
    }
    return result;
}

s32 WaitNandMove(const char* pPath, const char* pDestination) {
    CScopedActivity activity;
    NandWaitContext context;
    OSInitSemaphore(&context.mSemaphore, 0);
    s32 result = NANDMoveAsync(pPath, pDestination, CompleteNandOperation, &context);
    if (result == 0) {
        try {
            while (!OSTryWaitSemaphore(&context.mSemaphore)) {
                YieldFiber();
            }
            result = context.mResult;
        } catch (...) {
            OSWaitSemaphore(&context.mSemaphore);
            throw;
        }
    }
    return result;
}

s32 WaitNandCheck(u32 blocks, u32 inodes, u32* pAnswer) {
    CScopedActivity activity;
    NandWaitContext context;
    OSInitSemaphore(&context.mSemaphore, 0);
    s32 result = NANDCheckAsync(blocks, inodes, pAnswer, CompleteNandOperation, &context);
    if (result == 0) {
        try {
            while (!OSTryWaitSemaphore(&context.mSemaphore)) {
                YieldFiber();
            }
            result = context.mResult;
        } catch (...) {
            OSWaitSemaphore(&context.mSemaphore);
            throw;
        }
    }
    return result;
}

s32 WaitNandGetHomeDirUsage(u32* pBlocks, u32* pInodes) {
    CScopedActivity activity;
    NandWaitContext context;
    OSInitSemaphore(&context.mSemaphore, 0);
    s32 result = NANDGetHomeDirUsageAsync(pBlocks, pInodes, CompleteNandOperation, &context);
    if (result == 0) {
        try {
            while (!OSTryWaitSemaphore(&context.mSemaphore)) {
                YieldFiber();
            }
            result = context.mResult;
        } catch (...) {
            OSWaitSemaphore(&context.mSemaphore);
            throw;
        }
    }
    return result;
}
