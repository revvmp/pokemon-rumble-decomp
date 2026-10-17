#include <nw4r/snd/snd_SeqFile.h>

#include <nw4r/ut/ut_algorithm.h>

#include <string.h>

namespace nw4r {
namespace snd {
namespace detail {

bool SeqFileReader::IsValidFileHeader(const void* pSeqBin) {
    const ut::BinaryFileHeader* pHeader =
        static_cast<const ut::BinaryFileHeader*>(pSeqBin);

    if (pHeader->signature != SeqFile::SIGNATURE_FILE) {
        return false;
    }

    if (pHeader->version < NW4R_VERSION(1, 0)) {
        return false;
    }

    if (pHeader->version > SUPPORTED_FILE_VERSION) {
        return false;
    }

    return true;
}

SeqFileReader::SeqFileReader(const void* pSeqBin)
    : mHeader(nullptr), mDataBlock(nullptr) {
    if (!IsValidFileHeader(pSeqBin)) {
        return;
    }

    mHeader = static_cast<const SeqFile::Header*>(pSeqBin);
    mDataBlock = static_cast<const SeqFile::DataBlock*>(
        ut::AddOffsetToPtr(mHeader, mHeader->dataBlockOffset));
}

const void* SeqFileReader::GetBaseAddress() const {
    return ut::AddOffsetToPtr(mDataBlock, mDataBlock->baseOffset);
}

bool SeqFileReader::ReadOffsetByLabel(const char* pLabel, u32* pOffset) const {
    const SeqFile::LabelBlock* pLabelBlock =
        reinterpret_cast<const SeqFile::LabelBlock*>(
            ut::AddOffsetToPtr(
                mHeader, Util::ReadBigEndian(mHeader->labelBlockOffset)));

    if (pLabelBlock == nullptr) {
        return false;
    }

    u32 labelLen = strlen(pLabel);

    for (int i = 0; i < Util::ReadBigEndian(pLabelBlock->labelInfoTable.count);
         i++) {
        u32 offset = pLabelBlock->labelInfoTable.items[i];
        const SeqFile::LabelInfo* pLabelInfo =
            reinterpret_cast<const SeqFile::LabelInfo*>(ut::AddOffsetToPtr(
                pLabelBlock, Util::ReadBigEndian(offset) + 8));

        if (labelLen == Util::ReadBigEndian(pLabelInfo->nameLen) &&
            strncmp(pLabel, pLabelInfo->name, labelLen) == 0) {
            *pOffset = Util::ReadBigEndian(pLabelInfo->offset);
            return true;
        }
    }

    return false;
}

}
}
}
