#ifndef NW4R_G3D_SCN_ROOT_H
#define NW4R_G3D_SCN_ROOT_H
#include <nw4r/types_nw4r.h>

#include <nw4r/g3d/g3d_camera.h>
#include <nw4r/g3d/g3d_fog.h>
#include <nw4r/g3d/g3d_light.h>
#include <nw4r/g3d/g3d_scnobj.h>
#include <nw4r/g3d/g3d_state.h>

namespace nw4r {
namespace g3d {

class AnmScn;

class ScnRoot : public ScnGroup {
public:
    enum ScnRootFlag {
        SCNROOTFLAG_FORCE_RESMDLDRAWMODE = (1 << 0)
    };

public:
    static ScnRoot *
    Construct(MEMAllocator *pAllocator, u32 *pSize, u32 maxChildren, u32 maxScnObj, u32 numLightObj, u32 numLightSet);

    static ScnRoot *Construct(MEMAllocator *pAllocator, u32 *pSize, u32 maxChildren, u32 maxScnObj) {
        return Construct(pAllocator, pSize, maxChildren, maxScnObj, G3DState::NUM_LIGHT, G3DState::NUM_LIGHT_SET);
    }

    ScnRoot(
        MEMAllocator *pAllocator, IScnObjGather *pGather, ScnObj **ppChildrenBuf, u32 maxChildren, u32 numLight,
        u32 numLightSet, LightObj *pLightObjBuf, AmbLightObj *pAmbObjBuf, LightSetData *pLightSetBuf
    );

    virtual void G3dProc(u32 task, u32 param, void *pInfo);
    virtual ~ScnRoot();

    Camera GetCamera(int idx);
    Camera GetCurrentCamera();
    void SetCurrentCamera(int idx);

    Fog GetFog(int idx);
    LightSet GetLightSet(int idx);

    void UpdateFrame();
    void SetGlbSettings();

    void CalcAnmScn();
    void CalcWorld();
    void CalcMaterial();
    void CalcVtx();
    void CalcView();

    void GatherDrawScnObj();
    void ZSort();
    void DrawOpa();
    void DrawXlu();
    void ForceDrawMode(bool force, ResMdlDrawMode mode);

    u32 TestScnRootFlag(ScnRootFlag flag) const {
        return (mScnRootFlags & flag) != 0;
    }
    void SetScnRootFlag(ScnRootFlag flag, u32 on) {
        if (on) {
            mScnRootFlags |= flag;
        } else {
            mScnRootFlags &= ~flag;
        }
    }

    int GetCurrentCameraID() const {
        return mCurrentCameraID;
    }

    LightSetting &GetLightSetting() {
        return mLightSetting;
    }

private:
    IScnObjGather *mpCollection;
    ResMdlDrawMode mDrawMode;
    u32 mScnRootFlags;
    u8 mCurrentCameraID;
    u8 PADDING_0xF5;
    u8 PADDING_0xF6;
    u8 PADDING_0xF7;
    CameraData mCamera[G3DState::NUM_CAMERA];
    FogData mFog[G3DState::NUM_FOG];
    LightSetting mLightSetting;
    AnmScn *mpAnmScn;

    NW4R_G3D_RTTI_DECL_DERIVED(ScnRoot, ScnGroup);
};

class ScnObjGather : public IScnObjGather {
public:
    ScnObjGather(ScnObj **ppBufOpa, ScnObj **ppBufXlu, u32 capacity);

    virtual ~ScnObjGather() {}

    virtual CullingStatus Add(ScnObj *pObj, bool opa, bool xlu);
    virtual void Clear() {
        mNumScnObjOpa = mNumScnObjXlu = 0;
    }

    virtual void ZSort();

    virtual void Sort();
    virtual void Sort(LessThanFunc pOpaFunc,
                      LessThanFunc pXluFunc);

    virtual void DrawOpa(ResMdlDrawMode *pForceMode);
    virtual void DrawXlu(ResMdlDrawMode *pForceMode);

    virtual CheckStatus CheckScnObj(ScnObj *);

protected:
    ScnObj **mpArrayOpa;
    ScnObj **mpArrayXlu;
    u32 mSizeScnObj;
    u32 mNumScnObjOpa;
    u32 mNumScnObjXlu;
};

}
}

#endif
