#ifndef GAME_CMODEL_H
#define GAME_CMODEL_H

#include <game/ModelAim.h>
#include <game/ScnMdlSimpleData.h>
#include <functional>
#include <list>
#include <nw4r/g3d/g3d_anmobj.h>
#include <nw4r/g3d/g3d_scnmdlsmpl.h>
#include <memory>
#include <nw4r/g3d/g3d_calcworld.h>
#include <nw4r/g3d/g3d_scnobj.h>
#include <nw4r/math/math_types.h>
#include <vector>

namespace nw4r {
namespace g3d {
class AnmObjChrRootScale;
}
}

class CFileData;

class CModel : private nw4r::g3d::IScnObjCallback, private nw4r::g3d::ICalcWorldCallback {
public:
    class CMotionProgram;

    class BPartialAnm {
    protected:
        std::tr1::shared_ptr<CMotionProgram> mCurrentMotion;
        std::list<std::tr1::shared_ptr<CMotionProgram> > mMotions;
        f32 mFrame;
    public:
        BPartialAnm() : mFrame(0.0f) {}
        virtual ~BPartialAnm();
        virtual bool SetMotion(CModel* pModel, const std::tr1::shared_ptr<CMotionProgram>& rMotion, f32 step) = 0;
        virtual bool QueueMotion(CModel* pModel, const std::tr1::shared_ptr<CMotionProgram>& rMotion, f32 step) = 0;
        virtual bool AppendMotion(CModel* pModel, const std::tr1::shared_ptr<CMotionProgram>& rMotion, f32 step) = 0;
    };
    template<bool Main> class TPartialAnm : public BPartialAnm {
    public:
        virtual ~TPartialAnm();
        virtual bool SetMotion(CModel* pModel, const std::tr1::shared_ptr<CMotionProgram>& rMotion, f32 step);
        virtual bool QueueMotion(CModel* pModel, const std::tr1::shared_ptr<CMotionProgram>& rMotion, f32 step);
        virtual bool AppendMotion(CModel* pModel, const std::tr1::shared_ptr<CMotionProgram>& rMotion, f32 step);
    };
    class PartialAnmManager;

    struct LoadParams {
        const char** mpFileNames;
        bool mFlag;
        u32 mOptions;
        int mMode;
        const char* mpModelName;
    };

    CModel();
    virtual ~CModel();

    bool Load(const LoadParams& rParams, bool background);
    int GetMotionLength(const char* pName);
    std::tr1::shared_ptr<CMotionProgram> Play(const char* pName, f32 frame, f32 rate);
    std::tr1::shared_ptr<CMotionProgram> Play(int index, f32 frame, f32 rate);
    std::tr1::shared_ptr<CMotionProgram> PlayLoop(const char* pName, f32 frame, f32 rate);
    std::tr1::shared_ptr<CMotionProgram> PlayOnce(const char* pName, f32 frame, f32 rate);
    void Pause(bool pause);
    void UpdateFrame(f32 step);
    void SetRootScale(const std::tr1::shared_ptr<nw4r::g3d::AnmObjChrRootScale>& rRootScale);

    const nw4r::math::AABB& GetBounds() const { return mBounds; }
    nw4r::g3d::ScnObj* GetScnObj() const { return mModelData.GetModel(); }

    ScnMdlSimpleData mModelData;
    std::tr1::shared_ptr<nw4r::g3d::AnmObjChrRootScale> mRootScale;
private:
    std::vector<std::tr1::shared_ptr<nw4r::g3d::AnmObj> > mAnimations;
    int mNextMotionID;
    TPartialAnm<true> mMainAnimation;
    std::tr1::shared_ptr<PartialAnmManager> mPartialAnimations;
    bool mAnimationChanged;
    std::tr1::function<void(CModel*, const std::tr1::shared_ptr<CMotionProgram>&)> mMotionCallback;
    bool mAnimationControlsAim;
    bool mWorldAimTarget;
    ModelAim mAim;
    nw4r::math::VEC3 mAimTarget;
    bool mAimTargetPending;
    nw4r::math::AABB mBounds;
    virtual void ExecCallback_CALC_WORLD(nw4r::g3d::ScnObj::Timing timing, nw4r::g3d::ScnObj* pObject, u32 param, void* pInfo);
    virtual void ExecCallback_CALC_MAT(nw4r::g3d::ScnObj::Timing timing, nw4r::g3d::ScnObj* pObject, u32 param, void* pInfo);
    virtual void ExecCallback_CALC_VIEW(nw4r::g3d::ScnObj::Timing timing, nw4r::g3d::ScnObj* pObject, u32 param, void* pInfo);
    virtual void ExecCallback_DRAW_OPA(nw4r::g3d::ScnObj::Timing timing, nw4r::g3d::ScnObj* pObject, u32 param, void* pInfo);
    virtual void ExecCallback_DRAW_XLU(nw4r::g3d::ScnObj::Timing timing, nw4r::g3d::ScnObj* pObject, u32 param, void* pInfo);
    virtual void ExecCallbackA(nw4r::g3d::ChrAnmResult* pResult, nw4r::g3d::ResMdl mdl, nw4r::g3d::FuncObjCalcWorld* pFuncObj);
    virtual void ExecCallbackB(nw4r::g3d::WorldMtxManip* pManip, nw4r::g3d::ResMdl mdl, nw4r::g3d::FuncObjCalcWorld* pFuncObj);
    virtual void ExecCallbackC(nw4r::math::MTX34* pMtxArray, nw4r::g3d::ResMdl mdl, nw4r::g3d::FuncObjCalcWorld* pFuncObj);

};

#endif
