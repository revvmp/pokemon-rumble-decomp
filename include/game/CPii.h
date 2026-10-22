#ifndef GAME_CPII_H
#define GAME_CPII_H

#include <game/BModelObj.h>
#include <game/CModel.h>
#include <game/PiiMotion.h>
#include <game/ScnMdlSimpleData.h>
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

namespace NPpu {
class CEnvironment;
}

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
    virtual void SetEffectEnable(bool enable) {
        if (enable) {
            mPiiFlags |= 0x4;
        } else {
            mPiiFlags &= ~0x4;
        }
    }
    virtual CMiniMapMarker* GetEffect() const {
        if ((mPiiFlags & 0x4) != 0) {
            return mMiniMapMarker.get();
        }
        return nullptr;
    }
    virtual void OnEnterWorld();
    virtual void OnLeaveWorld(BWorld* pWorld);
    virtual void UpdateVisible();

    virtual void SetTransform(const nw4r::math::VEC3& rPosition,
                              const nw4r::math::VEC3& rVelocity);
    virtual void SetBaseMtx(const nw4r::math::MTX34& rMtx);

    virtual void Initialize(bool background);
    virtual const char* GetShadowName() const;
    virtual bool IsVisible() const;
    virtual void UpdateShadowMtx();

    std::tr1::shared_ptr<CModel::CMotionProgram> PlayMotion(int motion, f32 frame, f32 rate);
    std::tr1::shared_ptr<CModel::CMotionProgram> QueueMotion(int motion, f32 frame, f32 rate);
    std::tr1::shared_ptr<CModel::CMotionProgram> AppendMotion(int motion, f32 frame, f32 rate);
    std::tr1::shared_ptr<CHPBar> GetHPBar() { return mHPBar; }
    std::tr1::shared_ptr<CPiiPersonalData> GetPersonalData() {
        return mPersonalData;
    }
    void Initialize();
    void FaceHeadForward();
    void FaceHeadToward(const nw4r::math::VEC3& rPosition);
    void FaceHeadToward(BWorldObj* pObject);
    void FaceHeadToward(BModelObj* pObject, const char* pNode);
    void SetHPBar(std::tr1::shared_ptr<CHPBar> hpBar);
    void SetMiniMapMarker(std::tr1::shared_ptr<CMiniMapMarker> marker);
    bool LoadScript(const char* pName, const std::tr1::shared_ptr<NPpu::CEnvironment>& rEnvironment) {
        return LoadScript(pName, 0, rEnvironment);
    }
    bool LoadScript(const char* pName, int mode,
                    const std::tr1::shared_ptr<NPpu::CEnvironment>& rEnvironment);
    std::tr1::shared_ptr<CModel::CMotionProgram> PlayMotionForDuration(int index, f32 frame, f32 duration);
    std::tr1::shared_ptr<CModel::CMotionProgram> QueueMotionForDuration(int index, f32 frame, f32 duration);
    std::tr1::shared_ptr<CModel::CMotionProgram> AppendMotionForDuration(int index, f32 frame, f32 duration);
    bool IsPlayingMotion(int id);
    void ClearFollowTarget();
    void LookAt(const nw4r::math::VEC3& rTarget);
    void SetFollowTarget(BWorldObj* pTarget);
    void SetFollowTarget(BModelObj* pTarget, const char* pNodeName);
    int GetMotionLengthByIndex(int index);

private:
    void LoadModel(bool background);
    void ApplySpindaSpots(std::tr1::shared_ptr<CModel> model);
    void ApplyTextureVariant(ScnMdlSimpleData& rData, const char* pSuffix);
    bool SetTextureAnimation(const char* pName);
    bool OnMotion(CModel* pModel, const std::tr1::shared_ptr<CModel::CMotionProgram>& rMotion);
    const PiiMotionInfo& GetMotionInfo(int index);
    void LoadMotionResource(const PiiMotionInfo& rInfo);
    void BlendFromWalk(const PiiMotionInfo& rIntro);
    std::tr1::shared_ptr<CModel::CMotionProgram> ApplyMotionInfo(
        const std::tr1::shared_ptr<CModel::CMotionProgram>& rProgram, const PiiMotionInfo& rInfo);
    u32 mPiiFlags;
    std::tr1::shared_ptr<CPiiPersonalData> mPersonalData;
    std::tr1::shared_ptr<nw4r::g3d::AnmObjTexSrtRes> mTextureAnimation;
    f32 mTextureFrame;
    f32 mTextureRate;
    std::tr1::shared_ptr<CPpuEngine> mPpuEngine;
    std::tr1::shared_ptr<CHPBar> mHPBar;
    std::tr1::weak_ptr<BWorldObj> mFollowTarget;
    u32 mFollowNode;
    std::tr1::shared_ptr<CMiniMapMarker> mMiniMapMarker;
    ScnMdlSimpleData mShadowModel;
};

#endif
