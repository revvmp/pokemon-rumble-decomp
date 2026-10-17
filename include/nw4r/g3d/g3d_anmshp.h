#ifndef NW4R_G3D_ANM_SHP_H
#define NW4R_G3D_ANM_SHP_H
#include <nw4r/types_nw4r.h>

#include "nw4r/g3d/g3d_anmobj.h"
#include "nw4r/g3d/res/g3d_resanmshp.h"

namespace nw4r {
namespace g3d {

class AnmObjShpRes;

class AnmObjShp : public AnmObj {
public:
    AnmObjShp(MEMAllocator *pAllocator, u16 *pBindingBuf, int numBinding);
    virtual void G3dProc(u32 task, u32 param, void *pInfo) = 0;
    virtual ~AnmObjShp() {}

    virtual void SetFrame(f32 frame) = 0;
    virtual f32 GetFrame() const = 0;
    virtual void UpdateFrame() = 0;

    virtual void SetUpdateRate(f32 rate) = 0;
    virtual f32 GetUpdateRate() const = 0;

    virtual bool Bind(const ResMdl mdl) = 0;
    virtual void Release();

    virtual const ShpAnmResult *GetResult(ShpAnmResult *pResult,
                                          u32 idx) = 0;

    virtual AnmObjShpRes *Attach(int idx, AnmObjShpRes *pRes);
    virtual AnmObjShpRes *Detach(int idx);
    virtual void DetachAll();

    virtual void SetWeight(int idx, f32 weight);
    virtual f32 GetWeight(int idx) const;

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

    NW4R_G3D_RTTI_DECL_DERIVED(AnmObjShp, AnmObj);
};

class AnmObjShpNode : public AnmObjShp {
public:
    AnmObjShpNode(
        MEMAllocator *pAllocator, u16 *pBindingBuf, int numBinding, AnmObjShpRes **ppChildrenBuf, int numChildren
    );
    virtual void G3dProc(u32 task, u32 param, void *pInfo);
    virtual ~AnmObjShpNode();

    virtual void SetFrame(f32 frame);
    virtual f32 GetFrame() const;
    virtual void UpdateFrame();

    virtual void SetUpdateRate(f32 rate);
    virtual f32 GetUpdateRate() const;

    virtual bool Bind(const ResMdl mdl);
    virtual void Release();

    virtual AnmObjShpRes *Attach(int idx, AnmObjShpRes *pRes);
    virtual AnmObjShpRes *Detach(int idx);
    virtual void DetachAll();

protected:
    int mChildrenArraySize;
    AnmObjShpRes **mpChildrenArray;

    NW4R_G3D_RTTI_DECL_DERIVED(AnmObjShpNode, AnmObjShp);
};

class AnmObjShpRes : public AnmObjShp, protected FrameCtrl {
public:
    static AnmObjShpRes *Construct(MEMAllocator *pAllocator, u32 *pSize, ResAnmShp shp, ResMdl mdl, bool cache);

    AnmObjShpRes(
        MEMAllocator *pAllocator, ResAnmShp shp, u16 *pBindingBuf, ShpAnmVtxSet *pVtxSetBuf, int numBinding,
        ShpAnmResult *pCacheBuf
    );
    virtual void G3dProc(u32 task, u32 param, void *pInfo);
    virtual ~AnmObjShpRes() {}

    virtual void SetFrame(f32 frame);
    virtual f32 GetFrame() const;
    virtual void UpdateFrame();

    virtual void SetUpdateRate(f32 rate);
    virtual f32 GetUpdateRate() const;

    virtual bool Bind(const ResMdl mdl);

    virtual const ShpAnmResult *GetResult(ShpAnmResult *pResult,
                                          u32 idx);

    void UpdateCache();

    ResAnmShp GetResAnm() {
        return mRes;
    }

    void SetPlayPolicy(PlayPolicyFunc pFunc) {
        FrameCtrl::SetPlayPolicy(pFunc);
    }

private:
    ResAnmShp mRes;
    ShpAnmVtxSet *const mpVtxSetArray;
    ShpAnmResult *const mpResultCache;

    NW4R_G3D_RTTI_DECL_DERIVED(AnmObjShpRes, AnmObjShp);
};

}
}

#endif
