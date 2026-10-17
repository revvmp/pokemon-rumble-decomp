#ifndef NW4R_G3D_ANM_TEX_PAT_H
#define NW4R_G3D_ANM_TEX_PAT_H
#include <nw4r/types_nw4r.h>

#include <nw4r/g3d/g3d_anmobj.h>
#include <nw4r/g3d/res/g3d_resanmtexpat.h>

namespace nw4r {
namespace g3d {

void ApplyTexPatAnmResult(ResTexObj texObj, ResTlutObj tlutObj, const TexPatAnmResult *pResult);

class AnmObjTexPatRes;

class AnmObjTexPat : public AnmObj {
public:
    AnmObjTexPat(MEMAllocator *pAllocator, u16 *pBindingBuf, int numBinding);
    virtual void G3dProc(u32 task, u32 param, void *pInfo) = 0;
    virtual ~AnmObjTexPat() {}

    virtual void SetFrame(f32 frame) = 0;
    virtual f32 GetFrame() const = 0;
    virtual void UpdateFrame() = 0;

    virtual void SetUpdateRate(f32 rate) = 0;
    virtual f32 GetUpdateRate() const = 0;

    virtual bool Bind(const ResMdl mdl) = 0;
    virtual void Release();

    virtual const TexPatAnmResult *GetResult(TexPatAnmResult *pResult,
                                             u32 idx) = 0;

    virtual AnmObjTexPatRes *Attach(int idx, AnmObjTexPatRes *pRes);
    virtual AnmObjTexPatRes *Detach(int idx);
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

    NW4R_G3D_RTTI_DECL_DERIVED(AnmObjTexPat, AnmObj);
};

class AnmObjTexPatNode : public AnmObjTexPat {
public:
    AnmObjTexPatNode(
        MEMAllocator *pAllocator, u16 *pBindingBuf, int numBinding, AnmObjTexPatRes **ppChildrenBuf, int numChildren
    );
    virtual void G3dProc(u32 task, u32 param, void *pInfo);
    virtual ~AnmObjTexPatNode();

    virtual void SetFrame(f32 frame);
    virtual f32 GetFrame() const;
    virtual void UpdateFrame();

    virtual void SetUpdateRate(f32 rate);
    virtual f32 GetUpdateRate() const;

    virtual bool Bind(const ResMdl mdl);
    virtual void Release();

    virtual AnmObjTexPatRes *Attach(int idx, AnmObjTexPatRes *pRes);
    virtual AnmObjTexPatRes *Detach(int idx);
    virtual void DetachAll();

    inline int Size() {
        return mChildrenArraySize;
    }

    AnmObjTexPatRes *GetNode(int i) {
        return mpChildrenArray[i];
    }

protected:
    int mChildrenArraySize;
    AnmObjTexPatRes **mpChildrenArray;

    NW4R_G3D_RTTI_DECL_DERIVED(AnmObjTexPatNode, AnmObjTexPat);
};

class AnmObjTexPatOverride : public AnmObjTexPatNode {
public:
    static AnmObjTexPatOverride *Construct(MEMAllocator *pAllocator, u32 *pSize, ResMdl mdl, int numChildren);

    AnmObjTexPatOverride(
        MEMAllocator *pAllocator, u16 *pBindingBuf, int numBinding, AnmObjTexPatRes **ppChildrenBuf, int numChildren
    )
        : AnmObjTexPatNode(pAllocator, pBindingBuf, numBinding, ppChildrenBuf, numChildren) {}

    virtual ~AnmObjTexPatOverride() {}

    virtual const TexPatAnmResult *GetResult(TexPatAnmResult *pResult,
                                             u32 idx);

    NW4R_G3D_RTTI_DECL_DERIVED(AnmObjTexPatOverride, AnmObjTexPatNode);
};

class AnmObjTexPatRes : public AnmObjTexPat, protected FrameCtrl {
public:
    static AnmObjTexPatRes *Construct(MEMAllocator *pAllocator, u32 *pSize, ResAnmTexPat pat, ResMdl mdl, bool cache);

    AnmObjTexPatRes(
        MEMAllocator *pAllocator, ResAnmTexPat pat, u16 *pBindingBuf, int numBinding, TexPatAnmResult *pCacheBuf
    );
    virtual void G3dProc(u32 task, u32 param, void *pInfo);
    virtual ~AnmObjTexPatRes() {}

    virtual void SetFrame(f32 frame);
    virtual f32 GetFrame() const;
    virtual void UpdateFrame();

    virtual void SetUpdateRate(f32 rate);
    virtual f32 GetUpdateRate() const;

    virtual bool Bind(const ResMdl mdl);

    virtual const TexPatAnmResult *GetResult(TexPatAnmResult *pResult,
                                             u32 idx);

    void UpdateCache();

    ResAnmTexPat GetResAnm() {
        return mRes;
    }

    void SetPlayPolicy(PlayPolicyFunc pFunc) {
        FrameCtrl::SetPlayPolicy(pFunc);
    }

private:
    ResAnmTexPat mRes;
    TexPatAnmResult *const mpResultCache;

    NW4R_G3D_RTTI_DECL_DERIVED(AnmObjTexPatRes, AnmObjTexPat);
};

}
}

#endif
