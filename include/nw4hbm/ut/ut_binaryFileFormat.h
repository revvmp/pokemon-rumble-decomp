#ifndef NW4HBM_UT_BINARY_FILE_FORMAT_H
#define NW4HBM_UT_BINARY_FILE_FORMAT_H
#include "nw4hbm/types_nw4hbm.h"

namespace nw4hbm {
namespace ut {

struct BinaryBlockHeader {
    u32 kind;
    u32 size;
};

struct BinaryFileHeader {
    u32 signature;
    u16 byteOrder;
    u16 version;
    u32 fileSize;
    u16 headerSize;
    u16 dataBlocks;
};

bool IsValidBinaryFile(const BinaryFileHeader* pHeader, u32 signature,
                       u16 version, u16 minBlocks);
bool IsReverseEndianBinaryFile(const BinaryFileHeader* pHeader);
BinaryBlockHeader* GetNextBinaryBlockHeader(BinaryFileHeader* pHeader,
                                            BinaryBlockHeader* pBlockHeader);

}
}

#endif
