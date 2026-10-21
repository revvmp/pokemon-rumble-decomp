#ifndef GAME_PIISPECIESDATA_H
#define GAME_PIISPECIESDATA_H

#include <revolution/types.h>
struct PiiSpeciesData;

const PiiSpeciesData* GetPiiSpeciesData(u16 monsno, int form);

struct PiiSpeciesData {
    int GetSpecies() const;
    int GetFormNumber() const;
    int GetBoneType() const;
    int GetFlyHeight() const;
    int GetASize() const;
    u32 GetGenderAndTypeFlags() const;
    f32 GetWalkSpeedCoeff() const;
    f32 GetWalkAnmRate() const;
    int GetWazaKouka(int type) const;
    int GetPrimaryType() const;
    int GetSecondaryType() const;
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
