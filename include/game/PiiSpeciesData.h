#ifndef GAME_PIISPECIESDATA_H
#define GAME_PIISPECIESDATA_H

#include <revolution/types.h>
struct PiiSpeciesData {
    int GetSpecies() const;
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
