#ifndef GAME_CPII_H
#define GAME_CPII_H

#include <game/BModelObj.h>
#include <vector>

namespace nw4r {
namespace g3d {
class AnmObjTexSrtRes;
}
}

class CFileData;
class CHPBar;
class CMiniMapMarker;
class CPiiPersonalData;

class CPii : public BModelObj {
public:
    enum { TYPE = 1 << 6 };

    CPii(std::tr1::shared_ptr<CPiiPersonalData> personalData, u32 type);
    virtual ~CPii();

    virtual void Calc();
    virtual void CalcMtx();
    virtual void SetPosition(const nw4r::math::VEC3& rPosition);
    virtual nw4r::math::VEC3 GetPosition();
    virtual CPpuEngine* GetPpuEngine() const { return mPpuEngine.get(); }
    virtual CMiniMapMarker* GetEffect() const;
    virtual void SetEffectEnable(bool enable);
    virtual void OnEnterWorld();
    virtual void OnLeaveWorld();
    virtual void UpdateVisible();

    virtual void SetTransform(const nw4r::math::VEC3& rPosition,
                              const nw4r::math::VEC3& rVelocity);
    virtual void SetBaseMtx(const nw4r::math::MTX34& rMtx);

    virtual void Initialize(bool background);
    virtual const char* GetShadowName() const;
    virtual bool IsVisible() const;
    virtual void UpdateShadowMtx();

    std::tr1::shared_ptr<CHPBar> GetHPBar() const { return mHPBar; }
    std::tr1::shared_ptr<CPiiPersonalData> GetPersonalData() const {
        return mPersonalData;
    }
    void SetHPBar(const std::tr1::shared_ptr<CHPBar>& rHPBar);
    void SetMiniMapMarker(const std::tr1::shared_ptr<CMiniMapMarker>& rMarker);

private:
    u32 mPiiFlags;
    std::tr1::shared_ptr<CPiiPersonalData> mPersonalData;
    std::tr1::shared_ptr<nw4r::g3d::AnmObjTexSrtRes> mTextureAnimation;
    f32 mTextureFrame;
    f32 mTextureRate;
    std::tr1::shared_ptr<CPpuEngine> mPpuEngine;
    std::tr1::shared_ptr<CHPBar> mHPBar;
    std::tr1::weak_ptr<BWorldObj> mFollowTarget;
    int mFollowNode;
    std::tr1::shared_ptr<CMiniMapMarker> mMiniMapMarker;
    std::tr1::shared_ptr<nw4r::g3d::ScnObj> mShadow;
    std::vector<std::tr1::shared_ptr<CFileData> > mShadowFiles;
};

#endif
