#include <game/CFileData.h>

#include <game/CFiber.h>
#include <game/CFileDataWorker.h>
#include <game/Memory.h>
#include <game/ResourceFileConfig.h>
#include <nw4r/g3d/res/g3d_resfile.h>
#include <revolution/arc.h>
#include <revolution/cx/CXUncompression.h>
#include <revolution/os/OSCache.h>

static ResourceFileConfig sConfig;

static std::tr1::shared_ptr<CFileData::CWorker> sWorker;

CFileData::CFileData() : mFlags(0), mSize(0), mData(nullptr), mPending(false), mError(0) {}

inline void CFileData::CWorker::CFinishedQueue::WaitForReadyJob() {
    for (;;) {
        if (mpHead) {
            for (CJob* pJob = mpHead; pJob != nullptr; pJob = pJob->mpNext) {
                if (pJob->mState != CJob::STATE_WAIT_ARCHIVE || !pJob->mFile->mArchive->mPending) {
                    return;
                }
            }
        }
        OSWaitCond(&mCond, &mMutex);
    }
}

inline void CFileData::CWorker::CFinishedQueue::Wait() {
    OSLockMutex(&mMutex);
    WaitForReadyJob();
    OSUnlockMutex(&mMutex);
}

inline void* CFileData::TryAllocateByPlacement(u32 size, int alignment, u32 flags) {
    void* pMemory;
    switch (flags & FLAG_PLACEMENT_MASK) {
    default:
        pMemory = GameMemory::TryAllocateMEM2(size, alignment);
        break;
    case FLAG_PLACEMENT_MEM1:
        pMemory = GameMemory::TryAllocateMEM1(size, alignment);
        break;
    case FLAG_PLACEMENT_PREFER_MEM2:
        pMemory = GameMemory::TryAllocateMEM2(size, alignment);
        if (pMemory == nullptr) {
            pMemory = GameMemory::TryAllocateMEM1(size, alignment);
        }
        break;
    case FLAG_PLACEMENT_PREFER_MEM1:
        pMemory = GameMemory::TryAllocateMEM1(size, alignment);
        if (pMemory == nullptr) {
            pMemory = GameMemory::TryAllocateMEM2(size, alignment);
        }
        break;
    }
    return pMemory;
}

inline void CFileData::ValidateResource() {
    if ((mFlags & FLAG_KIND_MASK) != FLAG_KIND_RESOURCE) {
        return;
    }

    if (mSize >= 4 && *static_cast<u32*>(mData) == 'bres') {
        nw4r::g3d::ResFile resFile(mData);
        if (!resFile.CheckRevision()) {
            mBuffer.reset();
            mData = nullptr;
        } else {
            mFlags = (mFlags & ~FLAG_KIND_MASK) | FLAG_KIND_RESOURCE_READY;
            resFile.Init();
            DCFlushRange(mData, mSize);
        }
    } else {
        DCFlushRange(mData, mSize);
    }
}

inline void CFileData::CWorker::DiscardJob(CJob* pJob) {
    delete pJob;
    --mFinished.mCount;
}

