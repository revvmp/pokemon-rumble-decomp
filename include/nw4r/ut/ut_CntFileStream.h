#ifndef NW4R_UT_CNT_FILE_STREAM_H
#define NW4R_UT_CNT_FILE_STREAM_H
#include <nw4r/ut/ut_FileStream.h>
#include <revolution/cnt.h>
#include <revolution/os/OSMutex.h>

namespace nw4r {
namespace ut {
class CNTHandleWithMutex {
public:
    class ScopedLock {
    public:
        explicit ScopedLock(CNTHandleWithMutex& rHandle)
            : mHandle(rHandle) {
            OSLockMutex(&mHandle.mMutex);
        }
        ~ScopedLock() {
            OSUnlockMutex(&mHandle.mMutex);
        }
    private:
        ScopedLock(const ScopedLock&);
        ScopedLock& operator=(const ScopedLock&);
        CNTHandleWithMutex& mHandle;
    };

    CNTHandleWithMutex();
    ~CNTHandleWithMutex();
    void Init(int contentNum, MEMAllocator* pAllocator);

    bool mInitialized;
    OSMutex mMutex;
    CNTHandle mHandle;
};

class CntFileStream : public FileStream {
public:
    NW4R_UT_RTTI_DECL(CntFileStream);

    CntFileStream();
    virtual ~CntFileStream();

    bool Open(CNTHandleWithMutex* pHandle, const char* pPath);
    virtual void Close();
    virtual s32 Read(void* pDst, u32 size);
    virtual void Seek(s32 offset, u32 origin);
    virtual u32 Tell() const {
        return mFilePosition.Tell();
    }
    virtual u32 GetSize() const {
        return mFilePosition.GetFileSize();
    }
    virtual bool CanAsync() const {
        return false;
    }
    virtual bool CanSeek() const {
        return true;
    }
    virtual bool CanRead() const {
        return true;
    }
    virtual bool CanWrite() const {
        return false;
    }
    virtual bool CanCancel() const {
        return false;
    }
    virtual u32 GetOffsetAlign() const {
        return 4;
    }
    virtual u32 GetSizeAlign() const {
        return 32;
    }
    virtual u32 GetBufferAlign() const {
        return 32;
    }
private:
    FilePosition mFilePosition;
    CNTHandleWithMutex* mHandle;
    CNTFileInfo mFileInfo;
    bool mCloseOnDestroyFlg;
    bool mCloseEnableFlg;
};
}
}

#endif
