
#include <game/CFileData.h>

#include <game/CFiber.h>
#include <game/CFileDataWorker.h>
#include <game/FileList.h>
#include <game/Memory.h>
#include <game/ResourceFileConfig.h>
#include <nw4r/g3d/res/g3d_resfile.h>
#include <nw4r/ut/ut_CntFileStream.h>
#include <nw4r/ut/ut_NandFileStream.h>
#include <revolution/arc.h>
#include <revolution/cx.h>
#include <revolution/os/OSCache.h>
#include <revolution/os/OSError.h>
#include <string.h>

struct FileLoadParam {
    const char* mpPath;
    u32 mFlags;
};

std::tr1::shared_ptr<FileList> LoadFiles(const FileLoadParam* pFiles, unsigned int count, bool background);

static std::map<std::string, std::tr1::weak_ptr<CFileData> > sFileCache;

static std::tr1::shared_ptr<CFileData::CWorker> sWorker;

static ResourceFileConfig sConfig;

CFileData::CFileData() : mFlags(0), mSize(0), mData(nullptr), mPending(false), mError(0) {}

inline CFileData::~CFileData() {
    if ((mFlags & FLAG_KIND_MASK) == FLAG_KIND_RESOURCE_READY) {
        nw4r::g3d::ResFile resFile(mData);
        resFile.Terminate();
    }
}

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

