#ifndef NW4R_UT_NAND_FILE_STREAM_H
#define NW4R_UT_NAND_FILE_STREAM_H
#include <nw4r/types_nw4r.h>

#include <nw4r/ut/ut_FileStream.h>

#include <revolution/nand.h>

namespace nw4r {
namespace ut {

class NandFileStream : public FileStream {
public:
    NW4R_UT_RTTI_DECL(NandFileStream);

public:
    NandFileStream();
    virtual ~NandFileStream();

    bool Open(const char* pPath, u32 mode);

    virtual void Close();

    virtual s32 Read(void* pDst, u32 size);
    virtual bool ReadAsync(void* pDst, u32 size, StreamCallback pCallback, void* pCallbackArg);

    virtual s32 Write(const void* pSrc, u32 size);
    virtual bool WriteAsync(const void* pSrc, u32 size, StreamCallback pCallback, void* pCallbackArg);

    virtual void Seek(s32 offset, u32 origin);

    virtual bool IsBusy() const {
        return mIsBusy;
    }

    virtual u32 Tell() const;
    virtual u32 GetSize() const;
    virtual bool CanAsync() const {
        return true;
    }
    virtual bool CanSeek() const {
        return true;
    }
    virtual bool CanRead() const {
        return mCanRead;
    }
    virtual bool CanWrite() const {
        return mCanWrite;
    }
    virtual bool CanCancel() const {
        return false;
    }

    virtual u32 GetOffsetAlign() const {
        return 1;
    }
    virtual u32 GetSizeAlign() const {
        return 32;
    }
    virtual u32 GetBufferAlign() const {
        return 32;
    }

private:
    struct NandFileStreamInfo {
        NANDCommandBlock nandBlock;
        NANDFileInfo nandInfo;
        NandFileStream* stream;
    };

private:
    static void NandAsyncCallback_(s32 result, NANDCommandBlock* pBlock);


private:
    FilePosition mFilePosition;
    NandFileStreamInfo mFileInfo;
    bool mCanRead;
    bool mCanWrite;
    volatile bool mIsBusy;
    bool mCloseOnDestroyFlg;
    bool mCloseEnableFlg;
};

}
}

#endif
