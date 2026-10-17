#ifndef NW4R_G3D_SCN_OBJ_H
#define NW4R_G3D_SCN_OBJ_H
#include <nw4r/types_nw4r.h>

#include <nw4r/g3d/g3d_obj.h>

#include <nw4r/math/math_geometry.h>
#include <nw4r/math/math_types.h>

namespace nw4r {
namespace g3d {

extern const math::FRUSTUM *gpCullingFrustum;

enum ResMdlDrawMode {
    RESMDL_DRAWMODE_SORT_OPA_NONE = 0,
    RESMDL_DRAWMODE_SORT_OPA_Z = (1 << 0),

    RESMDL_DRAWMODE_SORT_XLU_NONE = 0,
    RESMDL_DRAWMODE_SORT_XLU_Z = (1 << 1),

    RESMDL_DRAWMODE_IGNORE_MATERIAL = (1 << 2),
    RESMDL_DRAWMODE_FORCE_LIGHTOFF = (1 << 3),
    RESMDL_DRAWMODE_NOPPCSYNC = (1 << 4),

    RESMDL_DRAWMODE_DEFAULT = RESMDL_DRAWMODE_SORT_XLU_Z,
    REDMDL_DRAWMODE_SORT_NONE = 0,
    RESMDL_DRAWMODE_SORT_Z = RESMDL_DRAWMODE_SORT_OPA_Z | RESMDL_DRAWMODE_SORT_XLU_Z,
};

class IScnObjCallback;

class ScnObj : public G3dObj {
public:
    enum ForEachResult {
        FOREACHRESULT_OK,
        FOREACHRESULT_CONTINUE,
        FOREACHRESULT_RETURN,
        FOREACHRESULT_GOBACK
    };

    typedef ForEachResult (*ForEachFunc)(ScnObj *pParent, void *pInfo);

    enum ScnObjMtxType {
        MTX_LOCAL,
        MTX_WORLD,
        MTX_VIEW,
        MTX_TYPE_MAX
    };

    enum ScnObjBoundingVolumeType {
        BOUNDINGVOLUME_AABB_LOCAL,
        BOUNDINGVOLUME_AABB_WORLD,
        BOUNDINGVOLUME_MAX
    };

#define OPT(KEY, VALUE) OPTION_##KEY = (0x00000 | (VALUE))
    enum ScnObjOption {
        OPT(NONE, 0),
        OPT(DISABLE_GATHER_SCNOBJ, 1),
        OPT(DISABLE_CALC_WORLD, 2),
        OPT(DISABLE_CALC_MAT, 3),
        OPT(DISABLE_CALC_VTX, 4),
        OPT(DISABLE_CALC_VIEW, 5),
        OPT(DISABLE_DRAW_OPA, 6),
        OPT(DISABLE_DRAW_XLU, 7),
        OPT(DISABLE_UPDATEFRAME, 8),
        OPT(ENABLE_CULLING, 9),
    };
#undef OPT

    enum Timing {
        CALLBACK_TIMING_A = (1 << 0),
        CALLBACK_TIMING_B = (1 << 1),
        CALLBACK_TIMING_C = (1 << 2)
    };

    enum ExecOp {
        EXECOP_CALC_WORLD = (1 << 0),
        EXECOP_CALC_MAT = (1 << 1),
        EXECOP_CALC_VIEW = (1 << 2),
        EXECOP_DRAW_OPA = (1 << 4),
        EXECOP_DRAW_XLU = (1 << 5)
    };

public:
    explicit ScnObj(MEMAllocator *pAllocator);

    virtual void G3dProc(u32 task, u32 param, void *pInfo) = 0;
    virtual ~ScnObj();

    virtual ForEachResult ForEach(ForEachFunc pFunc, void *pInfo,
                                  bool postOrder) = 0;

    virtual bool SetScnObjOption(u32 option, u32 value);
    virtual bool GetScnObjOption(u32 option, u32 *pValue) const;

    virtual f32 GetValueForSortOpa() const;
    virtual f32 GetValueForSortXlu() const;

