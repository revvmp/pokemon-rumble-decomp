#ifndef GAME_CFILEDATAWORKER_H
#define GAME_CFILEDATAWORKER_H

#include <game/CFileData.h>
#include <map>
#include <nw4r/ut/ut_CntFileStream.h>
#include <nw4r/ut/ut_FileStream.h>
#include <vector>

class CFileData::CWorker {
public:
    class CJob {
    public:
        enum State {
            STATE_READ_COMPRESSED = 0,
            STATE_READ_RAW = 2,
            STATE_QUEUED = 3,
            STATE_WAIT_ARCHIVE = 5,
            STATE_DECOMPRESS = 6,
            STATE_OPEN_ARCHIVE = 8,
            STATE_PUBLISH_BUFFER = 9,
            STATE_FINISHED = 10
        };

        CJob() {}
        ~CJob() {}

        int mState;
        std::tr1::shared_ptr<nw4r::ut::CNTHandleWithMutex> mHandle;
        std::tr1::shared_ptr<nw4r::ut::FileStream> mStream;
        std::tr1::shared_ptr<char> mBuffer;
        u32 mReadSize;
        CJob* mpNext;
        CScopedActivity mActivity;
        std::tr1::shared_ptr<CFileData> mFile;
    };

    class BThread {
    public:
        BThread(CWorker* pWorker, int priority);
        void Push(CJob* pJob);
        void Stop();
        static void* Entry(void* pThread);

        CWorker* mpWorker;
        OSMutex mMutex;
        OSCond mCond;
        CJob* volatile mpHead;
        bool mStopping;
        std::tr1::shared_ptr<u8> mStack;
        OSThread mThread;

        virtual ~BThread();
        virtual void Run() = 0;
    };

    class LoaderThread : public BThread {
    public:
        LoaderThread(CWorker* pWorker, int priority) : BThread(pWorker, priority) {}
        virtual ~LoaderThread() {}
        virtual void Run();
    };

    class DecodeThread : public BThread {
    public:
        DecodeThread(CWorker* pWorker, int priority) : BThread(pWorker, priority) {}
        virtual ~DecodeThread() {}
        virtual void Run();

        u8 mWorkArea[0x9000];
    };

    class CFinishedQueue {
    public:
        void Wait();
        void WaitForReadyJob();

        OSMutex mMutex;
        OSCond mCond;
        CJob* volatile mpHead;
        int mYieldDepth;
        int mCount;

        CFinishedQueue() : mpHead(nullptr), mYieldDepth(0), mCount(0) {}
    };

    typedef std::map<nw4r::ut::CNTHandleWithMutex*, std::tr1::weak_ptr<nw4r::ut::CNTHandleWithMutex> > CNTHandleMap;
    typedef std::vector<CNTHandleMap> CNTHandleTable;

    CWorker();
    ~CWorker();

    void PushFinished(CJob* pJob);
    bool WaitInFiber(CFileData* pFile);
    void ProcessFinishedJobs();
    void DiscardJob(CJob* pJob);
    bool IsIdle() const { return mFinished.mCount == 0 && mFinished.mYieldDepth == 0; }

    LoaderThread mLoaderThread;
    DecodeThread mDecodeThread;
    CFinishedQueue mFinished;
    CNTHandleTable mHandleTable;
};

#endif
