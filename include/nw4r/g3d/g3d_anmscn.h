#ifndef NW4R_G3D_ANM_SCN_H
#define NW4R_G3D_ANM_SCN_H
#include <nw4r/types_nw4r.h>

#include <nw4r/g3d/g3d_anmobj.h>
#include <nw4r/g3d/g3d_camera.h>
#include <nw4r/g3d/g3d_fog.h>
#include <nw4r/g3d/g3d_light.h>
#include <nw4r/g3d/g3d_obj.h>
#include <nw4r/g3d/res/g3d_resanmscn.h>

namespace nw4r {
namespace g3d {

class AnmScnRes;

class AnmScn : public G3dObj {
public:
    explicit AnmScn(MEMAllocator *pAllocator);
    virtual ~AnmScn();

    virtual void SetFrame(f32 frame) = 0;
    virtual f32 GetFrame() const = 0;

    virtual void SetUpdateRate(f32 rate) = 0;
    virtual f32 GetUpdateRate() const = 0;
    virtual void UpdateFrame() = 0;

    virtual AnmScnRes *Attach(int idx, AnmScnRes *pRes);
    virtual AnmScnRes *Detach(int idx);

    virtual u32 GetNumLightSet() const = 0;
    virtual u32 GetNumAmbLight() const = 0;
    virtual u32 GetNumDiffuseLight() const = 0;
    virtual u32 GetNumSpecularLight() const = 0;
    virtual u32 GetNumFog() const = 0;
    virtual u32 GetNumCamera() const = 0;

    virtual u32 GetLightSetMaxRefNumber() const = 0;
    virtual u32 GetAmbLightMaxRefNumber() const = 0;
    virtual u32 GetDiffuseLightMaxRefNumber() const = 0;
    virtual u32 GetFogMaxRefNumber() const = 0;
    virtual u32 GetCameraMaxRefNumber() const = 0;

    virtual bool GetLightSet(LightSet set, u32 refNumber) = 0;
    virtual ut::Color GetAmbLightColor(u32 refNumber) = 0;
    virtual void GetLight(LightObj *pDiff, LightObj *pSpec,
                          u32 refNumber) = 0;
    virtual void GetFog(Fog fog, u32 refNumber) = 0;
    virtual bool GetCamera(Camera camera, u32 refNumber) = 0;

    virtual u32 GetSpecularLightID(u32 refNumber) const = 0;
    virtual bool HasSpecularLight(u32 refNumber) const = 0;

    virtual AmbLightAnmResult *GetAmbLightResult(AmbLightAnmResult *pResult,
                                                 u32 refNumber) = 0;
    virtual LightAnmResult *GetLightResult(LightAnmResult *pResult,
                                           u32 refNumber) = 0;
    virtual FogAnmResult *GetFogResult(FogAnmResult *pResult,
                                       u32 refNumber) = 0;
    virtual CameraAnmResult *GetCameraResult(CameraAnmResult *pResult,
                                             u32 refNumber) = 0;

    void GetLightSetting(LightSetting *pSetting);

private:
    NW4R_G3D_RTTI_DECL_DERIVED(AnmScn, G3dObj);
};

class AnmScnRes : public AnmScn, protected FrameCtrl {
public:
    enum AnmScnResFlag {
        FLAG_CACHE = (1 << 0)
    };

public:
    static AnmScnRes *Construct(MEMAllocator *pAllocator, u32 *pSize, ResAnmScn scn, bool cache);

    AnmScnRes(
        MEMAllocator *pAllocator, ResAnmScn scn, AmbLightAnmResult *pAmbLightBuf, LightAnmResult *pLightBuf,
        FogAnmResult *pFogBuf, CameraAnmResult *pCameraBuf
    );

    virtual void G3dProc(u32 task, u32 param, void *pInfo);
    virtual ~AnmScnRes();

    virtual void SetFrame(f32 frame);
    virtual f32 GetFrame() const;

    virtual void SetUpdateRate(f32 rate);
    virtual f32 GetUpdateRate() const;
    virtual void UpdateFrame();

    virtual u32 GetNumLightSet() const;
    virtual u32 GetNumAmbLight() const;
    virtual u32 GetNumDiffuseLight() const;
    virtual u32 GetNumSpecularLight() const;
    virtual u32 GetNumFog() const;
    virtual u32 GetNumCamera() const;

    virtual u32 GetLightSetMaxRefNumber() const {
        return mResAnmScn.GetResLightSetMaxRefNumber();
    }
    virtual u32 GetAmbLightMaxRefNumber() const;
    virtual u32 GetDiffuseLightMaxRefNumber() const;
    virtual u32 GetFogMaxRefNumber() const;
    virtual u32 GetCameraMaxRefNumber() const;

    virtual bool GetLightSet(LightSet set, u32 refNumber);
    virtual ut::Color GetAmbLightColor(u32 refNumber);
    virtual void GetLight(LightObj *pDiff, LightObj *pSpec, u32 refNumber);
    virtual void GetFog(Fog fog, u32 refNumber);
    virtual bool GetCamera(Camera camera, u32 refNumber);

    virtual u32 GetSpecularLightID(u32 refNumber) const;
    virtual bool HasSpecularLight(u32 refNumber) const;

    virtual AmbLightAnmResult *GetAmbLightResult(AmbLightAnmResult *pResult, u32 refNumber);
    virtual LightAnmResult *GetLightResult(LightAnmResult *pResult, u32 refNumber);
    virtual FogAnmResult *GetFogResult(FogAnmResult *pResult, u32 refNumber);
    virtual CameraAnmResult *GetCameraResult(CameraAnmResult *pResult, u32 refNumber);

    void UpdateCache();

private:
    ResAnmScn mResAnmScn;
    u32 mFlags;
    AmbLightAnmResult *mpAmbLightCache;
    LightAnmResult *mpLightCache;
    FogAnmResult *mpFogCache;
    CameraAnmResult *mpCameraCache;

    NW4R_G3D_RTTI_DECL_DERIVED(AnmScnRes, AnmScn);
};

}
}

#endif