    virtual void CalcWorldMtx(const math::MTX34 *pParent,
                              u32 *pParam);

    void CalcViewMtx(const math::MTX34 *pCamera);

    bool SetMtx(ScnObjMtxType type, const math::MTX34 *pMtx);
    bool SetMtx(ScnObjMtxType type, const math::MTX34 &rMtx);
    bool GetMtx(ScnObjMtxType type, math::MTX34 *pMtx) const;
    const math::MTX34 *GetMtxPtr(ScnObjMtxType type) const {
        return &mMtxArray[type];
    }

    void SetPriorityDrawOpa(int prio);
    int GetPriorityDrawOpa() const {
        return mPriorityDrawOpa;
    }

    void SetPriorityDrawXlu(int prio);
    int GetPriorityDrawXlu() const {
        return mPriorityDrawXlu;
    }

    void EnableScnObjCallbackTiming(Timing timing);
    void DisableScnObjCallbackTiming(Timing timing);
    void EnableScnObjCallbackExecOp(ExecOp op);

    bool SetBoundingVolume(ScnObjBoundingVolumeType type, const math::AABB *pAABB);
    bool GetBoundingVolume(ScnObjBoundingVolumeType type, math::AABB *pAABB) const;

    bool SetBoundingVolume(const math::AABB *pAABB) {
        return SetBoundingVolume(BOUNDINGVOLUME_AABB_LOCAL, pAABB);
    }

    void SetCallback(IScnObjCallback *cb) {
        mpFuncObjExec = cb;
    }

    IScnObjCallback *GetScnObjCallback() {
        return mpFuncObjExec;
    }

protected:
    enum ScnObjFlag {
        SCNOBJFLAG_DISABLE_CALC_WORLD = (1 << 0),
        SCNOBJFLAG_DISABLE_CALC_MAT = (1 << 1),
        SCNOBJFLAG_DISABLE_CALC_VTX = (1 << 2),
        SCNOBJFLAG_DISABLE_CALC_VIEW = (1 << 3),
        SCNOBJFLAG_DISABLE_GATHER_SCNOBJ = (1 << 4),
        SCNOBJFLAG_DISABLE_DRAW_OPA = (1 << 5),
        SCNOBJFLAG_DISABLE_DRAW_XLU = (1 << 6),
        SCNOBJFLAG_DISABLE_UPDATEFRAME = (1 << 7),
        SCNOBJFLAG_IGNORE_ANMCHR_TRANS = (1 << 8),

        SCNOBJFLAG_ENABLE_CULLING = (1 << 28),
        SCNOBJFLAG_NOT_GATHER_DRAW_OPA = (1 << 29),
        SCNOBJFLAG_NOT_GATHER_DRAW_XLU = (1 << 30),
        SCNOBJFLAG_MTX_LOCAL_IDENTITY = (1 << 31),

        SCNOBJFLAG_DISABLE_DRAW = SCNOBJFLAG_DISABLE_DRAW_OPA | SCNOBJFLAG_DISABLE_DRAW_XLU
    };

protected:
    void SetScnObjFlag(ScnObjFlag flag, u32 on) {
        if (on) {
            mScnObjFlags |= flag;
        } else {
            mScnObjFlags &= ~flag;
        }
    }
    u32 TestScnObjFlag(ScnObjFlag flag) const {
        return (mScnObjFlags & flag) != 0;
    }

    bool IsG3dProcDisabled(u32 task) const {
        if (task < __G3DPROC_OPTIONAL_END && ((1 << (task - 1)) & mScnObjFlags)) {
            return true;
        }

        return false;
    }

    inline void CheckCallback_CALC_VIEW(Timing timing, u32 param, void *pInfo);
    inline void CheckCallback_CALC_MAT(Timing timing, u32 param, void *pInfo);
    inline void CheckCallback_CALC_WORLD(Timing timing, u32 param, void *pInfo);
    inline void CheckCallback_DRAW_OPA(Timing timing, u32 param, void *pInfo);
    inline void CheckCallback_DRAW_XLU(Timing timing, u32 param, void *pInfo);

protected:
    math::MTX34 mMtxArray[MTX_TYPE_MAX];
    math::AABB mAABB[BOUNDINGVOLUME_MAX];

private:
    u32 mScnObjFlags;
    u8 mPriorityDrawOpa;
    u8 mPriorityDrawXlu;
    u8 PADDING_0xD2;
    u8 PADDING_0xD3;
    IScnObjCallback *mpFuncObjExec;
    u8 mCallbackTiming;
    u8 mCallbackDeleteOption;
    u16 mCallbackExecOpMask;

