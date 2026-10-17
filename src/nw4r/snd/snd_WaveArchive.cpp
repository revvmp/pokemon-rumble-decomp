#include <nw4r/snd/snd_WaveArchive.h>

#include <nw4r/ut/ut_algorithm.h>

namespace nw4r {
namespace snd {
namespace detail {

WaveArchiveReader::WaveArchiveReader(const void* pWaveArc)
    : mTableBlock(nullptr), mDataBlock(nullptr) {
    if (!VerifyFileHeader(pWaveArc)) {
        return;
    }

    const WaveArchive::FileHeader* pFileHeader =
        static_cast<const WaveArchive::FileHeader*>(pWaveArc);
    const WaveArchive::TableBlock* pTableBlock =
        static_cast<const WaveArchive::TableBlock*>(
            ut::AddOffsetToPtr(pFileHeader, pFileHeader->tableChunkOffset));
    const WaveArchive::DataBlock* pDataBlock =
        static_cast<const WaveArchive::DataBlock*>(
            ut::AddOffsetToPtr(pFileHeader, pFileHeader->dataChunkOffset));

    mTableBlock = pTableBlock;
    mDataBlock = pDataBlock;
}

const WaveFile::FileHeader* WaveArchiveReader::GetWaveFile(int index) const {
    if (!mTableBlock) {
        return nullptr;
    }
    if (!mDataBlock) {
        return nullptr;
    }
    if (index < 0) {
        return nullptr;
    }
    if (index >= mTableBlock->fileTable.count) {
        return nullptr;
    }

    Util::DataRef<WaveFile::FileHeader> waveFileRef =
        mTableBlock->fileTable.items[index].waveFileRef;
    const WaveFile::FileHeader* pFileHeader =
        Util::GetDataRefAddress0(waveFileRef, mDataBlock);
    return pFileHeader;
}

bool WaveArchiveReader::VerifyFileHeader(const void* pWaveArc) {
    const ut::BinaryFileHeader* pFileHeader =
        static_cast<const ut::BinaryFileHeader*>(pWaveArc);
    if (pFileHeader->signature != SIGNATURE) {
        return false;
    }
    if (pFileHeader->version < VERSION) {
        return false;
    }
    if (pFileHeader->version > VERSION) {
        return false;
    }
    return true;
}

}
}
}
