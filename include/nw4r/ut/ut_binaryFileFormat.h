#ifndef NW4R_UT_BINARY_FILE_FORMAT_H
#define NW4R_UT_BINARY_FILE_FORMAT_H
#include <nw4r/types_nw4r.h>

namespace nw4r {
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

}
}

#endif
