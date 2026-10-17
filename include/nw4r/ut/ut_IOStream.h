#ifndef NW4R_UT_IO_STREAM_H
#define NW4R_UT_IO_STREAM_H
#include <nw4r/types_nw4r.h>

#include <nw4r/ut/ut_RuntimeTypeInfo.h>

namespace nw4r {
namespace ut {

class IOStream {
public:
    NW4R_UT_RTTI_DECL(IOStream);

    typedef void (*StreamCallback)(s32 result, IOStream* pStream,
                                   void* pCallbackArg);

public:
    IOStream() : mAvailable(false), mCallback(NULL), mArg(NULL) {}
    virtual ~IOStream() {}

    virtual void Close() = 0;

    virtual s32 Read(void* pDst, u32 size) = 0;
    virtual bool ReadAsync(void* pDst, u32 size, StreamCallback pCallback,
                           void* pCallbackArg);

    virtual s32 Write(const void* pSrc, u32 size);
    virtual bool WriteAsync(const void* pSrc, u32 size,
                            StreamCallback pCallback,
                            void* pCallbackArg);

    virtual bool IsBusy() const;

    virtual bool CanAsync() const = 0;
    virtual bool CanRead() const = 0;
    virtual bool CanWrite() const = 0;

    virtual u32 GetOffsetAlign() const {
        return 1;
    }
    virtual u32 GetSizeAlign() const {
        return 1;
    }
    virtual u32 GetBufferAlign() const {
        return 1;
    }

    bool IsAvailable() const {
        return mAvailable;
    }

protected:
    bool mAvailable;
    s32 mAsyncResult;
    StreamCallback mCallback;
    void* mArg;
};

}
}

#endif
