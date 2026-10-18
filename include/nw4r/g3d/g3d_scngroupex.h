#ifndef NW4R_G3D_SCN_GROUP_EX_H
#define NW4R_G3D_SCN_GROUP_EX_H

#include <nw4r/g3d/g3d_scnroot.h>
#include <nw4r/g3d/res/g3d_resmdl.h>

namespace nw4r {
namespace g3d {
namespace detail {
class ModelDrawGather;
}

class ScnGroupEx : public ScnGroup {
public:
    static ScnGroupEx* Construct(MEMAllocator* pAllocator, u32* pSize,
                                 u32 maxChildren, u32 maxModels, ResMdl mdl);

    ScnGroupEx(MEMAllocator* pAllocator, ScnObj** ppChildren, u32 maxChildren,
               detail::ModelDrawGather* pGather)
        : ScnGroup(pAllocator, ppChildren, maxChildren), mpGather(pGather) {
        SetScnObjFlag(SCNOBJFLAG_NOT_GATHER_DRAW_OPA, FALSE);
        SetScnObjFlag(SCNOBJFLAG_NOT_GATHER_DRAW_XLU, FALSE);
    }

    virtual void G3dProc(u32 task, u32 param, void* pInfo);
    virtual ~ScnGroupEx();

    detail::ModelDrawGather* GetGather() const { return mpGather; }

private:
    detail::ModelDrawGather* mpGather;
    NW4R_G3D_RTTI_DECL_DERIVED(ScnGroupEx, ScnGroup);
};

namespace detail {

class GatherProxy : public IScnObjGather {
public:
    GatherProxy(IScnObjGather* pOuter, ModelDrawGather* pInner)
        : mpOuter(pOuter), mpInner(pInner) {}

    virtual ~GatherProxy();
    virtual CullingStatus Add(ScnObj* pObj, bool opa, bool xlu);
    virtual void Clear() {}
    virtual void ZSort() {}
    virtual void Sort() {}
    virtual void Sort(LessThanFunc, LessThanFunc) {}
    virtual void DrawOpa(ResMdlDrawMode*) {}
    virtual void DrawXlu(ResMdlDrawMode*) {}
    virtual CheckStatus CheckScnObj(ScnObj* pObj);

private:
    IScnObjGather* mpOuter;
    ModelDrawGather* mpInner;
};

class ModelDrawGather : public ScnObjGather {
public:
    ModelDrawGather(ScnObj** pOpa, ScnObj** pXlu, u32 capacity, ResMdl mdl);

    virtual ~ModelDrawGather() {}
    virtual void DrawOpa(ResMdlDrawMode* pForceMode);
    virtual void DrawXlu(ResMdlDrawMode* pForceMode);
    virtual CheckStatus CheckScnObj(ScnObj* pObj);
    virtual void SetResMdl(ResMdl mdl) {
        if (mdl != mResMdl) {
            mpDrawOpa = mdl.IsValid() ? mdl.GetResByteCode("DrawOpa") : nullptr;
            mpDrawXlu = mdl.IsValid() ? mdl.GetResByteCode("DrawXlu") : nullptr;
            mResMdl = mdl;
            Clear();
        }
    }
    virtual ResMdl GetResMdl() const;
    virtual u32 GetSignature() const;
    virtual bool IsSameResource(const ModelDrawGather* pOther) const;
    inline bool MatchesGroup(ScnObj* pObj) const;

private:
    void Draw(const u8* pCode, u32 count, ScnObj** ppObjects, ResMdlDrawMode* pForceMode, bool opa);

    ResMdl mResMdl;
    const u8* mpDrawOpa;
    const u8* mpDrawXlu;
};

inline bool ModelDrawGather::MatchesGroup(ScnObj* pObj) const {
    ScnGroupEx* pGroup = G3dObj::DynamicCast<ScnGroupEx>(pObj);
    return pGroup != nullptr && IsSameResource(pGroup->GetGather());
}

}

}
}

#endif
