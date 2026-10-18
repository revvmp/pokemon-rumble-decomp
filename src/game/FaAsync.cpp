#include <game/FaAsync.h>

#include <game/CScopedActivity.h>

void YieldFiber();

static void CompleteFaOperation(s32 result, FACommandBlock* pBlock) {
    FaWaitContext* pContext = static_cast<FaWaitContext*>(pBlock);
    pContext->mResult = result;
    OSSignalSemaphore(&pContext->mSemaphore);
}

s32 WaitFaAttach(u8 device, s8* pNandPath, u32 nandSize, FADrvTbl* pDrivers) {
    CScopedActivity activity;
    FaWaitContext context;
    OSInitSemaphore(&context.mSemaphore, 0);
    s32 result = FAAttachAsync(device, pNandPath, nandSize, pDrivers, CompleteFaOperation, &context);
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

s32 WaitFaMount(s8 drive) {
    CScopedActivity activity;
    FaWaitContext context;
    OSInitSemaphore(&context.mSemaphore, 0);
    s32 result = FAMountAsync(drive, CompleteFaOperation, &context);
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

s32 WaitFaUnmount(s8 drive, u32 mode) {
    CScopedActivity activity;
    FaWaitContext context;
    OSInitSemaphore(&context.mSemaphore, 0);
    s32 result = FAUnmountAsync(drive, mode, CompleteFaOperation, &context);
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

s32 WaitFaCreate(const char* pPath, s32 mode, FAFile** ppFile) {
    CScopedActivity activity;
    FaWaitContext context;
    OSInitSemaphore(&context.mSemaphore, 0);
    s32 result = FACreateAsync(pPath, mode, ppFile, CompleteFaOperation, &context);
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

s32 WaitFaFwrite(void* pBuffer, u32 size, u32 count, FAFile* pFile, u32* pWritten) {
    CScopedActivity activity;
    FaWaitContext context;
    OSInitSemaphore(&context.mSemaphore, 0);
    s32 result = FAFwriteAsync(pBuffer, size, count, pFile, pWritten, CompleteFaOperation, &context);
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

s32 WaitFaFsfirst(const char* pPath, u8 attributes, FADta* pDta) {
    CScopedActivity activity;
    FaWaitContext context;
    OSInitSemaphore(&context.mSemaphore, 0);
    s32 result = FAFsfirstAsync(pPath, attributes, pDta, CompleteFaOperation, &context);
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

s32 WaitFaFsnext(FADta* pDta) {
    CScopedActivity activity;
    FaWaitContext context;
    OSInitSemaphore(&context.mSemaphore, 0);
    s32 result = FAFsnextAsync(pDta, CompleteFaOperation, &context);
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

s32 WaitFaRemove(const char* pPath) {
    CScopedActivity activity;
    FaWaitContext context;
    OSInitSemaphore(&context.mSemaphore, 0);
    s32 result = FARemoveAsync(pPath, CompleteFaOperation, &context);
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

s32 WaitFaMkdir(const char* pPath) {
    CScopedActivity activity;
    FaWaitContext context;
    OSInitSemaphore(&context.mSemaphore, 0);
    s32 result = FAMkdirAsync(pPath, CompleteFaOperation, &context);
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

s32 WaitFaRmdir(const char* pPath) {
    CScopedActivity activity;
    FaWaitContext context;
    OSInitSemaphore(&context.mSemaphore, 0);
    s32 result = FARmdirAsync(pPath, CompleteFaOperation, &context);
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
