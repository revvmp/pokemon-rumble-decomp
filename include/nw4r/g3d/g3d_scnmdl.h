#ifndef NW4R_G3D_SCN_MDL_H
#define NW4R_G3D_SCN_MDL_H

#include <nw4r/types_nw4r.h>

#include "nw4r/g3d/g3d_draw.h"
#include "nw4r/g3d/g3d_scnmdlsmpl.h"
#include "nw4r/g3d/res/g3d_resmat.h"

namespace nw4r {
namespace g3d {

class AnmObjShp;

class ScnMdl : public ScnMdlSimple {
public:
    class CopiedMatAccess {
    public:
        CopiedMatAccess(ScnMdl *pScnMdl, u32 id);

        bool IsValid() const {
            return mpScnMdl != NULL;
        }

        ResTexObj GetResTexObj(bool markDirty);
        ResTlutObj GetResTlutObj(bool markDirty);
        ResMatTexCoordGen GetResMatTexCoordGen(bool markDirty);
        ResTexSrt GetResTexSrt(bool markDirty);
        ResGenMode GetResGenMode(bool markDirty);
        ResMatPix GetResMatPix(bool markDirty);
        ResMatTevColor GetResMatTevColor(bool markDirty);
        ResTev GetResTev(bool markDirty);


    private:
        ScnMdl *mpScnMdl;
        u32 mMatID;
        ResTexObj mTexObj;
        ResTlutObj mTlutObj;
        ResTexSrt mTexSrt;
        ResMatChan mChan;
        ResGenMode mGenMode;
        ResMatMisc mMatMisc;
        ResMatPix mPix;
        ResMatTevColor mTevColor;
        ResMatIndMtxAndScale mIndMtxAndScale;
        ResMatTexCoordGen mTexCoordGen;
        ResTev mTev;
    };

    class CopiedVisAccess {
    public:
        CopiedVisAccess(ScnMdl *pScnMdl, u32 id);

        bool IsVisible() const;

    private:
        ScnMdl *mpScnMdl;
        u32 mNodeID;
        u8 *mpVis;
    };

#define OPT(KEY, VALUE) OPTION_##KEY = (0x30000 | (VALUE))
    enum ScnMdlOption {
        OPT(NONE, 0),
        OPT(VISBUFFER_REFRESH_NEEDED, 1),
    };
#undef OPT

public:
    static ScnMdl *Construct(MEMAllocator *pAllocator, u32 *pSize, ResMdl mdl, u32 bufferOption, int numView);

    ScnMdl(
        MEMAllocator *pAllocator, ResMdl mdl, math::MTX34 *pWorldMtxArray, u32 *pWorldMtxAttribArray,
        math::MTX34 *pViewPosMtxArray, math::MTX33 *pViewNrmMtxArray, math::MTX34 *pViewTexMtxArray, int numView,
        int numViewMtx, DrawResMdlReplacement *pReplacement, u32 *pMatBufferDirtyFlag, u32 replacementFlag
    );

    virtual void G3dProc(u32 task, u32 param, void *pInfo);
    virtual ~ScnMdl();

    virtual bool SetScnObjOption(u32 option, u32 value);
    virtual bool GetScnObjOption(u32 option, u32 *pValue) const;

    virtual bool SetAnmObj(AnmObj *pObj, AnmObjType type);
    virtual bool RemoveAnmObj(AnmObj *pObj);
    virtual AnmObj *RemoveAnmObj(AnmObjType type);
    virtual AnmObj *GetAnmObj(AnmObjType type);
    virtual const AnmObj *GetAnmObj(AnmObjType type) const;

    void InitBuffer();
    void CleanMatBuffer(u32 idx, u32 option);
    void CleanVisBuffer();

    AnmObjShp *GetAnmObjShp() {
        return mpAnmObjShp;
    }

    DrawResMdlReplacement &GetDrawResMdlReplacement() {
        return mReplacement;
    }

protected:
    void ScnMdl_G3DPROC_CALC_WORLD(u32 param, const math::MTX34 *pParent);
    void ScnMdl_G3DPROC_CALC_MAT(u32 param, void *pInfo);
    void ScnMdl_G3DPROC_CALC_VTX(u32 param, void *pInfo);
    void ScnMdl_G3DPROC_DRAW_OPA(u32 param, void *pInfo);
    void ScnMdl_G3DPROC_DRAW_XLU(u32 param, void *pInfo);

private:
    enum VisBufferFlag {
        VISBUFFER_DIRTY = (1 << 0),
        VISBUFFER_NOT_REFRESH_NEEDED = (1 << 1),
    };

    enum MatBufferOption {
        BUFOPTION_TEXOBJ = (1 << 0),
        BUFOPTION_TLUTOBJ = (1 << 1),
        BUFOPTION_TEXSRT = (1 << 2),
        BUFOPTION_MATCHAN = (1 << 3),
        BUFOPTION_GENMODE = (1 << 4),
        BUFOPTION_MATMISC = (1 << 5),
        BUFOPTION_VIS = (1 << 6),
        BUFOPTION_MATPIX = (1 << 7),
        BUFOPTION_MATTEVCOLOR = (1 << 8),
        BUFOPTION_MATINDMTXSCALE = (1 << 9),
        BUFOPTION_MATTEXCOORDGEN = (1 << 10),
        BUFOPTION_TEV = (1 << 11),
        BUFOPTION_VTXPOS = (1 << 12),
        BUFOPTION_VTXNRM = (1 << 13),
        BUFOPTION_VTXCLR = (1 << 14),

        BUFOPTION_RESTORE_VTX_ON_DETACH = (1 << 24),
    };

private:
    bool IsVisBufferDirty() const {
        return mFlagVisBuffer & VISBUFFER_DIRTY;
    }
    bool IsVisBufferRefreshNeeded() const {
        return !(mFlagVisBuffer & VISBUFFER_NOT_REFRESH_NEEDED);
    }
    void VisBufferDirty() {
        mFlagVisBuffer |= VISBUFFER_DIRTY;
    }

    bool IsMatBufferDirty(u32 idx, u32 option) const {
        return option & mpMatBufferDirtyFlag[idx];
    }
    void MatBufferDirty(u32 idx, u32 option) {
        mpMatBufferDirtyFlag[idx] |= option;
    }

private:
    AnmObjShp *mpAnmObjShp;
    u32 mFlagVisBuffer;
    u32 *mpMatBufferDirtyFlag;
    DrawResMdlReplacement mReplacement;
    u32 mReplacementFlag;

    NW4R_G3D_RTTI_DECL_DERIVED(ScnMdl, ScnMdlSimple);
};

}
}

#endif
