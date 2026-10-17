#include <nw4r/g3d/res/g3d_resvtx.h>
#include <nw4r/g3d/platform/g3d_cpu.h>

namespace nw4r {
namespace g3d {

void ResVtxPos::SetArray() {
    if (IsValid()) {
        GXSetArray(GX_VA_POS, GetData(), ref().stride);
    }
}

void ResVtxPos::GetArray(const void **ppBase, u8 *pStride) const {
    if (ppBase != NULL) {
        *ppBase = GetData();
    }

    if (pStride) {
        *pStride = ref().stride;
    }
}

void ResVtxPos::CopyTo(void *pDst) const {
    if (!IsValid()) {
        return;
    }

    u32 size = GetSize();
    detail::Copy32ByteBlocks(pDst, ptr(), size);
    DCStoreRange(pDst, size);
}

void ResVtxNrm::SetArray() {
    if (IsValid()) {
        GXSetArray(GX_VA_NRM, GetData(), ref().stride);
    }
}

void ResVtxNrm::GetArray(const void **ppBase, u8 *pStride) const {
    if (ppBase != NULL) {
        *ppBase = GetData();
    }

    if (pStride) {
        *pStride = ref().stride;
    }
}

void ResVtxNrm::CopyTo(void *pDst) const {
    if (!IsValid()) {
        return;
    }

    u32 size = GetSize();
    detail::Copy32ByteBlocks(pDst, ptr(), size);
    DCStoreRange(pDst, size);
}

void ResVtxClr::SetArray(GXAttr attr) {
    if (IsValid() && (attr == GX_VA_CLR0 || attr == GX_VA_CLR1)) {
        GXSetArray(attr, GetData(), ref().stride);
    }
}

void ResVtxClr::GetArray(const void **ppBase, u8 *pStride) const {
    if (ppBase != NULL) {
        *ppBase = GetData();
    }

    if (pStride) {
        *pStride = ref().stride;
    }
}

void ResVtxClr::CopyTo(void *pDst) const {
    if (!IsValid()) {
        return;
    }

    u32 size = GetSize();
    detail::Copy32ByteBlocks(pDst, ptr(), size);
    DCStoreRange(pDst, size);
}

void ResVtxTexCoord::GetArray(const void **ppBase, u8 *pStride) const {
    if (ppBase != NULL) {
        *ppBase = GetData();
    }

    if (pStride) {
        *pStride = ref().stride;
    }
}

void ResVtxFurPos::SetArray(u16 idx) {
    if (IsValid()) {
        void *pBase = GetData(idx);
        GXSetArray(GX_VA_POS, pBase, ref().stride);
    }
}

void ResVtxPos::DCStore(bool sync) {
    void *pBase = &ref();
    u32 size = GetSize();

    if (sync) {
        DC::StoreRange(pBase, size);
    } else {
        DC::StoreRangeNoSync(pBase, size);
    }
}

void ResVtxNrm::DCStore(bool sync) {
    void *pBase = &ref();
    u32 size = GetSize();

    if (sync) {
        DC::StoreRange(pBase, size);
    } else {
        DC::StoreRangeNoSync(pBase, size);
    }
}

void ResVtxClr::DCStore(bool sync) {
    void *pBase = &ref();
    u32 size = GetSize();

    if (sync) {
        DC::StoreRange(pBase, size);
    } else {
        DC::StoreRangeNoSync(pBase, size);
    }
}

void ResVtxTexCoord::DCStore(bool sync) {
    void *pBase = &ref();
    u32 size = GetSize();

    if (sync) {
        DC::StoreRange(pBase, size);
    } else {
        DC::StoreRangeNoSync(pBase, size);
    }
}

void ResVtxFurVec::DCStore(bool sync) {
    void *pBase = &ref();
    u32 size = GetSize();

    if (sync) {
        DC::StoreRange(pBase, size);
    } else {
        DC::StoreRangeNoSync(pBase, size);
    }
}

void ResVtxFurPos::DCStore(bool sync) {
    void *pBase = &ref();
    u32 size = GetSize();

    if (sync) {
        DC::StoreRange(pBase, size);
    } else {
        DC::StoreRangeNoSync(pBase, size);
    }
}

}
}
