#include <nw4r/ut/ut_algorithm.h>
#include <nw4r/g3d/g3d_anmchrext.h>
#undef RoundUp
namespace nw4r { namespace g3d {
NW4R_G3D_RTTI_DEF(AnmObjChrOverride);

AnmObjChrOverride* AnmObjChrOverride::Construct(MEMAllocator* pAllocator, u32* pSize,
                                              ResMdl mdl, int numChildren) {
    if (!mdl.IsValid()) {
        return nullptr;
    }
    int numBinding = mdl.GetResNodeNumEntries();
    u32 bindingOffset = ut::RoundUp(sizeof(AnmObjChrOverride), 4);
    u32 childrenOffset = ut::RoundUp(bindingOffset + numBinding * sizeof(u16), 4);
    u32 size = ut::RoundUp(childrenOffset + numChildren * sizeof(AnmObjChr*), 4);
    if (pSize != nullptr) {
        *pSize = size;
    }
    if (pAllocator == nullptr) {
        return nullptr;
    }
    u8* pBuffer = reinterpret_cast<u8*>(Alloc(pAllocator, size));
    if (pBuffer == nullptr) {
        return nullptr;
    }
    return new(pBuffer) AnmObjChrOverride(pAllocator, mdl,
                                        reinterpret_cast<u16*>(pBuffer + bindingOffset), numBinding,
                                        reinterpret_cast<AnmObjChr**>(pBuffer + childrenOffset), numChildren);
}

const ChrAnmResult* AnmObjChrOverride::GetResult(ChrAnmResult* pResult, u32 idx) {
    for (int i = mChildrenArraySize - 1; i >= 0; i--) {
        AnmObjChr* pChild = mpChildrenArray[i];
        if (pChild != nullptr) {
            const ChrAnmResult* pChildResult = pChild->GetResult(pResult, idx);
            if (pChildResult->flags & ChrAnmResult::FLAG_ANM_EXISTS) {
                return pChildResult;
            }
        }
    }
    pResult->flags = 0;
    return pResult;
}
} }
