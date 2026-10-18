#ifndef GAME_BWORLDLIGHTSET_H
#define GAME_BWORLDLIGHTSET_H

#include <game/BWorldObj.h>
#include <list>
#include <memory>
#include <nw4r/g3d/g3d_anmscn.h>
#include <string>

class CFileData;

class BWorldLightSet : public BWorldObj {
public:
    enum { TYPE = 1 << 4 };

    class CMotionProgram {
    public:
        enum Flag {
            FLAG_CREATED = 1 << 0,
            FLAG_QUEUED = 1 << 1,
            FLAG_PLAYING = 1 << 2,
            FLAG_LOOPED = 1 << 3,
            FLAG_STOPPED = 1 << 5,
            FLAG_PAUSED = 1 << 6,
            FLAG_STATE_MASK = 0x3E,
            LOOP_COUNT_INCREMENT = 1 << 16
        };

        void SetState(u32 state) { mFlags = (mFlags & ~FLAG_STATE_MASK) | state; }

        u32 mFlags;
        u32 mId;
        f32 mStartFrame;
        f32 mRate;
        u32 mReservedState;
        std::string mName;
        u32 mReserved;
        f32* mpCurrentFrame;
        nw4r::g3d::ResAnmScn mResource;
        std::tr1::shared_ptr<nw4r::g3d::AnmScnRes> mAnimation;
        int mLength;
    };

    typedef std::tr1::shared_ptr<CMotionProgram> MotionPtr;
    typedef std::list<MotionPtr> MotionList;

    explicit BWorldLightSet(u32 type);
    virtual ~BWorldLightSet();
    virtual void Calc();
    virtual void SetPosition(const nw4r::math::VEC3& rPosition);
    virtual void OnEnterWorld();
    virtual void OnLeaveWorld();

    bool Load(const char* pPath, bool background);
    MotionPtr GetCurrentMotion();
    MotionPtr FindMotion(u32 id);
    MotionPtr Play(MotionPtr& rMotion);
    MotionPtr Play(const char* pName, f32 frame, f32 rate);
    MotionPtr Request(MotionPtr& rMotion);
    MotionPtr Request(const char* pName, f32 frame, f32 rate);
    MotionPtr RequestNext(MotionPtr& rMotion);
    MotionPtr RequestNext(const char* pName, f32 frame, f32 rate);
    void Stop();
    void Pause(bool pause);
    int GetMotionLength(const char* pName);
    void UpdateFrame(f32 step);
    MotionPtr CreateMotion(MotionPtr& rMotion, f32 frame, f32 rate);
    MotionPtr CreateMotion(const char* pName, f32 frame, f32 rate);

private:
    bool Load(const std::tr1::shared_ptr<CFileData>& rFile);
    void ApplyFrame(MotionPtr& rMotion);

    u32 mReserved;
    u32 mNextMotionID;
    std::tr1::shared_ptr<CFileData> mAnmFile;
    MotionList mMotionPrograms;
    f32 mMotionFrame;
};

#endif