bool WaitFileData(CFileData* pFile, bool background) {
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


inline u32 AlignUp(u32 value, u32 alignment) {
    int mask = ~(alignment - 1);
    return (alignment + value - 1) & mask;
}

inline void* AllocateStagingBuffer(u32 size, int alignment, u32 flags) {
    if (alignment < 4) {
        alignment = 4;
    }
    if (flags & CFileData::FLAG_TOP_DOWN) {
        alignment = -alignment;
    }
    void* pMemory;
    if ((flags & CFileData::FLAG_PLACEMENT_MASK) == CFileData::FLAG_PLACEMENT_MEM1) {
        pMemory = GameMemory::TryAllocateMEM2(size, -alignment);
        if (pMemory == nullptr) {
            pMemory = GameMemory::TryAllocateMEM1(size, -alignment);
        }
    } else {
        pMemory = GameMemory::TryAllocateMEM1(size, -alignment);
        if (pMemory == nullptr) {
            pMemory = GameMemory::TryAllocateMEM2(size, -alignment);
        }
    }
    return pMemory;
}

inline std::tr1::shared_ptr<nw4r::ut::CNTHandleWithMutex> AcquireCNTHandle(int device, long& rError, CFileData::CWorker::CNTHandleTable& rTable) {
    if (rTable.size() <= static_cast<unsigned long>(device)) {
        rTable.resize(device + 1);
    }
    CFileData::CWorker::CNTHandleMap& rMap = rTable[device];
    std::tr1::shared_ptr<nw4r::ut::CNTHandleWithMutex> handle;
    CFileData::CWorker::CNTHandleMap::iterator it = rMap.begin();
    while (it != rMap.end()) {
        std::tr1::shared_ptr<nw4r::ut::CNTHandleWithMutex> locked = it->second.lock();
        if (locked) {
            if (!handle && locked.get()->TryLock()) {
                handle = locked;
            }
            ++it;
        } else {
            CFileData::CWorker::CNTHandleMap::iterator expired = it;
            rMap.erase(expired);
            it = expired;
        }
    }
    if (handle) {
        return handle;
    }

    handle.reset(new nw4r::ut::CNTHandleWithMutex);
    rError = handle.get()->Init(device, GetDefaultAllocator());
    if (rError != 0) {
        return 0;
    }
    rMap.insert(CFileData::CWorker::CNTHandleMap::value_type(handle.operator->(), std::tr1::weak_ptr<nw4r::ut::CNTHandleWithMutex>(handle)));
    handle.get()->Lock();
    return handle;
}

inline std::tr1::shared_ptr<nw4r::ut::FileStream> OpenFileStream(const CPath& rPath, u32 flags, long& rError, bool quietNotFound,
                                                                 CFileData::CWorker::CNTHandleTable& rTable,
                                                                 std::tr1::shared_ptr<nw4r::ut::CNTHandleWithMutex>& rHandle) {
    int device = rPath.GetDevice();
    if (device == CPath::DEVICE_DVD) {
        OSPanic("CFileData.cpp", 1170, "NANDAPP");
    } else if (device == CPath::DEVICE_NAND) {
        std::tr1::shared_ptr<nw4r::ut::NandFileStream> stream(new nw4r::ut::NandFileStream);
        bool opened = false;
        if (rPath.GetComponents().front().size() <= 12) {
            std::string fullPath = rPath.ToString(false);
            if (stream.get()->Open(fullPath.c_str(), 1)) {
                opened = true;
            }
        }
        if (opened) {
            return stream;
        }
        rError = CFileData::ERROR_NOT_FOUND;
    } else if (device >= 2 && device < 512) {
        std::tr1::shared_ptr<nw4r::ut::CNTHandleWithMutex> handle = AcquireCNTHandle(device, rError, rTable);
        if (handle) {
            std::tr1::shared_ptr<nw4r::ut::CntFileStream> stream(new nw4r::ut::CntFileStream);
            bool opened = stream.get()->Open(handle.get(), rPath.ToString(false).c_str());
            OSUnlockMutex(&handle.get()->mMutex);
            if (opened) {
                rHandle = handle;
                return stream;
            }
            rError = CFileData::ERROR_NOT_FOUND;
        }
    } else if ((device >= 512 && device < 538) || device == 539) {
        rError = CFileData::ERROR_NOT_FOUND;
    }

    if (!quietNotFound || rError != CFileData::ERROR_NOT_FOUND) {
        sConfig.mErrorHandler(static_cast<CPath::Drive>(rPath.GetDevice()), rPath.ToString(false).c_str(), rError, flags);
    }
    return std::tr1::shared_ptr<nw4r::ut::FileStream>();
}

inline CFileData::CWorker::BThread::BThread(CWorker* pWorker, int priority) : mpHead(nullptr), mpWorker(pWorker), mStopping(false) {
    OSInitMutex(&mMutex);
    OSInitCond(&mCond);
    void* pStack = GameMemory::Allocate(sConfig.mStackSize, -32);
    mStack.reset(static_cast<u8*>(pStack), GameMemory::Free);
    OSCreateThread(&mThread, Entry, this, mStack.get() + sConfig.mStackSize, sConfig.mStackSize, priority, 0);
    OSResumeThread(&mThread);
}

inline void CFileData::CWorker::BThread::Stop() {
    OSLockMutex(&mMutex);
    while (mpHead) {
        CJob* pJob = mpHead;
        mpHead = mpHead->mpNext;
        pJob->mpNext = nullptr;
        pJob->mState = CJob::STATE_FINISHED;
        mpWorker->PushFinished(pJob);
    }
    OSUnlockMutex(&mMutex);
    if (!mStopping) {
        OSLockMutex(&mMutex);
        mStopping = true;
        OSSignalCond(&mCond);
        OSUnlockMutex(&mMutex);
        OSJoinThread(&mThread, nullptr);
    }
}

CFileData::CWorker::BThread::~BThread() {
    Stop();
}

void* CFileData::CWorker::BThread::Entry(void* pThread) {
    static_cast<BThread*>(pThread)->Run();
}

inline CFileData::CWorker::CWorker() : mLoaderThread(this, sConfig.mReadPriority), mDecodeThread(this, sConfig.mDecodePriority) {
    OSInitMutex(&mFinished.mMutex);
    OSInitCond(&mFinished.mCond);
}

inline CFileData::CWorker::~CWorker() {
    mLoaderThread.Stop();
    mDecodeThread.Stop();
    while (mFinished.mpHead) {
        CJob* pJob = mFinished.mpHead;
        mFinished.mpHead = mFinished.mpHead->mpNext;
        if (pJob->mFile) {
            pJob->mFile->mBuffer.reset();
            pJob->mFile->mData = nullptr;
            pJob->mFile->mError = ERROR_SHUTDOWN;
        }
        delete pJob;
        --mFinished.mCount;
    }
}

void CFileData::CWorker::PushFinished(CJob* pJob) {
    OSLockMutex(&mFinished.mMutex);
    if (mFinished.mpHead) {
        CJob* pTail = mFinished.mpHead;
        while (pTail->mpNext) {
            pTail = pTail->mpNext;
        }
        pTail->mpNext = pJob;
    } else {
        mFinished.mpHead = pJob;
    }
    OSSignalCond(&mFinished.mCond);
    OSUnlockMutex(&mFinished.mMutex);
}

std::tr1::shared_ptr<CFileData> OpenFileData(const char* pPath, u32 flags) {
    CPath path;
    path.Set(pPath);
    long error;
    return OpenFileData(path, flags, error);
}

inline std::tr1::shared_ptr<CFileData> FindLoadedFile(const CPath& rPath) {
    std::map<std::string, std::tr1::weak_ptr<CFileData> >::iterator found = sFileCache.find(rPath.ToString());
    if (found != sFileCache.end() && !found->second.expired()) {
        std::tr1::shared_ptr<CFileData> locked = found->second.lock();
        return locked;
    }
    return std::tr1::shared_ptr<CFileData>();
}

std::tr1::shared_ptr<CFileData> OpenFileData(const CPath& rPath, u32 flags, long& rError) {
    {
        std::tr1::shared_ptr<CFileData> cached = FindLoadedFile(rPath);
        if (cached) {
            rError = 0;
            return cached;
        }
    }

    std::tr1::shared_ptr<CFileData> archive;
    if (rPath.GetComponents().size() >= 2) {
        CPath archivePath(rPath);
        {
            std::list<std::string> components(archivePath.GetComponents());
            components.pop_back();
            while (components.size() != 0) {
                const std::string& rName = components.back();
                if (rName.size() > 4 && strcmp(rName.c_str() + rName.size() - 4, ".arc") == 0) {
                    break;
                }
                components.pop_back();
            }
            archivePath.SetComponents(components);
        }
        if (archivePath.GetComponents().size() != 0) {
            archive = OpenFileData(archivePath, flags, rError);
            if (!archive) {
                sConfig.mErrorHandler(static_cast<CPath::Drive>(rPath.GetDevice()), rPath.ToString(false).c_str(), rError, flags);
                return std::tr1::shared_ptr<CFileData>();
            }

            std::tr1::shared_ptr<CFileData> file(new CFileData);
            u32 kind = CFileData::FLAG_KIND_RESOURCE;
            if (flags & CFileData::FLAG_RAW) {
                kind = 0;
            }
            file->mFlags = flags | kind;
            file->SetDrive(static_cast<CPath::Drive>(rPath.GetDevice()));
            file->mPath.assign(rPath.ToString());
            file->mSize = 0;
            file->mData = nullptr;
            file->mArchive = archive;
            if (archive->IsLoaded()) {
                if (!file.get()->OpenArchiveEntry()) {
                    rError = CFileData::ERROR_ARCHIVE_OPEN;
                    sConfig.mErrorHandler(static_cast<CPath::Drive>(rPath.GetDevice()), rPath.ToString(false).c_str(), rError, flags);
                    return std::tr1::shared_ptr<CFileData>();
                }
                sFileCache[rPath.ToString()] = file;
                rError = 0;
                return file;
            }

            CFileData::CWorker::CJob* pJob = new CFileData::CWorker::CJob;
            pJob->mpNext = nullptr;
            pJob->mFile = file;
            pJob->mFile->mPending = true;
            pJob->mState = CFileData::CWorker::CJob::STATE_WAIT_ARCHIVE;
            sFileCache[rPath.ToString()] = file;
            if (!sWorker) {
                sWorker.reset(new CFileData::CWorker);
            }
            ++sWorker.get()->mFinished.mCount;
            sWorker.get()->PushFinished(pJob);
            rError = 0;
            return file;
        }
    }

    std::tr1::shared_ptr<nw4r::ut::CNTHandleWithMutex> handle;
    std::tr1::shared_ptr<nw4r::ut::FileStream> stream;
    CFileData::CWorker::CNTHandleTable handleTable;
    if (sWorker) {
        handleTable = sWorker.get()->mHandleTable;
    }

    bool compressed = true;
    int state = CFileData::CWorker::CJob::STATE_READ_RAW;
    if (!stream) {
        state = CFileData::CWorker::CJob::STATE_READ_COMPRESSED;
        CPath compressedPath(rPath);
        {
            std::list<std::string> components(compressedPath.GetComponents());
            components.back() += ".cx";
            compressedPath.SetComponents(components);
        }
        stream = OpenFileStream(compressedPath, flags, rError, true, handleTable, handle);
    }
    if (!stream) {
        state = CFileData::CWorker::CJob::STATE_READ_RAW;
        stream = OpenFileStream(rPath, flags, rError, false, handleTable, handle);
        compressed = false;
    }
    if (!stream) {
        return std::tr1::shared_ptr<CFileData>();
    }

    std::tr1::shared_ptr<CFileData> file(new CFileData);
    u32 kind = CFileData::FLAG_KIND_RESOURCE;
    if (flags & CFileData::FLAG_RAW) {
        kind = 0;
    }
    file->mFlags = flags | kind;
    file->SetDrive(static_cast<CPath::Drive>(rPath.GetDevice()));
    file->mPath.assign(rPath.ToString());
    file->mSize = stream.get()->GetSize();
    file->mData = nullptr;
    u32 alignedSize = AlignUp(file->mSize, stream.get()->GetSizeAlign());
    if (!compressed) {
        file->mBuffer.reset(static_cast<char*>(CFileData::AllocateByPlacement(alignedSize, std::max<int>(stream.get()->GetBufferAlign(), 32), flags)), GameMemory::Free);
        if (!file->mBuffer) {
            rError = CFileData::ERROR_OUT_OF_MEMORY;
            sConfig.mErrorHandler(static_cast<CPath::Drive>(rPath.GetDevice()), rPath.ToString(false).c_str(), rError, flags);
            return std::tr1::shared_ptr<CFileData>();
        }
    }

    CFileData::CWorker::CJob* pJob = new CFileData::CWorker::CJob;
    pJob->mpNext = nullptr;
    pJob->mFile = file;
    pJob->mFile->mPending = true;
    pJob->mHandle = handle;
    pJob->mStream = stream;
    pJob->mState = state;
    if (compressed) {
        pJob->mBuffer.reset(static_cast<char*>(AllocateStagingBuffer(alignedSize + stream.get()->GetSizeAlign(), stream.get()->GetBufferAlign(), flags)), GameMemory::Free);
        if (!pJob->mBuffer) {
            delete pJob;
            rError = CFileData::ERROR_OUT_OF_MEMORY;
            sConfig.mErrorHandler(static_cast<CPath::Drive>(rPath.GetDevice()), rPath.ToString(false).c_str(), rError, flags);
            return std::tr1::shared_ptr<CFileData>();
        }
        file->mBuffer.reset();
    }

    sFileCache[rPath.ToString()] = file;
    if (!sWorker) {
        sWorker.reset(new CFileData::CWorker);
    }
    sWorker.get()->mHandleTable = handleTable;
    ++sWorker.get()->mFinished.mCount;
    sWorker.get()->mLoaderThread.Push(pJob);
    rError = 0;
    return file;
}

std::tr1::shared_ptr<CFileData> CreateFileData(const char* pPath, u32 flags, bool background) {
    std::tr1::shared_ptr<CFileData> file = OpenFileData(pPath, flags);
    if (file) {
        if (!WaitFileData(file.get(), background)) {
            file.reset();
        }
    }
    return file;
}

std::tr1::shared_ptr<FileList> LoadFiles(const FileLoadParam* pFiles, unsigned int count, bool background) {
    std::list<std::tr1::shared_ptr<CFileData> > opened;
    while (count != 0 && pFiles->mpPath != nullptr) {
        std::tr1::shared_ptr<CFileData> file = OpenFileData(pFiles->mpPath, pFiles->mFlags);
        if (file) {
            opened.push_back(file);
        }
        --count;
        ++pFiles;
    }

    std::tr1::shared_ptr<FileList> loaded(new LoadedFileData);
    for (std::list<std::tr1::shared_ptr<CFileData> >::iterator it = opened.begin(); it != opened.end(); ++it) {
        if (WaitFileData(it->get(), background)) {
            loaded->push_back(*it);
        }
    }
    if (loaded->size() == 0) {
        return std::tr1::shared_ptr<FileList>();
    }
    return loaded;
}

void DefaultFileErrorHandler(CPath::Drive drive, const char* pPath, long error, unsigned long flags);

ResourceFileConfig::ResourceFileConfig() :
    mErrorHandler(DefaultFileErrorHandler), mReadPriority(20), mDecodePriority(22), mStackSize(0x4000), mUseDriveWorker(true) {}

void InitializeResourceFiles(const ResourceFileConfig& rConfig) {
    sConfig = rConfig;
}

inline void RemoveExpiredFiles() {
    std::map<std::string, std::tr1::weak_ptr<CFileData> >::iterator end = sFileCache.end();
    std::map<std::string, std::tr1::weak_ptr<CFileData> >::iterator it = sFileCache.begin();
    while (it != end) {
        std::map<std::string, std::tr1::weak_ptr<CFileData> >::iterator next = it;
        ++next;
        if (it->second.expired()) {
            sFileCache.erase(it);
        }
        it = next;
    }
}

void UpdateResourceFiles() {
    if (sWorker) {
        sWorker.get()->ProcessFinishedJobs();
        if (sWorker.get()->IsIdle()) {
            sWorker.reset();
        }
        RemoveExpiredFiles();
    }
}

void ShutdownResourceFiles() {
    sWorker.reset();
    RemoveExpiredFiles();
}

void DefaultFileErrorHandler(CPath::Drive drive, const char* pPath, long error, unsigned long flags) {
    if ((flags & CFileData::FLAG_ALLOW_OUT_OF_MEMORY) && error == CFileData::ERROR_OUT_OF_MEMORY) {
        return;
    }
    switch (drive) {
    case CPath::DEVICE_NAND:
        switch (error) {
        case -4:
            OSPanic("CFileData.cpp", 1563, "fatal error: NAND_08");
            break;
        case -9:
            OSPanic("CFileData.cpp", 1566, "fatal error: NAND_05");
            break;
        case -11:
            OSPanic("CFileData.cpp", 1569, "fatal error: NAND_06");
            break;
        case -3:
        case -2:
            OSPanic("CFileData.cpp", 1573, "fatal error: NAND_11");
            break;
        case -64:
            OSPanic("CFileData.cpp", 1576, "fatal error: NAND_98");
            break;
        }
        if (!(flags & CFileData::FLAG_OPTIONAL)) {
            OSPanic("CFileData.cpp", 1580, "fatal error: WARE_01_01");
        }
        break;
    case 2 ... 511:
        switch (error) {
        case -5013:
        case -5012:
            OSPanic("CFileData.cpp", 1588, "fatal error: WARE_99(if shared) or WARE_01_01(not shared)");
            break;
        case -5011:
            OSPanic("CFileData.cpp", 1593, "fatal error: NAND_08");
            break;
        }
        if (!(flags & CFileData::FLAG_OPTIONAL)) {
            OSPanic("CFileData.cpp", 1597, "fatal error: WARE_99(if shared) or WARE_01_01(not shared)");
        }
        break;
    case CPath::DEVICE_MCS_BASE ... CPath::DEVICE_MCS_LAST:
    case CPath::DEVICE_UNC:
        if (!(flags & CFileData::FLAG_OPTIONAL)) {
            OSPanic("CFileData.cpp", 1606, "fatal error");
        }
        break;
    }
}

void CFileData::CWorker::DecodeThread::Run() {
    CJob* pJob;
    do {
        pJob = nullptr;
        OSLockMutex(&mMutex);
        while (mpHead == nullptr && !mStopping) {
            OSWaitCond(&mCond, &mMutex);
        }
        if (!mStopping) {
            pJob = mpHead;
            mpHead = mpHead->mpNext;
            pJob->mpNext = nullptr;
        }
        OSUnlockMutex(&mMutex);
        if (pJob == nullptr) {
            break;
        }
        if (pJob->mState == CJob::STATE_QUEUED) {
            u8* pSource = reinterpret_cast<u8*>(pJob->mBuffer.get());
            u32 sourceSize = pJob->mReadSize;
            u8* pDestination = reinterpret_cast<u8*>(pJob->mFile->mBuffer.get());
            int result = -1;
            switch (*pSource & 0xF0) {
            case 0x30:
                result = CXSecureUncompressRL(pSource, sourceSize, pDestination);
                break;
            case 0x10:
                result = CXSecureUncompressLZ(pSource, sourceSize, pDestination);
                break;
            case 0x20:
                result = CXSecureUncompressHuffman(pSource, sourceSize, pDestination);
                break;
            case 0x40:
                memset(mWorkArea, 0, sizeof(mWorkArea));
                result = CXSecureUncompressLH(pSource, sourceSize, pDestination, mWorkArea);
                break;
            case 0x50:
                memset(mWorkArea, 0, sizeof(mWorkArea));
                result = CXSecureUncompressLRC(pSource, sourceSize, pDestination, mWorkArea);
                break;
            case 0x80:
                result = CXSecureUnfilterDiff(pSource, sourceSize, pDestination);
                break;
            }
            if (result == 0) {
                pJob->mState = CJob::STATE_PUBLISH_BUFFER;
            } else {
                pJob->mState = CJob::STATE_FINISHED;
                pJob->mFile->mError = ERROR_DECOMPRESS;
            }
            mpWorker->PushFinished(pJob);
        } else {
            pJob->mState = CJob::STATE_FINISHED;
            pJob->mFile->mError = ERROR_BAD_DECODE_JOB;
            mpWorker->PushFinished(pJob);
        }
    } while (pJob != nullptr);
}

void CFileData::CWorker::LoaderThread::Run() {
    CJob* pJob;
    do {
        pJob = nullptr;
        OSLockMutex(&mMutex);
        while (mpHead == nullptr && !mStopping) {
            OSWaitCond(&mCond, &mMutex);
        }
        if (!mStopping) {
            pJob = mpHead;
            mpHead = mpHead->mpNext;
            pJob->mpNext = nullptr;
        }
        OSUnlockMutex(&mMutex);
        if (pJob == nullptr) {
            break;
        }
        switch (pJob->mState) {
        default:
            pJob->mState = CJob::STATE_FINISHED;
            pJob->mFile->mError = ERROR_BAD_JOB;
            mpWorker->PushFinished(pJob);
            break;
        case CJob::STATE_READ_COMPRESSED:
        case CJob::STATE_READ_COMPRESSED + 1: {
            u32 alignment = pJob->mStream->GetSizeAlign();
            s32 size = pJob->mStream->Read(pJob->mBuffer.get(), AlignUp(pJob->mFile->mSize, alignment));
            if (size > 0) {
                pJob->mState += 6;
                pJob->mReadSize = size;
            } else {
                pJob->mState = CJob::STATE_FINISHED;
                pJob->mFile->mError = size;
            }
            mpWorker->PushFinished(pJob);
            break;
        }
        case CJob::STATE_READ_RAW: {
            u32 alignment = pJob->mStream->GetSizeAlign();
            s32 size = pJob->mStream->Read(pJob->mFile->mBuffer.get(), AlignUp(pJob->mFile->mSize, alignment));
            if (size > 0 || (pJob->mFile->mFlags & CFileData::FLAG_ALLOW_EMPTY)) {
                pJob->mState = CJob::STATE_PUBLISH_BUFFER;
            } else {
                pJob->mState = CJob::STATE_FINISHED;
                pJob->mFile->mError = size;
            }
            mpWorker->PushFinished(pJob);
            break;
        }
        }
    } while (pJob != nullptr);
}
