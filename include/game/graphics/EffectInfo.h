#ifndef GAME_EFFECTINFO_H
#define GAME_EFFECTINFO_H

struct EffectInfo {
    int mId;
    const char* mName;
    const char* mNodeName;
    const char* mTargetNodeName;
    int mType;
    bool mUseCaptureGroup;
};

extern const EffectInfo gEffectInfos[696];

#endif
