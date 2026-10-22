#include <nw4r/ut/ut_algorithm.h>
#include <nw4r/g3d/g3d_anmchrext.h>
#undef RoundUp
namespace nw4r { namespace g3d {
NW4R_G3D_RTTI_DEF(AnmObjChrRootScale);

AnmObjChrRootScale* AnmObjChrRootScale::Construct(MEMAllocator* pAllocator, u32* pSize, ResMdl mdl) {
    if (!mdl.IsValid()) {
        return nullptr;
    }
    int numBinding = mdl.GetResNodeNumEntries();
    u32 bindingOffset = ut::RoundUp(sizeof(AnmObjChrRootScale), 4);
    u32 size = ut::RoundUp(bindingOffset + numBinding * sizeof(u16), 4);
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
    return new(pBuffer) AnmObjChrRootScale(pAllocator, mdl,
                                         reinterpret_cast<u16*>(pBuffer + bindingOffset), numBinding);
}

AnmObjChrRootScale::~AnmObjChrRootScale() {}

const ChrAnmResult* AnmObjChrRootScale::GetResult(ChrAnmResult* pResult, u32 idx) {
    if (mpChild != nullptr) {
        if (idx != 0) {
            return mpChild->GetResult(pResult, idx);
        }
        const ChrAnmResult* pChildResult = mpChild->GetResult(pResult, idx);
        if (pChildResult != pResult) {
            *pResult = *pChildResult;
        }
        return ApplyRootScale(pResult, idx);
    }
    pResult->flags = 0;
    return ApplyRootScale(pResult, idx);
}
} }
