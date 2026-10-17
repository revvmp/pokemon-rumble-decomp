#ifndef NW4R_G3D_ANM_CHR_H
#define NW4R_G3D_ANM_CHR_H
#include <nw4r/types_nw4r.h>

#include "nw4r/g3d/g3d_anmobj.h"
#include "nw4r/g3d/res/g3d_resanmchr.h"

namespace nw4r {
namespace g3d {

class AnmObjChrRes;

class AnmObjChr : public AnmObj {
public:
    enum BindOption {
        BIND_ONE,
        BIND_PARTIAL,
        NUM_OF_BIND_OPTION
    };

public:
    AnmObjChr(MEMAllocator *pAllocator, u16 *pBindingBuf, int numBinding);
    virtual void G3dProc(u32 task, u32 param, void *pInfo) = 0;
    virtual ~AnmObjChr() {}

    virtual void SetFrame(f32 frame) = 0;
    virtual f32 GetFrame() const = 0;
    virtual void UpdateFrame() = 0;

    virtual void SetUpdateRate(f32 rate) = 0;
    virtual f32 GetUpdateRate() const = 0;

    virtual bool Bind(const ResMdl mdl) = 0;
    virtual void Release();

    virtual const ChrAnmResult *GetResult(ChrAnmResult *pResult,
                                          u32 idx) = 0;

    virtual AnmObjChrRes *Attach(int idx, AnmObjChrRes *pRes);
    virtual AnmObjChrRes *Detach(int idx);
    virtual void DetachAll();

    virtual void SetWeight(int idx, f32 weight);
    virtual f32 GetWeight(int idx) const;

    virtual bool Bind(const ResMdl mdl, u32 target,
                      BindOption option) = 0;
    virtual void Release(const ResMdl mdl, u32 target,
                         BindOption option) = 0;

    bool TestExistence(u32 idx) const;
    bool TestDefined(u32 idx) const;

    void UseQuaternionBlend(bool enable) {
        SetAnmFlag(FLAG_USE_QUATERNION_ROTATION_BLEND, enable);
    }

    void UseAccurateScaleBlend(bool enable) {
        SetAnmFlag(FLAG_USE_ACCURATE_SCALE_BLEND, enable);
    }

protected:
    enum BindingFlag {
        BINDING_ID_MASK = (1 << 14) - 1,
        BINDING_INVALID = (1 << 14),
        BINDING_UNDEFINED = (1 << 15),
    };

protected:
    static const int DEFAULT_MAX_CHILDREN = 4;

protected:
    int mNumBinding;
    u16 *const mpBinding;

    NW4R_G3D_RTTI_DECL_DERIVED(AnmObjChr, AnmObj);
};

class AnmObjChrNode : public AnmObjChr {
public:
    AnmObjChrNode(
        MEMAllocator *pAllocator, u16 *pBindingBuf, int numBinding, AnmObjChrRes **ppChildrenBuf, int numChildren
    );
    virtual void G3dProc(u32 task, u32 param, void *pInfo);
    virtual ~AnmObjChrNode();

    virtual void SetFrame(f32 frame);
    virtual f32 GetFrame() const;
    virtual void UpdateFrame();

    virtual void SetUpdateRate(f32 rate);
    virtual f32 GetUpdateRate() const;

    virtual bool Bind(const ResMdl mdl);
    virtual void Release();

    virtual AnmObjChrRes *Attach(int idx, AnmObjChrRes *pRes);
    virtual AnmObjChrRes *Detach(int idx);
    virtual void DetachAll();

    virtual bool Bind(const ResMdl mdl, u32 target,
                      BindOption option);
    virtual void Release(const ResMdl mdl, u32 target,
                         BindOption option);

    inline int Size() {
        return mChildrenArraySize;
    }

    AnmObjChrRes *GetNode(int i) {
        return mpChildrenArray[i];
    }

protected:
    int mChildrenArraySize;
    AnmObjChrRes **mpChildrenArray;

    NW4R_G3D_RTTI_DECL_DERIVED(AnmObjChrNode, AnmObjChr);
};

class AnmObjChrBlend : public AnmObjChrNode {
public:
    static AnmObjChrBlend *Construct(MEMAllocator *pAllocator, u32 *pSize, ResMdl mdl, int numChildren);

    AnmObjChrBlend(
        MEMAllocator *pAllocator, u16 *pBindingBuf, int numBinding, AnmObjChrRes **ppChildrenBuf, int numChildren,
        f32 *pWeightBuf
    );

    virtual ~AnmObjChrBlend() {}

    virtual const ChrAnmResult *GetResult(ChrAnmResult *pResult,
                                          u32 idx);

    virtual void SetWeight(int idx, f32 weight);
    virtual f32 GetWeight(int idx) const;

private:
    f32 *mpWeightArray;

    NW4R_G3D_RTTI_DECL_DERIVED(AnmObjChrBlend, AnmObjChrNode);
};

class AnmObjChrRes : public AnmObjChr, protected FrameCtrl {
public:
    static AnmObjChrRes *Construct(MEMAllocator *pAllocator, u32 *pSize, ResAnmChr chr, ResMdl mdl, bool cache);

    AnmObjChrRes(MEMAllocator *pAllocator, ResAnmChr chr, u16 *pBindingBuf, int numBinding, ChrAnmResult *pCacheBuf);
    virtual void G3dProc(u32 task, u32 param, void *pInfo);
    virtual ~AnmObjChrRes() {}

    virtual void SetFrame(f32 frame);
    virtual f32 GetFrame() const;
    virtual void UpdateFrame();

    virtual void SetUpdateRate(f32 rate);
    virtual f32 GetUpdateRate() const;

    virtual bool Bind(const ResMdl mdl);
    using AnmObjChr::Release;

    virtual const ChrAnmResult *GetResult(ChrAnmResult *pResult,
                                          u32 idx);

    virtual bool Bind(const ResMdl mdl, u32 target,
                      BindOption option);
    virtual void Release(const ResMdl mdl, u32 target,
                         BindOption option);

    void UpdateCache();

    ResAnmChr GetResAnm() {
        return mRes;
    }

    void SetPlayPolicy(PlayPolicyFunc pFunc) {
        FrameCtrl::SetPlayPolicy(pFunc);
    }

private:
    ResAnmChr mRes;
    ChrAnmResult *const mpResultCache;

    NW4R_G3D_RTTI_DECL_DERIVED(AnmObjChrRes, AnmObjChr);
};

}
}

#endif
