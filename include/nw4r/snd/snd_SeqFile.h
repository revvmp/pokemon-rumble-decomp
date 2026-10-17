#ifndef NW4R_SND_SEQ_FILE_H
#define NW4R_SND_SEQ_FILE_H
#include <nw4r/types_nw4r.h>

#include <nw4r/snd/snd_Util.h>

#include <nw4r/ut/ut_binaryFileFormat.h>

namespace nw4r {
namespace snd {
namespace detail {

namespace SeqFile {

struct Header {
    ut::BinaryFileHeader fileHeader;
    u32 dataBlockOffset;
    u32 dataBlockSize;
    u32 labelBlockOffset;
    u32 labelBlockSize;
};

struct LabelInfo {
    u32 offset;
    u32 nameLen;
    char name[1];
};

struct LabelBlock {
    ut::BinaryBlockHeader blockHeader;
    Util::Table<u32> labelInfoTable;
};

struct DataBlock {
    ut::BinaryBlockHeader blockHeader;
    u32 baseOffset;
};

static const u32 SIGNATURE_FILE = FOURCC('R', 'S', 'E', 'Q');
static const u32 SIGNATURE_DATA_BLOCK = FOURCC('D', 'A', 'T', 'A');
static const u32 SIGNATURE_LABEL_BLOCK = FOURCC('L', 'A', 'B', 'L');
static const int FILE_VERSION = NW4R_VERSION(1, 2);

}

class SeqFileReader {
public:
    static const u16 SUPPORTED_FILE_VERSION = NW4R_VERSION(1, 0);

public:
    explicit SeqFileReader(const void* pSeqBin);

    const void* GetBaseAddress() const;
    bool ReadOffsetByLabel(const char* pLabel, u32* pOffset) const;

private:
    static bool IsValidFileHeader(const void* pSeqBin);

private:
    const SeqFile::Header* mHeader;
    const SeqFile::DataBlock* mDataBlock;
};

}
}
}

#endif
