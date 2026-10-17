#ifndef NW4R_G3D_ANM_TEX_SRT_H
#define NW4R_G3D_ANM_TEX_SRT_H
#include <nw4r/types_nw4r.h>

#include "nw4r/g3d/g3d_anmobj.h"
#include "nw4r/g3d/res/g3d_resanmtexsrt.h"
#include "nw4r/g3d/res/g3d_resmat.h"

namespace nw4r {
namespace g3d {

void ApplyTexSrtAnmResult(ResTexSrt srt, ResMatIndMtxAndScale ind, const TexSrtAnmResult *pResult);
void ApplyTexSrtAnmResult(ResTexSrt srt, const TexSrtAnmResult *pResult) __attribute__((noinline));

class AnmObjTexSrtRes;

class AnmObjTexSrt : public AnmObj {
public:
    AnmObjTexSrt(MEMAllocator *pAllocator, u16 *pBindingBuf, int numBinding);
    virtual void G3dProc(u32 task, u32 param, void *pInfo) = 0;
    virtual ~AnmObjTexSrt() {}

    virtual void SetFrame(f32 frame) = 0;
    virtual f32 GetFrame() const = 0;
    virtual void UpdateFrame() = 0;

    virtual void SetUpdateRate(f32 rate) = 0;
    virtual f32 GetUpdateRate() const = 0;

    virtual bool Bind(const ResMdl mdl) = 0;
    virtual void Release();

    virtual const TexSrtAnmResult *GetResult(TexSrtAnmResult *pResult,
                                             u32 idx) = 0;

    virtual AnmObjTexSrtRes *Attach(int idx, AnmObjTexSrtRes *pRes);
    virtual AnmObjTexSrtRes *Detach(int idx);
    virtual void DetachAll();

    bool TestExistence(u32 idx) const;
    bool TestDefined(u32 idx) const;

protected:
    enum BindingFlag {
        BINDING_ID_MASK = (1 << 14) - 1,
        BINDING_INVALID = (1 << 14),
        BINDING_UNDEFINED = (1 << 15),
    };

protected:
    static const int MAX_CHILD = 4;

protected:
    int mNumBinding;
    u16 *const mpBinding;

    NW4R_G3D_RTTI_DECL_DERIVED(AnmObjTexSrt, AnmObj);
};

class AnmObjTexSrtNode : public AnmObjTexSrt {
public:
    AnmObjTexSrtNode(
        MEMAllocator *pAllocator, u16 *pBindingBuf, int numBinding, AnmObjTexSrtRes **ppChildrenBuf, int numChildren
    );
    virtual void G3dProc(u32 task, u32 param, void *pInfo);
    virtual ~AnmObjTexSrtNode();

    virtual void SetFrame(f32 frame);
    virtual f32 GetFrame() const;
    virtual void UpdateFrame();

    virtual void SetUpdateRate(f32 rate);
    virtual f32 GetUpdateRate() const;

    virtual bool Bind(const ResMdl mdl);
    virtual void Release();

    virtual AnmObjTexSrtRes *Attach(int idx, AnmObjTexSrtRes *pRes);
    virtual AnmObjTexSrtRes *Detach(int idx);
    virtual void DetachAll();

    int Size() const {
        return mChildrenArraySize;
    }

protected:
    int mChildrenArraySize;
    AnmObjTexSrtRes **mpChildrenArray;

    NW4R_G3D_RTTI_DECL_DERIVED(AnmObjTexSrtNode, AnmObjTexSrt);
};

class AnmObjTexSrtOverride : public AnmObjTexSrtNode {
public:
    static AnmObjTexSrtOverride *Construct(MEMAllocator *pAllocator, u32 *pSize, ResMdl mdl, int numChildren);

    AnmObjTexSrtOverride(
        MEMAllocator *pAllocator, u16 *pBindingBuf, int numBinding, AnmObjTexSrtRes **ppChildrenBuf, int numChildren
    )
        : AnmObjTexSrtNode(pAllocator, pBindingBuf, numBinding, ppChildrenBuf, numChildren) {}

    virtual ~AnmObjTexSrtOverride() {}

    virtual const TexSrtAnmResult *GetResult(TexSrtAnmResult *pResult,
                                             u32 idx);

    NW4R_G3D_RTTI_DECL_DERIVED(AnmObjTexSrtOverride, AnmObjTexSrtNode);
};

class AnmObjTexSrtRes : public AnmObjTexSrt, protected FrameCtrl {
public:
    static AnmObjTexSrtRes *Construct(MEMAllocator *pAllocator, u32 *pSize, ResAnmTexSrt srt, ResMdl mdl, bool cache);

    AnmObjTexSrtRes(
        MEMAllocator *pAllocator, ResAnmTexSrt srt, u16 *pBindingBuf, int numBinding, TexSrtAnmResult *pCacheBuf
    );
    virtual void G3dProc(u32 task, u32 param, void *pInfo);
    virtual ~AnmObjTexSrtRes() {}

    virtual void SetFrame(f32 frame);
    virtual f32 GetFrame() const;
    virtual void UpdateFrame();

    virtual void SetUpdateRate(f32 rate);
    virtual f32 GetUpdateRate() const;

    virtual bool Bind(const ResMdl mdl);

    virtual const TexSrtAnmResult *GetResult(TexSrtAnmResult *pResult,
                                             u32 idx);

    void UpdateCache();

    ResAnmTexSrt GetResAnm() {
        return mRes;
    }

    void SetPlayPolicy(PlayPolicyFunc pFunc) {
        FrameCtrl::SetPlayPolicy(pFunc);
    }

private:
    ResAnmTexSrt mRes;
    TexSrtAnmResult *const mpResultCache;

    NW4R_G3D_RTTI_DECL_DERIVED(AnmObjTexSrtRes, AnmObjTexSrt);
};

}
}

#endif
