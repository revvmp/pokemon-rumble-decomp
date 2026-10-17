#ifndef NW4R_SND_MEMORY_SOUND_ARCHIVE_H
#define NW4R_SND_MEMORY_SOUND_ARCHIVE_H
#include <nw4r/types_nw4r.h>

#include <nw4r/ut/ut_FileStream.h>

#include <nw4r/snd/snd_SoundArchive.h>
#include <nw4r/snd/snd_SoundArchiveFile.h>

namespace nw4r {
namespace snd {

class MemorySoundArchive : public SoundArchive {
private:
    class MemoryFileStream;

public:
    MemorySoundArchive();
    virtual ~MemorySoundArchive();

    virtual const void* detail_GetFileAddress(u32 fileId) const;
    virtual const void* detail_GetWaveDataFileAddress(u32 fileId) const;
    virtual int detail_GetRequiredStreamBufferSize() const;
    virtual ut::FileStream* OpenStream(void* pBuffer, int size, u32 begin,
                                       u32 length) const;
    virtual ut::FileStream* OpenExtStream(void* pBuffer, int size,
                                          const char* pExtFilePath, u32 begin,
                                          u32 length) const;

    bool Setup(const void* pSoundArchiveData);
    void Shutdown();

private:
    const void* mData;
    detail::SoundArchiveFileReader mFileReader;
};

class MemorySoundArchive::MemoryFileStream : public ut::FileStream {
public:
    MemoryFileStream(const void* pBuffer, u32 size)
        : mBuffer(pBuffer), mSize(size), mPosition(0) {}
    virtual ~MemoryFileStream() {}

    virtual void Close();
    virtual s32 Read(void* pDst, u32 size);
    virtual void Seek(s32 offset, u32 origin);

    virtual bool CanSeek() const {
        return true;
    }
    virtual bool CanCancel() const {
        return true;
    }
    virtual bool CanAsync() const {
        return false;
    }
    virtual bool CanRead() const {
        return true;
    }
    virtual bool CanWrite() const {
        return false;
    }
    virtual u32 Tell() const {
        return mPosition;
    }
    virtual u32 GetSize() const {
        return mSize;
    }

private:
    const void* mBuffer;
    u32 mSize;
    u32 mPosition;
};

}
}

#endif
