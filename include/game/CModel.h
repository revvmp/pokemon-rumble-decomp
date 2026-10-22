#ifndef GAME_CMODEL_H
#define GAME_CMODEL_H

#include <game/ModelAim.h>
#include <nw4r/g3d/res/g3d_resanmchr.h>
#include <game/ScnMdlSimpleData.h>
#include <functional>
#include <list>
#include <string>
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
    class CMotionProgram {
    public:
        enum Flag {
            FLAG_CREATED = 1 << 0,
            FLAG_LOOP_TEXTURE_ANIMATION = 1 << 2,
            FLAG_NOTIFY_CALLBACK = 1 << 3,
            FLAG_PENDING = 1 << 4,
            FLAG_PLAYING = 1 << 5,
            FLAG_LOOPED = 1 << 6,
            FLAG_FINISHED = 1 << 7,
            FLAG_STOPPED = 1 << 8,
            FLAG_PAUSED = 1 << 9
        };

        void Wait();
        u32 GetId() const { return mId; }
        bool IsStarted() const { return !(mFlags & FLAG_PENDING); }
        bool IsPlaying() const { return mFlags & (FLAG_PLAYING | FLAG_LOOPED); }
        bool IsPlayingNotYetLooped() const { return (mFlags & FLAG_PLAYING) != 0; }
        bool IsCompleted() const { return mFlags & (FLAG_FINISHED | FLAG_STOPPED); }
        bool IsCompletedOrIsLooped() const { return mFlags & (FLAG_LOOPED | FLAG_FINISHED | FLAG_STOPPED); }
        f32 GetCurrentFrame() const {
            if (mFlags & FLAG_PENDING) { return 0.0f; }
            if (mFlags & (FLAG_PLAYING | FLAG_LOOPED)) { return *mpCurrentFrame; }
            return mLength;
        }
        int GetLength() const { return mLength; }
        u32 GetLoopCount() const { return mFlags >> 16; }
        f32 GetSpeed() const { return mRate; }
        void Pause() { mFlags |= FLAG_PAUSED; }
        void Resume() { mFlags &= ~FLAG_PAUSED; }
        bool IsPausing() const { return (mFlags & FLAG_PAUSED) != 0; }
        const void* GetUserData() const { return mpUserData; }
        void SetUserData(const void* pUserData) { mpUserData = pUserData; }
        void SetLoopTextureAnimation(bool loop) {
            if (loop) {
                mFlags |= FLAG_LOOP_TEXTURE_ANIMATION;
            } else {
                mFlags &= ~FLAG_LOOP_TEXTURE_ANIMATION;
            }
        }

    private:
        u32 mFlags;
        u32 mId;
        f32 mStartFrame;
        f32 mRate;
        const void* mpUserData;
        std::string mName;
        u32 mReserved;
        f32* mpCurrentFrame;
        nw4r::g3d::ResAnmChr mResource;
        int mLength;
    };
    typedef std::tr1::function<void(CModel*, const std::tr1::shared_ptr<CMotionProgram>&)> MotionCallback;

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
    void Stop();
    std::tr1::shared_ptr<CMotionProgram> GetCurrentMotion();
    std::tr1::shared_ptr<CMotionProgram> FindMotion(u32 id);
    void UpdateFrame(f32 step);
    void SetRootScale(const std::tr1::shared_ptr<nw4r::g3d::AnmObjChrRootScale>& rRootScale);
    void AddResourceFile(std::tr1::shared_ptr<CFileData> file);
    bool SetAimNode(const char* pNodeName);
    void SetAimTarget(const nw4r::math::VEC3& rTarget, bool snap);
    void DisableAimLimit();
    void SetMotionCallback(const MotionCallback& rCallback);

    ModelAim& GetAim() { return mAim; }
    const nw4r::math::AABB& GetBounds() const { return mBounds; }
    nw4r::g3d::ScnObj* GetScnObj() const { return mModelData.GetModel(); }
    nw4r::g3d::ResMdl GetResMdl() const { return mModelData.GetModel()->GetResMdl(); }
    nw4r::g3d::ResFile GetResFile() const;

    ScnMdlSimpleData mModelData;
    std::tr1::shared_ptr<nw4r::g3d::AnmObjChrRootScale> mRootScale;
private:
    std::vector<std::tr1::shared_ptr<nw4r::g3d::AnmObj> > mAnimations;
    int mNextMotionID;
    TPartialAnm<true> mMainAnimation;
    std::tr1::shared_ptr<PartialAnmManager> mPartialAnimations;
    bool mAnimationChanged;
    MotionCallback mMotionCallback;
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

bool LoadScnMdlSimpleData(ScnMdlSimpleData* pData, const CModel::LoadParams& rParams, int mode, bool background);
void NotifyMotionCallback(const std::tr1::shared_ptr<CModel::CMotionProgram>& rMotion, bool notify, CModel* pModel);

#endif
