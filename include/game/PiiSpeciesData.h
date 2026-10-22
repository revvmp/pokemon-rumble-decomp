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
    bool IsFlying() const;
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
    bool ScalesAsBoss() const;
    const wchar_t* GetName() const;
private:
    u32 mIdentityFlags;
    u32 mScaleAndTypeFlags;
    u32 mGenderAndTypeFlags;
    const char* mModelNames[2];
    f32 mWalkSpeedCoeff;
    f32 mWalkAnmRate;
};

#endif
