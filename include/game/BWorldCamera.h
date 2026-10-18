#ifndef GAME_BWORLDCAMERA_H
#define GAME_BWORLDCAMERA_H

#include <game/BWorldObj.h>
#include <list>
#include <memory>
#include <nw4r/g3d/g3d_camera.h>

class CFileData;

class BWorldCamera : public BWorldObj {
public:
    class CMotionProgram;

    enum { TYPE = TYPE_CAMERA };

    explicit BWorldCamera(u32 type);
    virtual ~BWorldCamera();

    virtual void Calc();
    virtual void SetPosition(const nw4r::math::VEC3& rPosition);
    virtual void OnEnterWorld();
    virtual void OnLeaveWorld();
    virtual void UpdateIndex(int prevIndex);

    nw4r::g3d::Camera GetG3dCamera();

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
