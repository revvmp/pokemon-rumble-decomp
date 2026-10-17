#ifndef NW4R_SND_NAND_SOUND_ARCHIVE_H
#define NW4R_SND_NAND_SOUND_ARCHIVE_H
#include "nw4r/snd/snd_SoundArchive.h"
#include "nw4r/snd/snd_SoundArchiveFile.h"
#include "nw4r/types_nw4r.h"
#include <nw4r/ut/ut_NandFileStream.h>
#include <revolution/nand.h>

namespace nw4r {
namespace snd {

class NandSoundArchive : public SoundArchive {
private:
    struct NandFileStream;

public:
    NandSoundArchive();
    virtual ~NandSoundArchive();

    virtual const void *detail_GetFileAddress(u32 ) const {
        return nullptr;
    }

    virtual const void *detail_GetWaveDataFileAddress(u32 ) const {
        return nullptr;
    }

    virtual int detail_GetRequiredStreamBufferSize() const;

    virtual ut::FileStream *OpenStream(void *pBuffer, int size, u32 offset,
                                       u32 length) const;

    virtual ut::FileStream *OpenExtStream(
        void *pBuffer, int size, const char *pExtPath, u32 offset,
        u32 length
    ) const;

    bool Open(const char *pPath);
    void Close();

    bool LoadHeader(void *pBuffer, u32 size);
    bool LoadLabelStringData(void *pBuffer, u32 size);

    u32 GetHeaderSize() const {
        return mFileReader.GetInfoChunkSize();
    }
    u32 GetLabelStringDataSize() const {
        return mFileReader.GetLabelStringChunkSize();
    }

private:
    bool LoadFileHeader();

private:
    detail::SoundArchiveFileReader mFileReader;
    NANDFileInfo mFileInfo;
    bool mOpen;
};

}
}

#endif
