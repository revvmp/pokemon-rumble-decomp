#ifndef GAME_BWORLDCAMERA_H
#define GAME_BWORLDCAMERA_H

#include <game/BWorldObj.h>
#include <list>
#include <string>
#include <nw4r/g3d/res/g3d_resanmscn.h>
#include <memory>
#include <nw4r/g3d/g3d_camera.h>

class CFileData;

class BWorldCamera : public BWorldObj {
public:
    class CMotionProgram {
    public:
        u32 GetId() const { return mId; }
        int GetLength() const { return mLength; }
        u32 GetLoopCount() const { return mFlags >> 16; }
        f32 GetSpeed() const { return mRate; }
        bool IsStarted() const { return !(mFlags & 2); }
        bool IsPlaying() const { return (mFlags & 0xC) != 0; }
        bool IsPlayingNotYetLooped() const { return (mFlags & 4) != 0; }
        bool IsCompleted() const { return (mFlags & 0x30) != 0; }
        bool IsCompletedOrLooped() const { return (mFlags & 0x38) != 0; }
        f32 GetFrame() const {
            if (mFlags & 2) { return 0.0f; }
            if (mFlags & 0xC) { return *mpCurrentFrame; }
            return mLength;
        }
        void Pause(bool pause) {
            if (pause) { mFlags |= 0x40; }
            else { mFlags &= ~0x40; }
        }
        bool IsPausing() const { return (mFlags & 0x40) != 0; }
        void SetIgnoreAimMode(bool ignore) {
            if (ignore) { mFlags |= 0x80; }
            else { mFlags &= ~0x80; }
        }
        u32 mFlags;
        u32 mId;
        f32 mStartFrame;
        f32 mRate;
        u32 mUserData;
        std::string mName;
        u32 mReserved;
        f32* mpCurrentFrame;
        nw4r::g3d::ResAnmScn mResource;
        nw4r::g3d::ResAnmCamera mCamera;
        int mLength;
    };
    typedef std::tr1::shared_ptr<CMotionProgram> MotionPtr;
    MotionPtr Play(const char* pName, f32 start, f32 rate);
    MotionPtr Request(const char* pName, f32 start, f32 rate);
    MotionPtr RequestNext(const char* pName, f32 start, f32 rate);
    MotionPtr FindMotion(u32 id);
    MotionPtr GetCurrentMotion();

    enum { TYPE = TYPE_CAMERA };

    explicit BWorldCamera(u32 type);
    virtual ~BWorldCamera();

    virtual void Calc();
    virtual void SetPosition(const nw4r::math::VEC3& rPosition);
    virtual void OnEnterWorld();
    virtual void OnLeaveWorld();
    virtual void UpdateIndex(int prevIndex);

    nw4r::g3d::Camera GetG3dCamera();

    void Pause(bool pause);
    void Stop();
    int GetMotionLength(const char* pName);

    int GetIndex() const { return mIndex; }
    void SetIndex(int index) { mIndex = index; }

private:
    int mIndex;
    u32 mNextMotionID;
    std::tr1::shared_ptr<CFileData> mAnmFile;
    std::list<std::tr1::shared_ptr<CMotionProgram> > mMotionPrograms;
    f32 mMotionFrame;
};

#endif
