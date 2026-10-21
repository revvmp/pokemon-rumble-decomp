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
    u32 mMoveIds[2];
    f32 mModelScale;
    f32 mShadowScale;
};

#endif
