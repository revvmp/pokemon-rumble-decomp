#ifndef GAME_CPIIPERSONALDATA_H
#define GAME_CPIIPERSONALDATA_H

#include <game/CPpuRegister.h>
#include <game/PiiSpeciesData.h>
#include <memory>
#include <revolution/types.h>
class CPiiPersonalData : public std::tr1::enable_shared_from_this<CPiiPersonalData> {
public:
    CPiiPersonalData(u32 id, u32 originalTrainerId, const PiiSpeciesData* pSpeciesData, u32 sex);
    ~CPiiPersonalData();

    const PiiSpeciesData* GetSpeciesData() const { return mSpeciesData; }
private:
    u32 mFlags;
    u32 mId;
    const PiiSpeciesData* mSpeciesData;
    int mSex;
    int mMoveIds[2];
    int mLevel;
    int mBonusMaxHP;
    int mBonusAP;
    int mBonusDP;
    int mBonusSPD;
    int mPrefix;
    u32 mRandomSeed;
    s64 mCreationTime;
    u32 mOriginalTrainerId;
    CPpuRegister mRegister;
    int mMaxHP;
    int mAP;
    int mDP;
    int mSPD;
    int mBP;
    int mCappedLevel;
    int mHP;
    int mDamageTotal;
    int mHealTotal;
    int mPrice;
    int mGroupNo;
};

#endif
