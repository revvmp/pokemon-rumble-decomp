#ifndef NW4R_G3D_SCN_MDL_SMPL_H
#define NW4R_G3D_SCN_MDL_SMPL_H
#include <nw4r/types_nw4r.h>

#include "nw4r/g3d/g3d_calcview.h"
#include "nw4r/g3d/g3d_scnobj.h"
#include "nw4r/g3d/res/g3d_resmdl.h"

#include <nw4r/math/math_geometry.h>

namespace nw4r {
namespace g3d {

class AnmObj;
class AnmObjChr;
class AnmObjVis;
class AnmObjMatClr;
class AnmObjTexPat;
class AnmObjTexSrt;
class ICalcWorldCallback;

class ScnMdlSimple : public ScnLeaf {
public:
    enum AnmObjType {
        ANMOBJTYPE_CHR,
        ANMOBJTYPE_VIS,
        ANMOBJTYPE_MATCLR,
        ANMOBJTYPE_TEXPAT,
        ANMOBJTYPE_TEXSRT,
        ANMOBJTYPE_SHP,
        ANMOBJTYPE_NOT_SPECIFIED,

        ANMOBJTYPE_VTX = ANMOBJTYPE_SHP
    };

    enum ByteCodeType {
        BYTE_CODE_CALC,
        BYTE_CODE_MIX,
        BYTE_CODE_DRAW_OPA,
        BYTE_CODE_DRAW_XLU,
    };

#define OPT(KEY, VALUE) OPTION_##KEY = (0x20000 | (VALUE))
    enum ScnMdlSimpleOption {
        OPT(NONE, 0),
        OPT(IGNORE_ANMCHR_TRANS, 1),
    };
#undef OPT

public:
    static ScnMdlSimple *Construct(MEMAllocator *pAllocator, u32 *pSize, ResMdl mdl, int numView);

    ScnMdlSimple(
        MEMAllocator *pAllocator, ResMdl mdl, math::MTX34 *pWorldMtxArray, u32 *pWorldMtxAttribArray,
        math::MTX34 *pViewPosMtxArray, math::MTX33 *pViewNrmMtxArray, math::MTX34 *pViewTexMtxArray, int numView,
        int numViewMtx
    );

    virtual void G3dProc(u32 task, u32 param, void *pInfo);
    virtual ~ScnMdlSimple();

    virtual bool SetScnObjOption(u32 option, u32 value);
    virtual bool GetScnObjOption(u32 option, u32 *pValue) const;

    virtual bool SetAnmObj(AnmObj *pObj, AnmObjType type);
    virtual bool RemoveAnmObj(AnmObj *pObj);
    virtual AnmObj *RemoveAnmObj(AnmObjType type);
    virtual AnmObj *GetAnmObj(AnmObjType type);
    virtual const AnmObj *GetAnmObj(AnmObjType type) const;

    bool GetScnMtxPos(math::MTX34 *pMtx, ScnObjMtxType type, u32 idx) const;

    void UpdateFrame();

    void EnableScnMdlCallbackTiming(Timing timing);
    void DisableScnMdlCallbackTiming(Timing timing);

    math::MTX34 *GetViewPosMtxArray();
    const math::MTX34 *GetViewPosMtxArray() const;

    math::MTX33 *GetViewNrmMtxArray();
    const math::MTX33 *GetViewNrmMtxArray() const;

    math::MTX34 *GetViewTexMtxArray();
    const math::MTX34 *GetViewTexMtxArray() const;

    const u8 *GetByteCode(ByteCodeType type) const;

    ResMdl GetResMdl() {
        return mResMdl;
    }
    const ResMdl GetResMdl() const {
        return mResMdl;
    }

    math::MTX34 *GetWldMtxArray() {
        return mpWorldMtxArray;
    }
    u32 *GetWldMtxAttribArray() {
        return mpWorldMtxAttribArray;
    }

    u32 GetNumViewMtx() const {
        return mNumViewMtx;
    }

