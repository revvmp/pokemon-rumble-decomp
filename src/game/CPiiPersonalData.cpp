#include <algorithm>
#include <game/CPiiPersonalData.h>
#include <game/CPpuEngine.h>
#include <game/Message.h>
#include <game/Random.h>
#include <game/SystemMessage.h>
#include <revolution/os.h>

CPiiPersonalData::CPiiPersonalData(u32 id, u32 originalTrainerId, const PiiSpeciesData* pSpeciesData, u32 sex)
    : mFlags(0), mId(id), mSpeciesData(pSpeciesData),
      mSex(pSpeciesData->IsMaleOnly() ? 0 : pSpeciesData->IsFemaleOnly() ? 1 : pSpeciesData->IsGenderless() ? 2 : sex != 0),
      mLevel(0), mBonusMaxHP(0), mBonusAP(0), mBonusDP(0), mBonusSPD(0), mPrefix(0),
      mRandomSeed(Rand32()), mCreationTime(OSGetTime()), mOriginalTrainerId(originalTrainerId),
      mMaxHP(1), mAP(1), mDP(1), mSPD(1), mBP(1), mCappedLevel(0), mHP(1),
      mDamageTotal(0), mHealTotal(0), mPrice(0), mGroupNo(0) {
    while (IsRareColor()) {
        mRandomSeed = Rand32();
    }
    mMoveIds[0] = 0;
    mMoveIds[1] = 0;
}

CPiiPersonalData::~CPiiPersonalData() {}

void CPiiPersonalData::SetOriginalTrainerId(u32 id) {
    mOriginalTrainerId = id;
}

int CPiiPersonalData::GetDisplayKind() const {
    if (mHP <= 0) {
        return 1;
    }
    if (IsRareColor()) {
        return 3;
    }
    int kind = 0;
    if (mPrefix != 0) {
        kind = 2;
    }
    return kind;
}

void CPiiPersonalData::SetRandomSeed(u32 seed) {
    mRandomSeed = seed;
}

void CPiiPersonalData::RerollRandomSeed() {
    mRandomSeed = Rand32();
}

bool CPiiPersonalData::IsRareColor() const {
    return (((mRandomSeed & 0xFFFF) ^ (mRandomSeed >> 16)) ^ ((mOriginalTrainerId >> 16) ^ (mOriginalTrainerId & 0xFFFF))) < 8;
}

void CPiiPersonalData::SetRareColorRnd() {
    u32 id = mOriginalTrainerId;
    mRandomSeed = (id << 16) | (id >> 16);
}

void CPiiPersonalData::SetNormalColorRnd() {
    while (IsRareColor()) {
        mRandomSeed = Rand32();
    }
}

void CPiiPersonalData::SetCreationTime(const s64& rTime) {
    mCreationTime = rTime;
}

void CPiiPersonalData::SetWazaID(int index, int id) {
    mMoveIds[index] = id;
}

int CPiiPersonalData::GetWazaID(int index) const {
    return mMoveIds[index];
}

void CPiiPersonalData::SetHP(int hp) {
    if (hp < mHP) {
        mDamageTotal += hp - mHP;
    } else if (mHP < hp) {
        mHealTotal += hp - mHP;
    }
    mHP = std::max(0, std::min(hp, mMaxHP));
}

f32 CPiiPersonalData::GetHPRatio() {
    if (mHP == mMaxHP)
        return 1.0f;
    if (mHP == 0)
        return 0.0f;
    return f32(mHP) / f32(mMaxHP);
}

void CPiiPersonalData::ResetDamage() {
    mDamageTotal = 0;
}

void CPiiPersonalData::ResetHeal() {
    mHealTotal = 0;
}

void CPiiPersonalData::FillUpHP() {
    mHP = mMaxHP;
    mDamageTotal = 0;
    mHealTotal = 0;
}

void CPiiPersonalData::SetMaxHP(int value) {
    mMaxHP = value;
    mDamageTotal = 0;
    mHealTotal = 0;
    mHP = std::min(mHP, mMaxHP);
}

void CPiiPersonalData::SetBonusMaxHP(int value) {
    mBonusMaxHP = value;
}

void CPiiPersonalData::SetLevel(int value) {
    mLevel = value;
    mCappedLevel = value;
}

void CPiiPersonalData::SetCappedLevel(int value) {
    mCappedLevel = value;
}

void CPiiPersonalData::SetAP(int value) {
    mAP = value;
}

void CPiiPersonalData::SetBonusAP(int value) {
    mBonusAP = value;
}

void CPiiPersonalData::SetDP(int value) {
    mDP = value;
}

void CPiiPersonalData::SetBonusDP(int value) {
    mBonusDP = value;
}

void CPiiPersonalData::SetSPD(int value) {
    mSPD = value;
}

