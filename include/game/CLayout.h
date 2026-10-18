#ifndef GAME_CLAYOUT_H
#define GAME_CLAYOUT_H

#include <memory>
#include <revolution/types.h>

namespace nw4r {
namespace lyt {
class AnimTransform;
class Pane;
}
}

class CLayout {
public:
    class CMotionProgram {
    public:
        bool IsStarted() const { return mState != 0; }
        bool IsFinished() const { return mState == 3 || mState == 2; }

    private:
        f32 mFrame;
        f32 mRate;
        nw4r::lyt::AnimTransform* mpAnimTransform;
        nw4r::lyt::Pane* mpTarget;
        u16 mLastFrame;
        int mState;
    };

    void SetVisible(bool visible);
    std::tr1::shared_ptr<CMotionProgram> CreateMotion(const char* pName);
};

#endif