    NW4R_G3D_RTTI_DECL_DERIVED(ScnObj, G3dObj);
};

class IScnObjGather {
public:
    typedef bool (*LessThanFunc)(const ScnObj *pLhs, const ScnObj *pRhs);

    enum CullingStatus {
        CULLINGSTATUS_INTERSECT,
        CULLINGSTATUS_INSIDE,
        CULLINGSTATUS_OUTSIDE,
        CULLINGSTATUS_NOTEST
    };

    enum CheckStatus {
        CHECKSTATUS_GATHER_SCNOBJ,
        CHECKSTATUS_IGNORE_SCNOBJ
    };

public:
    virtual ~IScnObjGather() {}

    virtual CullingStatus Add(ScnObj *pObj, bool opa, bool xlu) = 0;
    virtual void Clear() = 0;
    virtual void ZSort() = 0;

    virtual void Sort() = 0;
    virtual void Sort(LessThanFunc pOpaFunc,
                      LessThanFunc pXluFunc) = 0;

    virtual void DrawOpa(ResMdlDrawMode *pForceMode) = 0;
    virtual void DrawXlu(ResMdlDrawMode *pForceMode) = 0;
};

class IScnObjCallback {
public:
    virtual ~IScnObjCallback() {}

    virtual void
    ExecCallback_CALC_WORLD(ScnObj::Timing , ScnObj * , u32 , void * ) {
    }

    virtual void
    ExecCallback_CALC_MAT(ScnObj::Timing , ScnObj * , u32 , void * ) {
    }

    virtual void
    ExecCallback_CALC_VIEW(ScnObj::Timing , ScnObj * , u32 , void * ) {
    }

    virtual void
    ExecCallback_DRAW_OPA(ScnObj::Timing , ScnObj * , u32 , void * ) {
    }

    virtual void
    ExecCallback_DRAW_XLU(ScnObj::Timing , ScnObj * , u32 , void * ) {
    }
};

void ScnObj::CheckCallback_CALC_VIEW(Timing timing, u32 param, void *pInfo) {
    if (mpFuncObjExec != NULL && (mCallbackExecOpMask & EXECOP_CALC_VIEW) && (mCallbackTiming & timing)) {
        mpFuncObjExec->ExecCallback_CALC_VIEW(timing, this, param, pInfo);
    }
}

void ScnObj::CheckCallback_CALC_MAT(Timing timing, u32 param, void *pInfo) {
    if (mpFuncObjExec != NULL && (mCallbackExecOpMask & EXECOP_CALC_MAT) && (mCallbackTiming & timing)) {
        mpFuncObjExec->ExecCallback_CALC_MAT(timing, this, param, pInfo);
    }
}

void ScnObj::CheckCallback_CALC_WORLD(Timing timing, u32 param, void *pInfo) {
    if (mpFuncObjExec != NULL && (mCallbackExecOpMask & EXECOP_CALC_WORLD) && (mCallbackTiming & timing)) {
        mpFuncObjExec->ExecCallback_CALC_WORLD(timing, this, param, pInfo);
    }
}

void ScnObj::CheckCallback_DRAW_OPA(Timing timing, u32 param, void *pInfo) {
    if (mpFuncObjExec != NULL && (mCallbackExecOpMask & EXECOP_DRAW_OPA) && (mCallbackTiming & timing)) {
        mpFuncObjExec->ExecCallback_DRAW_OPA(timing, this, param, pInfo);
    }
}

void ScnObj::CheckCallback_DRAW_XLU(Timing timing, u32 param, void *pInfo) {
    if (mpFuncObjExec != NULL && (mCallbackExecOpMask & EXECOP_DRAW_XLU) && (mCallbackTiming & timing)) {
        mpFuncObjExec->ExecCallback_DRAW_XLU(timing, this, param, pInfo);
    }
}

class ScnLeaf : public ScnObj {
public:
    enum ScaleProperty {
        NOT_SCALED,
        UNIFORM_SCALED,
        NONUNIFORM_SCALED,
    };

#define OPT(KEY, VALUE) OPTION_##KEY = (0x10000 | (VALUE))
    enum ScnLeafOption {
        OPT(NONE, 0),
        OPT(DISABLE_DRAW_ALL, 1),
    };
#undef OPT

public:
    explicit ScnLeaf(MEMAllocator *pAllocator) : ScnObj(pAllocator), mScale(1.0f, 1.0f, 1.0f) {}

