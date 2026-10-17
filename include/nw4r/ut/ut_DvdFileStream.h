#ifndef NW4R_UT_DVD_FILE_STREAM_H
#define NW4R_UT_DVD_FILE_STREAM_H
#include <nw4r/types_nw4r.h>

#include <nw4r/ut/ut_FileStream.h>

#include <revolution/dvd.h>

namespace nw4r {
namespace ut {

class DvdFileStream : public FileStream {
public:
    NW4R_UT_RTTI_DECL(DvdFileStream);

public:
    explicit DvdFileStream(s32 entrynum);
    DvdFileStream(const DVDFileInfo* pInfo, bool close);
    virtual ~DvdFileStream();

    bool Open(s32 entrynum);
    bool Open(const DVDFileInfo* pInfo, bool close);

    void SetPriority(s32 priority) {
        mPriority = priority;
    }

    virtual void Close();

    virtual s32 Read(void* pDst, u32 size);
    virtual bool ReadAsync(void* pDst, u32 size, StreamCallback pCallback,
                           void* pCallbackArg);

    virtual s32 Peek(void* pDst, u32 size);
    virtual bool PeekAsync(void* pDst, u32 size, StreamCallback pCallback,
                           void* pCallbackArg);

    virtual void Seek(s32 offset, u32 origin);

    virtual void Cancel();
    virtual bool CancelAsync(StreamCallback pCallback,
                             void* pCallbackArg);

    virtual bool IsBusy() const {
        return mIsBusy;
    }

    virtual u32 Tell() const {
        return mFilePosition.Tell();
    }
    virtual u32 GetSize() const {
        return mFilePosition.GetFileSize();
    }

    virtual bool CanAsync() const {
        return true;
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
        return true;
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
    struct DvdFileStreamInfo {
        DVDFileInfo dvdInfo;
        DvdFileStream* stream;
    };

private:
    static void DvdAsyncCallback_(s32 result, DVDFileInfo* pInfo);
    static void DvdCBAsyncCallback_(s32 result, DVDCommandBlock* pBlock);

    void Initialize_();
    u32 AdjustReadLength_(u32 len);

private:
    FilePosition mFilePosition;
    StreamCallback mCancelCallback;
    void* mCancelArg;
    volatile bool mIsCanceling;
    DvdFileStreamInfo mFileInfo;
    s32 mPriority;
    volatile bool mIsBusy;
    bool mCloseOnDestroyFlg;
    bool mCloseEnableFlg;
};

}
}
#endif
