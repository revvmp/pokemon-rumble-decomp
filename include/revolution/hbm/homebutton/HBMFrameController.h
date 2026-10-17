#ifndef HBM_FRAME_CONTROLLER_H
#define HBM_FRAME_CONTROLLER_H

#include "revolution/types.h"

namespace homebutton {

class FrameController {
public:
    enum eState {
        eState_Stopped,
        eState_Playing
    };

    enum eAnmType {
        eAnmType_Forward,
        eAnmType_Backward,
        eAnmType_Wrap,
        eAnmType_Oscillate,

        eAnmType_Max
    };

    FrameController() {}
    virtual ~FrameController() {}

    virtual void calc();

    void init(int anmType, f32 maxFrame, f32 minFrame, f32 delta);
    void initFrame();

    void start() {
        initFrame();
        restart();
    }

    void restart() { mState = eState_Playing; }
    void stop() { mState = eState_Stopped; }

    f32 getMaxFrame() const { return mMaxFrame; }
    f32 getCurrentFrame() const { return mCurFrame; }
    f32 getLastFrame() const { return mMaxFrame - 1.0f; }
    bool isPlaying() const { return mState == eState_Playing; }

    void setAnmType(int anmType) { mAnmType = anmType; }

protected:
    f32 mMaxFrame;
    f32 mMinFrame;
    f32 mCurFrame;
    f32 mFrameDelta;
    int mState;
    int mAnmType;
    bool mAltFlag;
};

}

#endif
