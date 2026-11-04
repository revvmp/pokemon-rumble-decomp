#ifndef GAME_PIIMOTION_H
#define GAME_PIIMOTION_H

#include <revolution/types.h>

struct PiiMotionInfo {
    int mId;
    const char* mName;
    f32 mSpeed;
    const char* mRootMotionName;
    const char* mResourceName;
    bool mLoopsTextureAnimation;
    bool mHasTextureAnimation;
};

struct PiiMotionSet {
    int mBoneType;
    PiiMotionInfo mMotions[3];
};

extern const PiiMotionSet gPiiMotionSets[];
extern const PiiMotionInfo gSharedMotions[];

#endif
