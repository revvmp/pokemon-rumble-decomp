#include <nw4r/ut/ut_CntFileStream.h>

namespace nw4r {
namespace ut {

CNTHandleWithMutex::CNTHandleWithMutex() : mInitialized(false) {
    OSInitMutex(&mMutex);
}

CNTHandleWithMutex::~CNTHandleWithMutex() {
    if (mInitialized) {
        CNTReleaseHandle(&mHandle);
    }
}

void CNTHandleWithMutex::Init(int contentNum, MEMAllocator* pAllocator) {
    mInitialized = CNTInitHandle(contentNum, &mHandle, pAllocator) == CNT_RESULT_OK;
}

CntFileStream::CntFileStream()
    : mHandle(nullptr), mCloseOnDestroyFlg(false), mCloseEnableFlg(false) {
    mAvailable = false;
    mCallback = nullptr;
    mArg = nullptr;
    mAsyncResult = 0;
}

CntFileStream::~CntFileStream() {
    if (mCloseOnDestroyFlg) {
        Close();
    }
}

bool CntFileStream::Open(CNTHandleWithMutex* pHandle, const char* pPath) {
    if (mCloseOnDestroyFlg) {
        Close();
    }
    CNTHandleWithMutex::ScopedLock lock(*pHandle);
    if (CNTOpen(&pHandle->mHandle, pPath, &mFileInfo) != CNT_RESULT_OK) {
        return false;
    }
    mFilePosition.SetFileSize(CNTGetLength(&mFileInfo));
    mFilePosition.Seek(0, SEEK_ORIGIN_BEG);
    mCloseOnDestroyFlg = true;
    mCloseEnableFlg = true;
    mAvailable = true;
    mHandle = pHandle;
    return true;
}

void CntFileStream::Close() {
    if (mCloseEnableFlg && mAvailable) {
        CNTHandleWithMutex::ScopedLock lock(*mHandle);
        CNTClose(&mFileInfo);
        mAvailable = false;
        mHandle = nullptr;
    }
}

s32 CntFileStream::Read(void* pDst, u32 size) {
    CNTHandleWithMutex::ScopedLock lock(*mHandle);
    s32 result = CNTReadAt(&mFileInfo, pDst, size, mFilePosition.Tell());
    if (result > 0) {
        mFilePosition.Skip(result);
    }
    return result;
}

void CntFileStream::Seek(s32 offset, u32 origin) {
    mFilePosition.Seek(offset, origin);
}

NW4R_UT_RTTI_DEF_DERIVED(CntFileStream, FileStream);
}
}