inline void CFileData::CWorker::ProcessFinishedJobs() {
    CJob* pDeferred = nullptr;
    for (;;) {
        CJob* pJob = nullptr;
        OSLockMutex(&mFinished.mMutex);
        if (mFinished.mpHead) {
            pJob = mFinished.mpHead;
            mFinished.mpHead = mFinished.mpHead->mpNext;
            pJob->mpNext = nullptr;
        }
        if (!pJob) {
            mFinished.mpHead = pDeferred;
            OSSignalCond(&mFinished.mCond);
            OSUnlockMutex(&mFinished.mMutex);
            break;
        }
        OSUnlockMutex(&mFinished.mMutex);

        switch (pJob->mState) {
        default:
            pJob->mState = CJob::STATE_FINISHED;
            pJob->mFile->mError = ERROR_BAD_STATE;
            break;
        case CJob::STATE_DECOMPRESS: {
            const u8* pSource = reinterpret_cast<const u8*>(pJob->mBuffer.get());
            int type = *pSource & 0xF0;
            if (type != 0x30 && type != 0x10 && type != 0x20 && type != 0x40 && type != 0x50 && type != 0x80) {
                pJob->mState = CJob::STATE_FINISHED;
                pJob->mFile->mError = ERROR_BAD_COMPRESSION;
                break;
            }
            pJob->mFile->mSize = CXGetUncompressedSize(pSource);
            CFileData* pTarget = pJob->mFile.get();
            u32 flags = pTarget->mFlags;
            u32 size = pTarget->mSize;
            int alignment = 32;
            if (flags & FLAG_TOP_DOWN) {
                alignment = -32;
            }
            char* pBuffer = static_cast<char*>(TryAllocateByPlacement(size, alignment, flags));
            pJob->mFile->mBuffer.reset(pBuffer, GameMemory::Free);
            if (!pJob->mFile->mBuffer) {
                pJob->mState = CJob::STATE_FINISHED;
                pJob->mFile->mError = ERROR_OUT_OF_MEMORY;
                break;
            }
            pJob->mState = CJob::STATE_QUEUED;
            mDecodeThread.Push(pJob);
            continue;
        }
        case CJob::STATE_WAIT_ARCHIVE:
            if (pJob->mFile->mArchive->mPending) {
                if (pDeferred) {
                    CJob* pTail = pDeferred;
                    while (pTail->mpNext) {
                        pTail = pTail->mpNext;
                    }
                    pTail->mpNext = pJob;
                } else {
                    pDeferred = pJob;
                }
                continue;
            }
            if (pJob->mFile->mArchive->IsLoaded()) {
                pJob->mState = CJob::STATE_OPEN_ARCHIVE;
            } else {
                pJob->mState = CJob::STATE_FINISHED;
                pJob->mFile->mError = pJob->mFile->mArchive->mError;
                break;
            }
        case CJob::STATE_OPEN_ARCHIVE:
            if (!pJob->mFile->OpenArchiveEntry()) {
                pJob->mState = CJob::STATE_FINISHED;
                pJob->mFile->mError = ERROR_ARCHIVE_OPEN;
                break;
            }
            pJob->mFile->mPending = false;
            DiscardJob(pJob);
            continue;
        case CJob::STATE_PUBLISH_BUFFER:
            pJob->mFile->mData = pJob->mFile->mBuffer.get();
            pJob->mFile->ValidateResource();
            pJob->mFile->mPending = false;
            DiscardJob(pJob);
            continue;
        case CJob::STATE_FINISHED:
            break;
        }

        pJob->mFile->mBuffer.reset();
        pJob->mFile->mData = nullptr;
        pJob->mFile->mPending = false;
        DiscardJob(pJob);
    }
}

bool CFileData::WaitUntilLoaded(CFileData* pFile) {
    CWorker* pWorker = sWorker.get();
    if (sWorker) {
        while (pFile->mPending) {
            pWorker->mFinished.Wait();
            pWorker->ProcessFinishedJobs();
        }

        if (pFile->IsLoaded()) {
            return true;
        }

        sConfig.mErrorHandler(static_cast<CPath::Drive>(pFile->mFlags & FLAG_DRIVE_MASK), pFile->mPath.c_str(),
                              pFile->mError, pFile->mFlags);
        return false;
    }
    return pFile->IsLoaded();
}

void* CFileData::AllocateByPlacement(u32 size, int alignment, u32 flags) {
    if (alignment < 4) {
        alignment = 4;
    }
    if (flags & FLAG_TOP_DOWN) {
        alignment = -alignment;
    }
    return TryAllocateByPlacement(size, alignment, flags);
}

void CFileData::CWorker::BThread::Push(CJob* pJob) {
    OSLockMutex(&mMutex);
    if (mpHead) {
        CJob* pTail = mpHead;
        while (pTail->mpNext) {
            pTail = pTail->mpNext;
        }
        pTail->mpNext = pJob;
    } else {
        mpHead = pJob;
    }
    OSSignalCond(&mCond);
    OSUnlockMutex(&mMutex);
}

bool CFileData::OpenArchiveEntry() {
    ARCHandle handle;
    if (!ARCInitHandle(mArchive->mData, &handle)) {
        return false;
    }

    ARCFileInfo info;
    std::string name = mPath.substr(mArchive->mPath.size() + 1);
    if (!ARCOpen(&handle, name.c_str(), &info)) {
        ARCClose(&info);
        return false;
    }

    mData = ARCGetStartAddrInMem(&info);
    mSize = ARCGetLength(&info);
    ValidateResource();
    return true;
}

static bool WaitFileData(CFileData* pFile, bool background) {
    if (!background) {
        return CFileData::WaitUntilLoaded(pFile);
    }
    if (sWorker) {
        return sWorker.get()->WaitInFiber(pFile);
    }
    return pFile->IsLoaded();
}

bool CFileData::CWorker::WaitInFiber(CFileData* pFile) {
    while (pFile->mPending) {
        ProcessFinishedJobs();
        ++mFinished.mYieldDepth;
        YieldFiber();
        --mFinished.mYieldDepth;
    }

    if (pFile->IsLoaded()) {
        return true;
    }

    sConfig.mErrorHandler(static_cast<CPath::Drive>(pFile->mFlags & FLAG_DRIVE_MASK), pFile->mPath.c_str(),
                          pFile->mError, pFile->mFlags);
    return false;
}

void CFileData::SetDrive(CPath::Drive drive) {
    mFlags = (mFlags & ~FLAG_DRIVE_MASK) | (drive & FLAG_DRIVE_MASK);
}