    const u8 *GetByteCodeCalc() {
        return mpByteCodeCalc;
    }
    const u8 *GetByteCodeMix() {
        return mpByteCodeMix;
    }
    const u8 *GetByteCodeDrawOpa() {
        return mpByteCodeDrawOpa;
    }
    const u8 *GetByteCodeDrawXlu() {
        return mpByteCodeDrawXlu;
    }

    ResMdlDrawMode GetDrawMode() const {
        return mDrawMode;
    }

    void SetScnMdlCallback(ICalcWorldCallback *pCallback) {
        mpCalcWorldCallback = pCallback;
    }
    ICalcWorldCallback *GetScnMdlCallback() {
        return mpCalcWorldCallback;
    }

    u8 GetScnMdlCallbackTiming() const {
        return mCwcbTiming;
    }

    void SetScnMdlCallbackNodeID(u32 id) {
        mCwcbNodeID = id;
    }
    u32 GetScnMdlCallbackNodeID() const {
        return mCwcbNodeID;
    }

    AnmObjChr *GetAnmObjChr() {
        return mpAnmObjChr;
    }
    AnmObjVis *GetAnmObjVis() {
        return mpAnmObjVis;
    }
    AnmObjMatClr *GetAnmObjMatClr() {
        return mpAnmObjMatClr;
    }
    AnmObjTexPat *GetAnmObjTexPat() {
        return mpAnmObjTexPat;
    }
    AnmObjTexSrt *GetAnmObjTexSrt() {
        return mpAnmObjTexSrt;
    }

protected:
    enum ScnMdlSmplFlag {
        SCNMDLSMPLFLAG_LC_DMA = (1 << 0),
    };

    static const int VIEW_MAX = 16;

    static const int MTX_CACHE_MIN = 8;
    static const int MTX_CACHE_MAX = sizeof(detail::MtxCacheMap) / sizeof(math::MTX34) + 1;

protected:
    void ScnMdlSmpl_CalcPosture(u32 param, const math::MTX34 *pParent);

    void ScnMdlSmpl_G3DPROC_GATHER_SCNOBJ(u32 param, IScnObjGather *pCollection);
    void ScnMdlSmpl_G3DPROC_CALC_WORLD(u32 param, const math::MTX34 *pParent);
    void ScnMdlSmpl_G3DPROC_CALC_MAT(u32 param, void *pInfo);
    void ScnMdlSmpl_G3DPROC_CALC_VIEW(u32 param, const math::MTX34 *pCamera);
    void ScnMdlSmpl_G3DPROC_DRAW_OPA(u32 param, void *pInfo);
    void ScnMdlSmpl_G3DPROC_DRAW_XLU(u32 param, void *pInfo);

    void ScnMdlSmpl_G3DPROC_UPDATEFRAME(u32  , void *  ) {
        UpdateFrame();
    }

private:
    ResMdl mResMdl;
    math::MTX34 *mpWorldMtxArray;
    u32 *mpWorldMtxAttribArray;
    math::MTX34 *mpViewPosMtxArray;
    math::MTX33 *mpViewNrmMtxArray;
    math::MTX34 *mpViewTexMtxArray;
    u8 mNumView;
    u8 mCurView;
    u16 mNumViewMtx;
    u32 mFlagScnMdlSimple;

    const u8 *mpByteCodeCalc;
    const u8 *mpByteCodeMix;
    const u8 *mpByteCodeDrawOpa;
    const u8 *mpByteCodeDrawXlu;
    ResMdlDrawMode mDrawMode;

    ICalcWorldCallback *mpCalcWorldCallback;
    u8 mCwcbTiming;
    u8 mCwcbDeleteOption;
    u16 mCwcbNodeID;

    AnmObjChr *mpAnmObjChr;
    AnmObjVis *mpAnmObjVis;
    AnmObjMatClr *mpAnmObjMatClr;
    AnmObjTexPat *mpAnmObjTexPat;
    AnmObjTexSrt *mpAnmObjTexSrt;

    NW4R_G3D_RTTI_DECL_DERIVED(ScnMdlSimple, ScnLeaf);
};

}
}

#endif
