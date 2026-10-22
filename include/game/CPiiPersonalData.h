#ifndef GAME_CPIIPERSONALDATA_H
#define GAME_CPIIPERSONALDATA_H

#include <game/CPpuRegister.h>
#include <game/PiiSpeciesData.h>
#include <game/SaveData.h>
#include <memory>
#include <revolution/types.h>

class CPpuEnv;
class COyaInfo;
class CPiiPersonalData;
namespace NPpu {
class CEnvironment;
}

std::tr1::shared_ptr<CPiiPersonalData> CreatePiiPersonalData(const COyaInfo* pOyaInfo, const PiiSpeciesData* pSpeciesData, u32 sex);
std::tr1::shared_ptr<CPiiPersonalData> CreatePiiPersonalData(const COyaInfo* pOyaInfo, const SD_PiiPersonalData::Data& rData);

class CPiiPersonalData : public std::tr1::enable_shared_from_this<CPiiPersonalData> {
public:
    CPiiPersonalData(u32 id, u32 originalTrainerId, const PiiSpeciesData* pSpeciesData, u32 sex);
    ~CPiiPersonalData();

    enum Flag {
        FLAG_FAVORITE = 1 << 1,
        FLAG_IN_TEAM = 1 << 16,
        FLAG_IN_WII_REMOTE = 1 << 17,
        FLAG_SPEC_HIDDEN = 1 << 18,
        FLAG_NEWLY_DISCLOSED = 1 << 19,
        FLAG_SELLING = 1 << 20,
        FLAG_BOSS_HIDE_HP = 1 << 30,
        FLAG_BOSS = 1 << 31
    };

    const PiiSpeciesData* GetSpeciesData() const { return mSpeciesData; }
    const s64& GetCreationTime() const { return mCreationTime; }
    f32 GetHPRatio();
    int GetHP() const { return mHP; }
    int GetDamageTotal() const { return mDamageTotal; }
    int GetHealTotal() const { return mHealTotal; }
    void SetHP(int hp);
    void FillUpHP();
    void ResetDamage();
    void ResetHeal();
    int GetDisplayKind() const;
    const wchar_t* GetPrefixText() const;
    void SetSelling(bool selling);
    void SetCreationTime(const s64& rTime);
    void RerollRandomSeed();
    void Export(SD_PiiPersonalData::Data& rData);
    static std::tr1::shared_ptr<CPiiPersonalData> Import(u32 id, const SD_PiiPersonalData::Data& rData);
    void ExecuteInitScript(int mode, const std::tr1::shared_ptr<CPpuEnv>& rEnv);
    bool IsFainted() const { return mHP <= 0; }
    bool IsFavorite() const { return HasFlag(FLAG_FAVORITE); }
    bool IsSpecHidden() const { return HasFlag(FLAG_SPEC_HIDDEN); }
    bool IsSpecHidden() { return HasFlag(FLAG_SPEC_HIDDEN); }
    void SetSpecHidden(bool hidden) { SetFlag(FLAG_SPEC_HIDDEN, hidden); }
    void SetNewlyDisclosed(bool disclosed) { SetFlag(FLAG_NEWLY_DISCLOSED, disclosed); }
    void SetFavorite(bool favorite);
    int GetPrefix() const { return mPrefix; }
    int GetId() const { return mId; }
    u32 GetOriginalTrainerId() const { return mOriginalTrainerId; }
    int GetMaxHP() const { return mMaxHP; }
    void SetMaxHP(int value);
    int GetBonusMaxHP() const { return mBonusMaxHP; }
    void SetBonusMaxHP(int value);
    int GetSex() const { return mSex; }
    int GetLevel() const { return mLevel; }
    void SetLevel(int value);
    int GetCappedLevel() const { return mCappedLevel; }
    void SetCappedLevel(int value);
    int GetAP() const { return mAP; }
    void SetAP(int value);
    int GetDP() const { return mDP; }
    void SetDP(int value);
    int GetSPD() const { return mSPD; }
    void SetSPD(int value);
    int GetBonusAP() const { return mBonusAP; }
    void SetBonusAP(int value);
    int GetBonusDP() const { return mBonusDP; }
    void SetBonusDP(int value);
    int GetBonusSPD() const { return mBonusSPD; }
    void SetBonusSPD(int value);
    int GetBP() const { return mBP; }
    void SetBP(int value);
    int GetWazaID(int index) const;
    void SetWazaID(int index, int id);
    int GetPrice() const { return mPrice; }
    void SetPrice(int value);
    int GetGroupNo() const { return mGroupNo; }
    void SetGroupNo(int value);
    void SetPrefix(int value);
    bool IsBoss() const { return HasFlag(FLAG_BOSS); }
    void SetBoss(bool boss);
    bool IsBossHideHP() const { return HasFlag(FLAG_BOSS_HIDE_HP); }
    void SetBossHideHP(bool hidden) { SetFlag(FLAG_BOSS_HIDE_HP, hidden); }
    bool IsSelling() const { return HasFlag(FLAG_SELLING); }
    bool IsNewlyDisclosed() const { return HasFlag(FLAG_NEWLY_DISCLOSED); }
    CPpuRegister* GetRegister() { return &mRegister; }
    void SetOriginalTrainerId(u32 id);
    u32 GetRandomSeed() const { return mRandomSeed; }
    void SetRandomSeed(u32 seed);
    bool IsRareColor() const;
    void SetRareColorRnd();
    void SetNormalColorRnd();
    bool IsInWiiRemote() const { return HasFlag(FLAG_IN_WII_REMOTE); }
    void SetInTeam(bool inTeam) { SetFlag(FLAG_IN_TEAM, inTeam); }
    void SetInWiiRemote(bool inWiiRemote) { SetFlag(FLAG_IN_WII_REMOTE, inWiiRemote); }
private:
    bool HasFlag(u32 flag) const { return (mFlags & flag) != 0; }
    bool HasFlag(u32 flag) { return (mFlags & flag) != 0; }
    void SetFlag(u32 flag, bool enable) {
        if (enable) {
            AddFlag(flag);
        } else {
            RemoveFlag(flag);
        }
    }
    void AddFlag(u32 flag) { mFlags |= flag; }
    void RemoveFlag(u32 flag) { mFlags &= ~flag; }

    u32 mFlags;
    int mId;
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
