#ifndef NW4R_G3D_ANMCHREXT_H
#define NW4R_G3D_ANMCHREXT_H
#include <nw4r/g3d/g3d_anmchr.h>
namespace nw4r { namespace g3d {
class AnmObjChrGroup : public AnmObjChr {
public:
    AnmObjChrGroup(MEMAllocator* pAllocator, ResMdl mdl, u16* pBindingBuf, int numBinding, AnmObjChr** ppChildrenBuf, int numChildren);
    virtual ~AnmObjChrGroup();
    virtual void G3dProc(u32 task, u32 param, void* pInfo);
    virtual void UpdateFrame();
    virtual void SetFrame(f32 frame);
    virtual f32 GetFrame() const;
    virtual void SetUpdateRate(f32 rate);
    virtual f32 GetUpdateRate() const;
    virtual bool Bind(const ResMdl mdl);
    virtual bool Bind(const ResMdl mdl, u32 target, BindOption option);
    virtual void Release();
    virtual void Release(const ResMdl mdl, u32 target, BindOption option);
    virtual AnmObjChr* AttachChild(int idx, AnmObjChr* pChild);
    virtual AnmObjChr* DetachChild(int idx);
    virtual void DetachAllChildren();
    void SetRootScale(const math::VEC3& rScale);
protected:
    const ChrAnmResult* ApplyRootScale(ChrAnmResult* pResult, u32 idx) const;
    ResMdl mResMdl;
    int mChildrenArraySize;
    AnmObjChr** mpChildrenArray;
    u32 mRootScaleFlags;
    math::VEC3 mRootScale;
    NW4R_G3D_RTTI_DECL_DERIVED(AnmObjChrGroup, AnmObjChr);
};
class AnmObjChrOverride : public AnmObjChrGroup {
public:
    static AnmObjChrOverride* Construct(MEMAllocator* pAllocator, u32* pSize, ResMdl mdl, int numChildren);
    AnmObjChrOverride(MEMAllocator* pAllocator, ResMdl mdl, u16* pBindingBuf, int numBinding, AnmObjChr** ppChildrenBuf, int numChildren) : AnmObjChrGroup(pAllocator, mdl, pBindingBuf, numBinding, ppChildrenBuf, numChildren) {}
    virtual ~AnmObjChrOverride() {}
    virtual const ChrAnmResult* GetResult(ChrAnmResult* pResult, u32 idx);
    NW4R_G3D_RTTI_DECL_DERIVED(AnmObjChrOverride, AnmObjChrGroup);
};
class AnmObjChrRootScale : public AnmObjChrGroup {
public:
    static AnmObjChrRootScale* Construct(MEMAllocator* pAllocator, u32* pSize, ResMdl mdl);
    AnmObjChrRootScale(MEMAllocator* pAllocator, ResMdl mdl, u16* pBindingBuf, int numBinding) : AnmObjChrGroup(pAllocator, mdl, pBindingBuf, numBinding, &mpChild, 1) {}
    virtual ~AnmObjChrRootScale();
    virtual const ChrAnmResult* GetResult(ChrAnmResult* pResult, u32 idx);
private:
    AnmObjChr* mpChild;
    NW4R_G3D_RTTI_DECL_DERIVED(AnmObjChrRootScale, AnmObjChrGroup);
};
} }
#endif
