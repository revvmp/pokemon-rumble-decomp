#include <nw4r/ut/ut_algorithm.h>

#include <nw4r/snd/snd_MemorySoundArchive.h>

#include <string.h>

inline void* operator new(size_t, void* p) {
    return p;
}

namespace nw4r {
namespace snd {

MemorySoundArchive::MemorySoundArchive() : mData(nullptr) {}

MemorySoundArchive::~MemorySoundArchive() {}

bool MemorySoundArchive::Setup(const void* pSoundArchiveData) {
    mFileReader.Init(pSoundArchiveData);
    SoundArchive::Setup(&mFileReader);

    const void* pInfoChunk = ut::AddOffsetToPtr(
        pSoundArchiveData, mFileReader.GetInfoChunkOffset());
    mFileReader.SetInfoChunk(pInfoChunk, mFileReader.GetInfoChunkSize());

    const void* pStringChunk = ut::AddOffsetToPtr(
        pSoundArchiveData, mFileReader.GetLabelStringChunkOffset());
    mFileReader.SetStringChunk(pStringChunk,
                               mFileReader.GetLabelStringChunkSize());

    mData = pSoundArchiveData;
    return true;
}

void MemorySoundArchive::Shutdown() {
    mData = nullptr;
    SoundArchive::Shutdown();
}

const void* MemorySoundArchive::detail_GetFileAddress(u32 fileId) const {
    SoundArchive::FilePos filePos;
    if (!detail_ReadFilePos(fileId, 0, &filePos)) {
        return nullptr;
    }

    SoundArchive::GroupInfo groupInfo;
    if (!detail_ReadGroupInfo(filePos.groupId, &groupInfo)) {
        return nullptr;
    }

    SoundArchive::GroupItemInfo itemInfo;
    if (!detail_ReadGroupItemInfo(filePos.groupId, filePos.index, &itemInfo)) {
        return nullptr;
    }

    if (groupInfo.extFilePath != nullptr) {
        return nullptr;
    }

    return ut::AddOffsetToPtr(mData, groupInfo.offset + itemInfo.offset);
}

const void*
MemorySoundArchive::detail_GetWaveDataFileAddress(u32 fileId) const {
    SoundArchive::FilePos filePos;
    if (!detail_ReadFilePos(fileId, 0, &filePos)) {
        return nullptr;
    }

    SoundArchive::GroupInfo groupInfo;
    if (!detail_ReadGroupInfo(filePos.groupId, &groupInfo)) {
        return nullptr;
    }

    SoundArchive::GroupItemInfo itemInfo;
    if (!detail_ReadGroupItemInfo(filePos.groupId, filePos.index, &itemInfo)) {
        return nullptr;
    }

    if (groupInfo.extFilePath != nullptr) {
        return nullptr;
    }

    return ut::AddOffsetToPtr(mData, groupInfo.waveDataOffset +
                                         itemInfo.waveDataOffset);
}

ut::FileStream* MemorySoundArchive::OpenStream(void* pBuffer, int size,
                                               u32 begin, u32 length) const {
    if (mData == nullptr) {
        return nullptr;
    }

    if (size < sizeof(MemoryFileStream)) {
        return nullptr;
    }

    return new (pBuffer)
        MemoryFileStream(ut::AddOffsetToPtr(mData, begin), length);
}

ut::FileStream* MemorySoundArchive::OpenExtStream(void* pBuffer, int size,
                                                  const char* pExtFilePath,
                                                  u32 begin,
                                                  u32 length) const {
    return nullptr;
}

int MemorySoundArchive::detail_GetRequiredStreamBufferSize() const {
    return sizeof(MemoryFileStream);
}

void MemorySoundArchive::MemoryFileStream::Close() {
    mBuffer = nullptr;
    mSize = 0;
    mPosition = 0;
}

s32 MemorySoundArchive::MemoryFileStream::Read(void* pDst, u32 size) {
    u32 readSize = ut::Min(size, mSize - mPosition);
    memcpy(pDst, ut::AddOffsetToPtr(mBuffer, mPosition), readSize);
    return readSize;
}

void MemorySoundArchive::MemoryFileStream::Seek(s32 offset, u32 origin) {
    switch (origin) {
    case SEEK_ORIGIN_BEG:
        mPosition = offset;
        break;
    case SEEK_ORIGIN_CUR:
        mPosition += offset;
        break;
    case SEEK_ORIGIN_END:
        mPosition = mSize - offset;
        break;
    default:
        return;
    }
}

}
}
