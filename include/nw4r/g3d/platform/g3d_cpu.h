#ifndef NW4R_G3D_PLATFORM_CPU_H
#define NW4R_G3D_PLATFORM_CPU_H
#include <nw4r/types_nw4r.h>

#include <revolution/os.h>
#include <revolution/os/OSCache.h>

namespace nw4r {
namespace g3d {

namespace DC {

inline void StoreRange(void *pBase, u32 size) {
    DCStoreRange(pBase, size);
}

inline void StoreRangeNoSync(void *pBase, u32 size) {
    DCStoreRangeNoSync(pBase, size);
}

inline void FlushRangeNoSync(void *pBase, u32 size) {
    DCFlushRangeNoSync(pBase, size);
}

inline void InvalidateRange(void *pBase, u32 size) {
    DCInvalidateRange(pBase, size);
}

}

namespace detail {

void Copy32ByteBlocks(void *pDst, const void *pSrc, u32 size);
void ZeroMemory32ByteBlocks(void *pDst, u32 size);
void ZeroMemory16ByteBlocks(void *pDst, u32 size);

}

}
}

#endif
