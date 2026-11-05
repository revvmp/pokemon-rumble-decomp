#ifndef GAME_PIISPECIESDATA_H
#define GAME_PIISPECIESDATA_H

#include <revolution/types.h>
struct PiiSpeciesData;

const PiiSpeciesData* GetPiiSpeciesData(u16 monsno, int form);
bool IsValidPiiSpecies(u16 species);
bool IsValidPiiForm(u16 species, int form);

struct PiiSpeciesData {
    int GetSpecies() const;
    u32 GetDefaultSex() const;
    int GetFormNumber() const;
    int GetBoneType() const;
    int GetFlyHeight() const;
    bool IsFlying() const { return GetFlyHeight() != 0; }
    int GetASize() const;
    u32 GetGenderAndTypeFlags() const;
    bool IsMaleOnly() const { return static_cast<u8>(GetGenderAndTypeFlags()) == 0; }
    bool IsFemaleOnly() const { return static_cast<u8>(GetGenderAndTypeFlags()) == 254; }
    bool IsGenderless() const { return static_cast<u8>(GetGenderAndTypeFlags()) == 255; }
    f32 GetWalkSpeedCoeff() const;
    f32 GetWalkAnmRate() const;
    int GetWazaKouka(int type) const;
    int GetPrimaryType() const;
    int GetSecondaryType() const;
    f32 GetModelScale() const;
    f32 GetMotionRateScale() const;
    const char* GetModelName(int sex) const;
    int ScalesAsBoss() const;
    const wchar_t* GetName() const;
public:
    u16 mSpecies : 9;
    u8 mForm : 5;
    u8 : 2;
    u8 mBoneType : 4;
    u8 mFlyHeight : 2;
    u8 : 2;
    u8 mASize : 3;
    u8 : 5;
    u16 mScalePercent : 10;
    u16 : 6;
    u8 mMotionRatePercent : 7;
    u8 : 1;
    u8 mPrimaryType : 5;
    u8 : 3;
    u8 mSecondaryType : 5;
    u8 : 3;
    u8 mGenderAndTypeFlags;
    u8 mScalesAsBoss : 1;
    bool mDefaultMale : 1;
    u8 : 6;
    u8 : 8;
    const char* mModelNames[2];
    f32 mWalkSpeedCoeff;
    f32 mWalkAnmRate;
};

#endif
