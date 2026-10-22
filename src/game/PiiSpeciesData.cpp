#include <game/PiiSpeciesData.h>
#include <game/Message.h>

extern const PiiSpeciesData gPiiSpeciesData[560];
extern const u16 gSpeciesFormOffsets[508];

int GetTypeEfficacy(int attackType, int defendType);

namespace {
s8 sCombinedEfficacy[5][5] = {
    {0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0},
    {0, 0, 1, 2, 3},
    {0, 0, 2, 3, 4},
    {0, 0, 3, 4, 5},
};
}

bool IsValidPiiSpecies(u16 species) {
    int id = species;
    return id >= 1U && id <= 495U;
}

bool IsValidPiiForm(u16 species, int form) {
    if (!IsValidPiiSpecies(species)) {
        return false;
    }
    if (species == 201) {
        return static_cast<u32>(form) <= 27;
    }
    if (species == 351) {
        return static_cast<u32>(form) <= 3;
    }
    if (species == 386) {
        return static_cast<u32>(form) <= 3;
    }
    if (species == 412) {
        return static_cast<u32>(form) <= 2;
    }
    if (species == 413) {
        return static_cast<u32>(form) <= 2;
    }
    if (species == 421) {
        return static_cast<u32>(form) <= 1;
    }
    if (species == 422) {
        return static_cast<u32>(form) <= 1;
    }
    if (species == 423) {
        return static_cast<u32>(form) <= 1;
    }
    if (species == 492) {
        return static_cast<u32>(form) <= 1;
    }
    if (species == 479) {
        return static_cast<u32>(form) <= 5;
    }
    if (species == 487) {
        return static_cast<u32>(form) <= 1;
    }
    if (species == 493) {
        return static_cast<u32>(form) <= 17;
    }
    if (species == 494) {
        return static_cast<u32>(form) <= 1;
    }
    return form == 0;
}

const PiiSpeciesData* GetPiiSpeciesData(u16 species, int form) {
    return &gPiiSpeciesData[gSpeciesFormOffsets[species] + form];
}

int PiiSpeciesData::GetSpecies() const {
    return mSpecies;
}

int PiiSpeciesData::GetFormNumber() const {
    return mForm;
}

int PiiSpeciesData::GetBoneType() const {
    return mBoneType;
}

int PiiSpeciesData::GetFlyHeight() const {
    return mFlyHeight;
}

int PiiSpeciesData::GetASize() const {
    return mASize;
}

f32 PiiSpeciesData::GetModelScale() const {
    return mScalePercent == 100 ? 1.0f : 0.01f * mScalePercent;
}

f32 PiiSpeciesData::GetMotionRateScale() const {
    return 0.01f * mMotionRatePercent;
}

int PiiSpeciesData::GetPrimaryType() const {
    return mPrimaryType;
}

int PiiSpeciesData::GetSecondaryType() const {
    return mSecondaryType;
}

u32 PiiSpeciesData::GetGenderAndTypeFlags() const {
    return mGenderAndTypeFlags;
}

int PiiSpeciesData::ScalesAsBoss() const {
    return mScalesAsBoss;
}

u32 PiiSpeciesData::GetDefaultSex() const {
    switch (mGenderAndTypeFlags) {
    case 0:
        return 0;
    case 254:
        return 1;
    case 255:
        return 2;
    default:
        return mDefaultMale ^ 1;
    }
}

const char* PiiSpeciesData::GetModelName(int sex) const {
    return mModelNames[sex & 1];
}

f32 PiiSpeciesData::GetWalkSpeedCoeff() const {
    return mWalkSpeedCoeff;
}

f32 PiiSpeciesData::GetWalkAnmRate() const {
    return mWalkAnmRate;
}

const wchar_t* PiiSpeciesData::GetName() const {
    return GetMessageText(GetSpecies() + 0x30000 - 1, gEmptyText);
}

int PiiSpeciesData::GetWazaKouka(int type) const {
    int primary = GetPrimaryType();
    int secondary = GetSecondaryType();
    int second = primary == secondary ? 3 : GetTypeEfficacy(type, secondary);
    return sCombinedEfficacy[GetTypeEfficacy(type, primary)][second];
}

int GetPokemonVoiceId(u16 species, u16 form) {
    if (species == 492 && form == 1) {
        return 596;
    }
    return species + 101;
}