void CPiiPersonalData::SetBonusSPD(int value) {
    mBonusSPD = value;
}

void CPiiPersonalData::SetBP(int value) {
    mBP = value;
}

void CPiiPersonalData::SetGroupNo(int value) {
    mGroupNo = value;
}

void CPiiPersonalData::SetPrefix(int value) {
    mPrefix = value;
}

const wchar_t* CPiiPersonalData::GetPrefixText() const {
    return GetMessageText(mPrefix + 0x6001D, gEmptyText);
}

void CPiiPersonalData::SetBoss(bool boss) {
    if (boss) {
        mFlags |= FLAG_BOSS;
    } else {
        mFlags &= ~FLAG_BOSS;
    }
}

void CPiiPersonalData::SetFavorite(bool favorite) {
    if (favorite) {
        mFlags |= FLAG_FAVORITE;
    } else {
        mFlags &= ~FLAG_FAVORITE;
    }
}

void CPiiPersonalData::SetSelling(bool selling) {
    if (selling) {
        mFlags |= FLAG_SELLING;
    } else {
        mFlags &= ~FLAG_SELLING;
    }
}

void CPiiPersonalData::SetPrice(int value) {
    mPrice = value;
}

void CPiiPersonalData::Export(SD_PiiPersonalData::Data& rData) {
    rData.mSpecies = mSpeciesData->GetSpecies();
    rData.mForm = mSpeciesData->GetFormNumber();
    rData.mSex = mSex;
    rData.mMoveIds[0] = mMoveIds[0];
    rData.mMoveIds[1] = mMoveIds[1];
    rData.mLevel = mLevel;
    {
        int bonus = mBonusMaxHP;
        rData.mBonusLow[0] = bonus;
        rData.mBonusHigh[0] = bonus >> 16;
    }
    {
        int bonus = mBonusAP;
        rData.mBonusLow[1] = bonus;
        rData.mBonusHigh[1] = bonus >> 16;
    }
    {
        int bonus = mBonusDP;
        rData.mBonusLow[2] = bonus;
        rData.mBonusHigh[2] = bonus >> 16;
    }
    {
        int bonus = mBonusSPD;
        rData.mBonusLow[3] = bonus;
        rData.mBonusHigh[3] = bonus >> 16;
    }
    rData.mPrefix = mPrefix;
    rData.mFlagsLow = mFlags;
    rData.mRandomSeed = mRandomSeed;
    s64 creationTime = mCreationTime;
    rData.mCreationTimeLow = u32(creationTime);
    rData.mCreationTimeHigh = u32(creationTime >> 32);
    rData.mOriginalTrainerId = mOriginalTrainerId;
}

std::tr1::shared_ptr<CPiiPersonalData> CPiiPersonalData::Import(u32 id, const SD_PiiPersonalData::Data& rData) {
    u16 species = rData.mSpecies;
    if (!IsValidPiiSpecies(species)) {
        return std::tr1::shared_ptr<CPiiPersonalData>();
    }
    int form = rData.mForm;
    if (!IsValidPiiForm(species, form)) {
        return std::tr1::shared_ptr<CPiiPersonalData>();
    }
    const PiiSpeciesData* pSpeciesData = GetPiiSpeciesData(species, u8(form));
    u32 sex = rData.mSex;
    std::tr1::shared_ptr<CPiiPersonalData> data(new CPiiPersonalData(id, 0, pSpeciesData, sex));
    data->mMoveIds[0] = rData.mMoveIds[0];
    data->mMoveIds[1] = rData.mMoveIds[1];
    data->mLevel = rData.mLevel;
    data->mBonusMaxHP = (u32(rData.mBonusHigh[0]) << 16) | rData.mBonusLow[0];
    data->mBonusAP = (u32(rData.mBonusHigh[1]) << 16) | rData.mBonusLow[1];
    data->mBonusDP = (u32(rData.mBonusHigh[2]) << 16) | rData.mBonusLow[2];
    data->mBonusSPD = (u32(rData.mBonusHigh[3]) << 16) | rData.mBonusLow[3];
    data->mPrefix = rData.mPrefix;
    data->mFlags = rData.mFlagsLow | (data->mFlags & 0xFFFF0000);
    data->mCreationTime = rData.GetCreationTime();
    data->mRandomSeed = rData.mRandomSeed;
    data->mOriginalTrainerId = rData.mOriginalTrainerId;
    return data;
}

void CPiiPersonalData::ExecuteInitScript(int mode, const std::tr1::shared_ptr<NPpu::CEnvironment>& rEnv) {
    std::tr1::shared_ptr<CPpuEngine> engine(new CPpuEngine);
    NPpu::CValue self;
    self.SetInt(mId | 0x80000000);
    engine->SetThis(self);
    engine->Execute("CPiiPersonalData_init.pkc", mode, rEnv);
}
