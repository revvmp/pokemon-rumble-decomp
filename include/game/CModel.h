#ifndef GAME_CMODEL_H
#define GAME_CMODEL_H

#include <game/ModelAim.h>
#include <nw4r/g3d/res/g3d_resanmchr.h>
#include <nw4r/g3d/g3d_anmchrext.h>
#include <nw4r/g3d/g3d_anmchr.h>
#include <nw4r/g3d/g3d_anmvis.h>
#include <nw4r/g3d/g3d_anmclr.h>
#include <nw4r/g3d/g3d_anmtexpat.h>
#include <nw4r/g3d/g3d_anmtexsrt.h>
#include <nw4r/g3d/g3d_anmshp.h>
#include <game/ScnMdlSimpleData.h>
#include <functional>
#include <list>
#include <map>
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
            FLAG_BIND_FAILED = 1 << 1,
            FLAG_LOOP_TEXTURE_ANIMATION = 1 << 2,
            FLAG_NOTIFY_CALLBACK = 1 << 3,
            FLAG_PENDING = 1 << 4,
            FLAG_PLAYING = 1 << 5,
            FLAG_LOOPED = 1 << 6,
            FLAG_FINISHED = 1 << 7,
            FLAG_STOPPED = 1 << 8,
            FLAG_PAUSED = 1 << 9
        };

        enum { PLAYBACK_STATE_MASK = FLAG_PENDING | FLAG_PLAYING | FLAG_LOOPED | FLAG_FINISHED | FLAG_STOPPED };
        void SetPlaybackState(u32 state) { mFlags = (mFlags & ~PLAYBACK_STATE_MASK) | state; }
        void Wait();
        u32 TestFlags(u32 mask) const;
#if defined(CMODEL_EMISSION_LAYOUT)
        __declspec(section ".text.model_update")
