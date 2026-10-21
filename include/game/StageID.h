#ifndef GAME_STAGEID_H
#define GAME_STAGEID_H

#include <revolution/types.h>

struct StageID {
    union {
        struct {
            s32 mKind : 8;
            s32 mRank : 8;
            s32 mWorld : 8;
            s32 mReserved : 8;
        };
        s32 mValue;
    };
};

struct StageRankInfo {
    const char* mName;
    int mMessageID;
};

struct StageWorldInfo {
    int mIndex;
    const char* mName;
    int mMessageID;
};

extern const int gStageKindMessageIDs[];
extern const StageRankInfo gStageRanks[];
extern const StageWorldInfo gStageWorlds[];

#endif
