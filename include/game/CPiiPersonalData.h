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

    enum Flag {
        FLAG_FAVORITE = 1 << 1,
        FLAG_IN_WII_REMOTE = 1 << 17,
        FLAG_SPEC_HIDDEN = 1 << 18,
        FLAG_NEWLY_DISCLOSED = 1 << 19,
        FLAG_SELLING = 1 << 20,
        FLAG_BOSS_HIDE_HP = 1 << 30,
        FLAG_BOSS = 1 << 31
    };

    const PiiSpeciesData* GetSpeciesData() const { return mSpeciesData; }
    const s64& GetCreationTime() const { return mCreationTime; }
    int GetHP() const { return mHP; }
    bool IsFainted() const { return mHP <= 0; }
    bool IsFavorite() const { return HasFlag(FLAG_FAVORITE); }
    bool IsSpecHidden() const { return HasFlag(FLAG_SPEC_HIDDEN); }
    void SetSpecHidden(bool hidden) { SetFlag(FLAG_SPEC_HIDDEN, hidden); }
    void SetNewlyDisclosed(bool disclosed) { SetFlag(FLAG_NEWLY_DISCLOSED, disclosed); }
    void SetFavorite(bool favorite);
    int GetPrefix() const { return mPrefix; }
    u32 GetOriginalTrainerId() const { return mOriginalTrainerId; }
private:
    bool HasFlag(u32 flag) const { return (mFlags & flag) != 0; }
    void SetFlag(u32 flag, bool enable) {
        if (enable) {
            mFlags |= flag;
        } else {
            mFlags &= ~flag;
        }
    }

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
