#include <nw4r/ut/ut_FileStream.h>
#include <nw4r/ut/ut_algorithm.h>

namespace nw4r {
namespace ut {

NW4R_UT_RTTI_DEF_DERIVED(FileStream, IOStream);

void FileStream::Cancel() {}

bool FileStream::CancelAsync(StreamCallback, void*) {
    return true;
}

u32 FileStream::FilePosition::Skip(s32 offset) {
    if (offset != 0) {
        s64 position = mPosition + offset;
        mPosition = Clamp<s64>(position, 0, mFileSize);
    }

    return mPosition;
}

u32 FileStream::FilePosition::Append(s32 offset) {
    s64 position = mPosition + offset;

    if (position < 0) {
        mPosition = 0;
    } else {
        mPosition = position;
        mFileSize = Max(mPosition, mFileSize);
    }

    return mPosition;
}

void FileStream::FilePosition::Seek(s32 offset, u32 origin) {
    switch (origin) {
    case SEEK_ORIGIN_BEG:
        mPosition = 0;
        break;
    case SEEK_ORIGIN_END:
        mPosition = mFileSize;
        break;
    case SEEK_ORIGIN_CUR:
    default:
        break;
    }

    Skip(offset);
}

}
}