#endif
        bool IsOneShotTexture() const { return !(mFlags & FLAG_LOOP_TEXTURE_ANIMATION); }
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
        f32 GetPlaybackFrame() {
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
        void SetBindFailed(bool failed) {
            if (failed) {
                mFlags |= FLAG_BIND_FAILED;
            } else {
                mFlags &= ~FLAG_BIND_FAILED;
            }
        }
        void SetLoopTextureAnimation(bool loop) {
            if (loop) {
                mFlags |= FLAG_LOOP_TEXTURE_ANIMATION;
            } else {
                mFlags &= ~FLAG_LOOP_TEXTURE_ANIMATION;
            }
        }

        u32 mFlags;
        u32 mId;
        f32 mStartFrame;
        f32 mRate;
        const void* mpUserData;
        std::string mName;
        int mReserved;
        f32* mpCurrentFrame;
        nw4r::g3d::ResAnmChr mResource;
        int mLength;
    };
    typedef std::tr1::function<void(CModel*, const std::tr1::shared_ptr<CMotionProgram>&)> MotionCallback;
    friend void NotifyMotionCallback(const std::tr1::shared_ptr<CMotionProgram>& rMotion, bool notify, CModel* pModel);

    class BPartialAnm {
    public:
        std::tr1::shared_ptr<nw4r::g3d::AnmObjChrRes> mAnimObj;
        std::list<std::tr1::shared_ptr<CMotionProgram> > mMotions;
        f32 mFrame;
    public:
        BPartialAnm() : mFrame(0.0f) {}
        virtual ~BPartialAnm();
#if defined(CMODEL_EMISSION_LAYOUT)
        __declspec(section ".text.model_middle")
#endif
        virtual bool SetMotion(CModel* pModel, const std::tr1::shared_ptr<CMotionProgram>& rMotion) = 0;
#if defined(CMODEL_EMISSION_LAYOUT)
        __declspec(section ".text.model_middle")
#endif
        virtual bool QueueMotion(CModel* pModel, const std::tr1::shared_ptr<CMotionProgram>& rMotion) = 0;
#if defined(CMODEL_EMISSION_LAYOUT)
        __declspec(section ".text.model_middle")
#endif
        virtual bool AppendMotion(CModel* pModel, const std::tr1::shared_ptr<CMotionProgram>& rMotion) = 0;
        void Reset() {
            mAnimObj.reset();
            for (std::list<std::tr1::shared_ptr<CMotionProgram> >::iterator it = mMotions.begin(); it != mMotions.end(); ++it) {
                (*it)->SetPlaybackState(CMotionProgram::FLAG_STOPPED);
            }
            mMotions.clear();
        }
        bool CreateAnimObj(nw4r::g3d::ResMdl mdl);
        bool IsIdle() {
            return mMotions.size() == 0 || (mMotions.size() == 1 && (mMotions.front()->mFlags & CMotionProgram::FLAG_FINISHED));
        }
    };
    template<bool Main> class TPartialAnm : public BPartialAnm {
    public:
#if defined(CMODEL_THUNK_EMISSION_LAYOUT)
        virtual ~TPartialAnm();
#endif
#if defined(CMODEL_EMISSION_LAYOUT)
        __declspec(section ".text.model_middle")
#endif
        virtual bool SetMotion(CModel* pModel, const std::tr1::shared_ptr<CMotionProgram>& rMotion);
#if defined(CMODEL_EMISSION_LAYOUT)
        __declspec(section ".text.model_middle")
#endif
        virtual bool QueueMotion(CModel* pModel, const std::tr1::shared_ptr<CMotionProgram>& rMotion);
#if defined(CMODEL_EMISSION_LAYOUT)
        __declspec(section ".text.model_middle")
#endif
        virtual bool AppendMotion(CModel* pModel, const std::tr1::shared_ptr<CMotionProgram>& rMotion);
#if defined(CMODEL_EMISSION_LAYOUT)
        __declspec(section ".text.model_middle")
#endif
        bool Update(CModel* pModel, f32 step);
        bool IsReplaceable() { return Main ? IsIdle() : mMotions.size() == 0; }
    };
    class PartialAnmManager {
    public:
        std::tr1::shared_ptr<nw4r::g3d::AnmObjChrOverride> mAnimObj;
        std::map<u32, std::tr1::shared_ptr<TPartialAnm<false> > > mPartials;
    };

    struct LoadParams {
        const char** mpFileNames;
        bool mSimple;
        bool mExplicitOptions;
        u32 mOptions;
        int mMode;
        const char* mpModelName;
    };

    CModel();
    virtual ~CModel();

    bool Load(const LoadParams& rParams, bool background);
    int GetMotionLength(const char* pName) const;
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

    std::tr1::shared_ptr<CMotionProgram> CreateMotion(const char* pName, f32 frame, f32 rate, BPartialAnm** ppPartial);
    std::tr1::shared_ptr<CMotionProgram> CreateMotion(int index, f32 frame, f32 rate, BPartialAnm** ppPartial);
    std::tr1::shared_ptr<CMotionProgram> SetupMotion(std::tr1::shared_ptr<CMotionProgram>& rProgram, f32 frame, f32 rate, BPartialAnm** ppPartial);
    BPartialAnm* GetPartialAnm(u32 nodeId);
    void Unload();
    void UpdateAnimations();
    nw4r::g3d::ResAnmVis FindVis(const char* pName) const;
    nw4r::g3d::ResAnmVis FindVis(int index) const;
    static bool LoadBounds(nw4r::math::AABB* pBounds, const nw4r::g3d::ResMdl& rMdl);
    template <class Traits, class Key> std::tr1::shared_ptr<typename Traits::Obj> CreateAnimObj(Key key, f32 frame);

    ModelAim& GetAim() { return mAim; }
    const nw4r::math::AABB& GetBounds() const { return mBounds; }
    nw4r::g3d::ScnObj* GetScnObj() const { return mModelData.GetModel(); }
    nw4r::g3d::ResMdl GetResMdl() const { return mModelData.GetModel()->GetResMdl(); }
    nw4r::g3d::ResFile GetResFile() const;

    ScnMdlSimpleData mModelData;
    std::tr1::shared_ptr<nw4r::g3d::AnmObjChrRootScale> mRootScale;
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

};

bool LoadScnMdlSimpleData(ScnMdlSimpleData* pData, const CModel::LoadParams& rParams, u32* pOutOptions, bool background);
void NotifyMotionCallback(const std::tr1::shared_ptr<CModel::CMotionProgram>& rMotion, bool notify, CModel* pModel);

#endif
