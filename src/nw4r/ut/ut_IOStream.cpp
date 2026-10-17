#include <nw4r/ut/ut_IOStream.h>

namespace nw4r {
namespace ut {

NW4R_UT_RTTI_DEF_BASE(IOStream);

bool IOStream::ReadAsync(void*, u32, StreamCallback, void*) {
    return false;
}

s32 IOStream::Write(const void*, u32) {
    return 0;
}

bool IOStream::WriteAsync(const void*, u32, StreamCallback, void*) {
    return false;
}

bool IOStream::IsBusy() const {
    return false;
}

}
}
