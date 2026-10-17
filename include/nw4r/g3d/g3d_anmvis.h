#ifndef NW4R_G3D_ANM_VIS_H
#define NW4R_G3D_ANM_VIS_H
#include <nw4r/types_nw4r.h>

#include <nw4r/g3d/g3d_anmobj.h>
#include <nw4r/g3d/res/g3d_resanmvis.h>

namespace nw4r {
namespace g3d {

void ApplyVisAnmResult(ResMdl mdl, AnmObjVis *pObj);
void ApplyVisAnmResult(u8 *pByteVec, ResMdl mdl, AnmObjVis *pObj);

class AnmObjVisRes;

class AnmObjVis : public AnmObj {
public:
    AnmObjVis(MEMAllocator *pAllocator, u16 *pBindingBuf, int numBinding);
    virtual void G3dProc(u32 task, u32 param, void *pInfo);
    virtual ~AnmObjVis() {}

    virtual void SetFrame(f32 frame) = 0;
    virtual f32 GetFrame() const = 0;
    virtual void UpdateFrame() = 0;

    virtual void SetUpdateRate(f32 rate) = 0;
    virtual f32 GetUpdateRate() const = 0;

    virtual bool Bind(const ResMdl mdl) = 0;
    virtual void Release();

    virtual bool GetResult(u32 idx) = 0;

    virtual AnmObjVisRes *Attach(int idx, AnmObjVisRes *pRes);
    virtual AnmObjVisRes *Detach(int idx);
    void DetachAll();

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

    NW4R_G3D_RTTI_DECL_DERIVED(AnmObjVis, AnmObj);
};

class AnmObjVisNode : public AnmObjVis {
public:
    AnmObjVisNode(MEMAllocator *pAllocator, u16 *pBindingBuf, int numBinding);
    virtual void G3dProc(u32 task, u32 param, void *pInfo);
    virtual ~AnmObjVisNode();

    virtual void SetFrame(f32 frame);
    virtual f32 GetFrame() const;
    virtual void UpdateFrame();

    virtual void SetUpdateRate(f32 rate);
    virtual f32 GetUpdateRate() const;

    virtual bool Bind(const ResMdl mdl);
    virtual void Release();

    virtual bool GetResult(u32 idx) = 0;

    virtual AnmObjVisRes *Attach(int idx, AnmObjVisRes *pRes);
    virtual AnmObjVisRes *Detach(int idx);

protected:
    AnmObjVisRes *mpChildren[MAX_CHILD];

    NW4R_G3D_RTTI_DECL_DERIVED(AnmObjVisNode, AnmObjVis);
};

class AnmObjVisOR : public AnmObjVisNode {
public:
    static AnmObjVisOR *Construct(MEMAllocator *pAllocator, u32 *pSize, ResMdl mdl);

    AnmObjVisOR(MEMAllocator *pAllocator, u16 *pBindingBuf, int numBinding)
        : AnmObjVisNode(pAllocator, pBindingBuf, numBinding) {}

    virtual ~AnmObjVisOR() {}

    virtual bool GetResult(u32 idx);

    NW4R_G3D_RTTI_DECL_DERIVED(AnmObjVisOR, AnmObjVisNode);
};

class AnmObjVisRes : public AnmObjVis, protected FrameCtrl {
public:
    static AnmObjVisRes *Construct(MEMAllocator *pAllocator, u32 *pSize, ResAnmVis vis, ResMdl mdl);

    AnmObjVisRes(MEMAllocator *pAllocator, ResAnmVis vis, u16 *pBindingBuf, int numBinding);
    virtual ~AnmObjVisRes() {}

    virtual void SetFrame(f32 frame);
    virtual f32 GetFrame() const;
    virtual void UpdateFrame();

    virtual void SetUpdateRate(f32 rate);
    virtual f32 GetUpdateRate() const;

    virtual bool Bind(const ResMdl mdl);

    virtual bool GetResult(u32 idx);

    ResAnmVis GetResAnm() {
        return mRes;
    }

    void SetPlayPolicy(PlayPolicyFunc pFunc) {
        FrameCtrl::SetPlayPolicy(pFunc);
    }

private:
    ResAnmVis mRes;

    NW4R_G3D_RTTI_DECL_DERIVED(AnmObjVisRes, AnmObjVis);
};

}
}

#endif