    virtual void G3dProc(u32 task, u32 param, void *pInfo) = 0;
    virtual ~ScnLeaf() {}

    virtual ForEachResult ForEach(ForEachFunc pFunc, void *pInfo,
                                  bool postOrder);

    virtual bool SetScnObjOption(u32 option, u32 value);
    virtual bool GetScnObjOption(u32 option, u32 *pValue) const;

    virtual void CalcWorldMtx(const math::MTX34 *pParent,
                              u32 *pParam);

    ScaleProperty GetScaleProperty() const;

    inline void SetScale(f32 x, f32 y, f32 z) {
        mScale.x = x;
        mScale.y = y;
        mScale.z = z;
    }

    inline void SetScale(const math::VEC3 &scale) {
        mScale = scale;
    }

    inline void GetScale(math::VEC3 *scale) const {
        if (scale) {
            *scale = mScale;
        }
    }

protected:
    void DefG3dProcScnLeaf(u32 task, u32 param, void *pInfo);

private:
    math::VEC3 mScale;

    NW4R_G3D_RTTI_DECL_DERIVED(ScnLeaf, ScnObj);
};

class ScnGroup : public ScnObj {
public:
    static ScnGroup *Construct(MEMAllocator *pAllocator, u32 *pSize, u32 maxChildren);

    ScnGroup(MEMAllocator *pAllocator, ScnObj **ppObj, u32 capacity);

    virtual void G3dProc(u32 task, u32 param, void *pInfo);
    virtual ~ScnGroup();

    virtual ForEachResult ForEach(ForEachFunc pFunc, void *pInfo,
                                  bool postOrder);

    virtual bool Insert(u32 idx, ScnObj *pObj);
    virtual ScnObj *Remove(u32 idx);
    virtual bool Remove(ScnObj *pObj);

    ScnObj **Begin() {
        return mpScnObjArray;
    }
    ScnObj **End() {
        return mpScnObjArray + mNumScnObj;
    }

    ScnObj *operator[](u32 idx) {
        return mpScnObjArray[idx];
    }

    u32 Size() const {
        return mNumScnObj;
    }
    bool Empty() const {
        return mNumScnObj == 0;
    }

    bool PushBack(ScnObj *pObj) {
        return Insert(mNumScnObj, pObj);
    }
    ScnObj *PopBack() {
        if (!Empty()) {
            return Remove(Size() - 1);
        }

        return NULL;
    }

    void Clear() {
        while (!Empty()) {
            PopBack();
        }
    }

protected:
    void DefG3dProcScnGroup(u32 task, u32 param, void *pInfo);

private:
    void ScnGroup_G3DPROC_GATHER_SCNOBJ(u32 param, IScnObjGather *pCollection);
    void ScnGroup_G3DPROC_CALC_WORLD(u32 param, const math::MTX34 *pParent);
    void ScnGroup_G3DPROC_CALC_MAT(u32 param, void *pInfo);
    void ScnGroup_G3DPROC_CALC_VIEW(u32 param, const math::MTX34 *pCamera);

private:
    ScnObj **mpScnObjArray;
    u32 mSizeScnObj;
    u32 mNumScnObj;

    NW4R_G3D_RTTI_DECL_DERIVED(ScnGroup, ScnObj);
};

}
}

#endif
