#ifndef NW4R_SND_WAVE_ARCHIVE_H
#define NW4R_SND_WAVE_ARCHIVE_H
#include <nw4r/types_nw4r.h>

#include <nw4r/snd/snd_Util.h>
#include <nw4r/snd/snd_WaveFile.h>

#include <nw4r/ut/ut_binaryFileFormat.h>

namespace nw4r {
namespace snd {
namespace detail {

namespace WaveArchive {

struct FileHeader {
    ut::BinaryFileHeader fileHeader;
    u32 tableChunkOffset;
    u32 tableChunkSize;
    u32 dataChunkOffset;
    u32 dataChunkSize;
};

struct TableItem {
    Util::DataRef<WaveFile::FileHeader> waveFileRef;
    u32 waveFileSize;
};

struct TableBlock {
    ut::BinaryBlockHeader blockHeader;
    Util::Table<TableItem> fileTable;
};

struct DataBlock {
    ut::BinaryBlockHeader blockHeader;
    u8 data[1];
};

}

class WaveArchiveReader {
public:
    static const u32 SIGNATURE = FOURCC('R', 'W', 'A', 'R');
    static const int VERSION = NW4R_VERSION(1, 0);

public:
    explicit WaveArchiveReader(const void* pWaveArc);

    const WaveFile::FileHeader* GetWaveFile(int index) const;

private:
    bool VerifyFileHeader(const void* pWaveArc);

private:
    const WaveArchive::TableBlock* mTableBlock;
    const WaveArchive::DataBlock* mDataBlock;
};

}
}
}

#endif
