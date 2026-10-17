#ifndef NW4R_G3D_ANM_CLR_H
#define NW4R_G3D_ANM_CLR_H
#include <nw4r/types_nw4r.h>

#include <nw4r/g3d/g3d_anmobj.h>
#include <nw4r/g3d/res/g3d_resanmclr.h>
#include <nw4r/g3d/res/g3d_resmat.h>

namespace nw4r {
namespace g3d {

void ApplyClrAnmResult(ResMatChan chan, ResMatTevColor tev, const ClrAnmResult *pResult);

class AnmObjMatClrRes;

class AnmObjMatClr : public AnmObj {
public:
    AnmObjMatClr(MEMAllocator *pAllocator, u16 *pBindingBuf, int numBinding);
    virtual void G3dProc(u32 task, u32 param, void *pInfo) = 0;
    virtual ~AnmObjMatClr() {}

    virtual void SetFrame(f32 frame) = 0;
    virtual f32 GetFrame() const = 0;
    virtual void UpdateFrame() = 0;

    virtual void SetUpdateRate(f32 rate) = 0;
    virtual f32 GetUpdateRate() const = 0;

    virtual bool Bind(const ResMdl mdl) = 0;
    virtual void Release();

    virtual const ClrAnmResult *GetResult(ClrAnmResult *pResult,
                                          u32 idx) = 0;

    virtual AnmObjMatClrRes *Attach(int idx, AnmObjMatClrRes *pRes);
    virtual AnmObjMatClrRes *Detach(int idx);
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
    static const int DEFAULT_MAX_CHILDREN = 4;

protected:
    int mNumBinding;
    u16 *const mpBinding;

    NW4R_G3D_RTTI_DECL_DERIVED(AnmObjMatClr, AnmObj);
};

class AnmObjMatClrNode : public AnmObjMatClr {
public:
    AnmObjMatClrNode(
        MEMAllocator *pAllocator, u16 *pBindingBuf, int numBinding, AnmObjMatClrRes **ppChildrenBuf, int numChildren
    );
    virtual void G3dProc(u32 task, u32 param, void *pInfo);
    virtual ~AnmObjMatClrNode();

    virtual void SetFrame(f32 frame);
    virtual f32 GetFrame() const;
    virtual void UpdateFrame();

    virtual void SetUpdateRate(f32 rate);
    virtual f32 GetUpdateRate() const;

    virtual bool Bind(const ResMdl mdl);
    virtual void Release();

    virtual AnmObjMatClrRes *Attach(int idx, AnmObjMatClrRes *pRes);
    virtual AnmObjMatClrRes *Detach(int idx);
    virtual void DetachAll();

    int Size() const {
        return mChildrenArraySize;
    }

protected:
    int mChildrenArraySize;
    AnmObjMatClrRes **mpChildrenArray;

    NW4R_G3D_RTTI_DECL_DERIVED(AnmObjMatClrNode, AnmObjMatClr);
};

class AnmObjMatClrOverride : public AnmObjMatClrNode {
public:
    static AnmObjMatClrOverride *Construct(MEMAllocator *pAllocator, u32 *pSize, ResMdl mdl, int numChildren);

    AnmObjMatClrOverride(
        MEMAllocator *pAllocator, u16 *pBindingBuf, int numBinding, AnmObjMatClrRes **ppChildrenBuf, int numChildren
    )
        : AnmObjMatClrNode(pAllocator, pBindingBuf, numBinding, ppChildrenBuf, numChildren) {}

    virtual ~AnmObjMatClrOverride() {}

    virtual const ClrAnmResult *GetResult(ClrAnmResult *pResult,
                                          u32 idx);

    NW4R_G3D_RTTI_DECL_DERIVED(AnmObjMatClrOverride, AnmObjMatClrNode);
};

class AnmObjMatClrRes : public AnmObjMatClr, protected FrameCtrl {
public:
    static AnmObjMatClrRes *Construct(MEMAllocator *pAllocator, u32 *pSize, ResAnmClr clr, ResMdl mdl, bool cache);

    AnmObjMatClrRes(MEMAllocator *pAllocator, ResAnmClr clr, u16 *pBindingBuf, int numBinding, ClrAnmResult *pCacheBuf);
    virtual void G3dProc(u32 task, u32 param, void *pInfo);
    virtual ~AnmObjMatClrRes() {}

    virtual void SetFrame(f32 frame);
    virtual f32 GetFrame() const;
    virtual void UpdateFrame();

    virtual void SetUpdateRate(f32 rate);
    virtual f32 GetUpdateRate() const;

    virtual bool Bind(const ResMdl mdl);

    virtual const ClrAnmResult *GetResult(ClrAnmResult *pResult,
                                          u32 idx);

    void UpdateCache();

    ResAnmClr GetResAnm() {
        return mRes;
    }

    void SetPlayPolicy(PlayPolicyFunc pFunc) {
        FrameCtrl::SetPlayPolicy(pFunc);
    }

private:
    ResAnmClr mRes;
    ClrAnmResult *const mpResultCache;

    NW4R_G3D_RTTI_DECL_DERIVED(AnmObjMatClrRes, AnmObjMatClr);
};

}
}

#endif
