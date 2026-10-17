#ifndef NW4R_G3D_CALC_WORLD_H
#define NW4R_G3D_CALC_WORLD_H
#include <nw4r/types_nw4r.h>


#include "nw4r/g3d/res/g3d_resmdl.h"

#include <nw4r/math/math_types.h>

namespace nw4r {
namespace g3d {

class AnmObjChr;
struct ChrAnmResult;
class FuncObjCalcWorld;
class WorldMtxManip;

void CalcWorld(
    math::MTX34 *pModelMtxArray, u32 *pModelMtxAttribArray, const u8 *pByteCode, const math::MTX34 *pBaseMtx,
    ResMdl mdl, AnmObjChr *pAnmChr, FuncObjCalcWorld *pFuncObj, u32 rootAttrib
);

void CalcWorld(
    math::MTX34 *pModelMtxArray, u32 *pModelMtxAttribArray, const u8 *pByteCode, const math::MTX34 *pBaseMtx,
    ResMdl mdl, AnmObjChr *pAnmChr, FuncObjCalcWorld *pFuncObj
);

void CalcSkinning(math::MTX34 *pModelMtxArray, u32 *pModelMtxAttribArray, const ResMdl mdl, const u8 *pByteCode);

class ICalcWorldCallback {
public:
    virtual ~ICalcWorldCallback() {};

    virtual void ExecCallbackA(ChrAnmResult *pResult, ResMdl mdl,
                               FuncObjCalcWorld *pFuncObj) {};

    virtual void ExecCallbackB(WorldMtxManip *pManip, ResMdl mdl,
                               FuncObjCalcWorld *pFuncObj) {};

    virtual void ExecCallbackC(math::MTX34 *pMtxArray, ResMdl mdl,
                               FuncObjCalcWorld *pFuncObj) {};
};

class WorldMtxManip {
public:
    WorldMtxManip(math::MTX34 *pM, math::VEC3 *pS, u32 *pWMAttr) : mpM(pM), mpS(pS), mpWMAttr(pWMAttr) {}

    void SetScale(f32 x, f32 y, f32 z);
    void GetMtx(nw4r::math::MTX34 *out) {
        if (out != nullptr) {
            math::MTX34Copy(out, mpM);
        }
    }

    void SetMtx(const nw4r::math::MTX34 *arg) {
        if (arg != nullptr) {
            math::MTX34Copy(mpM, arg);
        } else {
            math::MTX34Identity(mpM);
        }
    }

    void SetMtxUnchecked(const nw4r::math::MTX34 *arg) {
        math::MTX34Copy(mpM, arg);
    }

private:
    math::MTX34 *mpM;
    math::VEC3 *mpS;
    u32 *mpWMAttr;
};

class FuncObjCalcWorld {
public:
    enum CallbackTiming { CALLBACK_TIMING_A = 1, CALLBACK_TIMING_B = 2, CALLBACK_TIMING_C = 4 };
    FuncObjCalcWorld(ICalcWorldCallback *pCallback, u32 timing, u32 nodeID)
        : mpCallback(pCallback), mTiming(timing), mNodeID(nodeID) {}

    ~FuncObjCalcWorld() {}

    u16 GetCallbackNodeID() const {
        return mNodeID;
    }

    void SetCallbackNodeID(u16 id) {
        mNodeID = id;
    }

    void CheckCallbackA(u32 nodeID, ChrAnmResult *pResult, ResMdl mdl) {
        if (nodeID == mNodeID && (mTiming & CALLBACK_TIMING_A)) {
            mpCallback->ExecCallbackA(pResult, mdl, this);
        }
    }

    void CheckCallbackB(u32 nodeID, math::MTX34 *pM, math::VEC3 *pS, u32 *pWMAttr, ResMdl mdl) {
        if (nodeID == mNodeID && (mTiming & CALLBACK_TIMING_B)) {
            WorldMtxManip manip(pM, pS, pWMAttr);
            mpCallback->ExecCallbackB(&manip, mdl, this);
        }
    }

    void CheckCallbackC(math::MTX34 *pMtxArray, ResMdl mdl) {
        if (mTiming & CALLBACK_TIMING_C) {
            mpCallback->ExecCallbackC(pMtxArray, mdl, this);
        }
    }

private:
    ICalcWorldCallback *mpCallback;
    u8 mTiming;
    u16 mNodeID;
};

}
}

#endif
