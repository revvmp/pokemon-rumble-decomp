#ifndef NW4R_LYT_TYPES_H
#define NW4R_LYT_TYPES_H
#include <nw4r/types_nw4r.h>

namespace nw4r {
namespace lyt {

struct Size {
    Size() : width(0.0f), height(0.0f) {}
    Size(f32 w, f32 h) : width(w), height(h) {}

    f32 width;
    f32 height;
};

namespace res {

struct BinaryFileHeader {
    char signature[4];
    u16 byteOrder;
    u16 version;
    u32 fileSize;
    u16 headerSize;
    u16 dataBlocks;
};

}
}
}

#endif
