#ifndef GAME_LOCALE_H
#define GAME_LOCALE_H

#include <revolution/types.h>

enum Region {
    REGION_JPN,
    REGION_USA,
    REGION_EUR,
    REGION_AUS,
    REGION_BRA,
    REGION_TWN,
    REGION_KOR,
    REGION_CHN,
    REGION_HKG,
    REGION_ASI,
    REGION_LTN,
    REGION_SAF,
    REGION_ALL,
    REGION_UNKNOWN
};

const char* GetRegionName(Region region);
Region GetConsoleRegion();
Region GetGameRegion();
int GetConsoleLang();
int GetGameLang();
const char* GetGameLocale();
const char* GetConsoleLocale();

#endif
