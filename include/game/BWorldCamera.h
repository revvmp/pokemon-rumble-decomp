#ifndef GAME_BWORLDCAMERA_H
#define GAME_BWORLDCAMERA_H

#include <game/BWorldObj.h>
#include <nw4r/g3d/g3d_camera.h>

class BWorldCamera : public BWorldObj {
public:
    enum { TYPE = TYPE_CAMERA };

    virtual void UpdateIndex(int prevIndex);

    nw4r::g3d::Camera GetG3dCamera();

    int GetIndex() const { return mIndex; }
    void SetIndex(int index) { mIndex = index; }

private:
    int mIndex;
};

#endif
