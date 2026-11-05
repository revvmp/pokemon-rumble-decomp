#include <game/text/Locale.h>
#include <revolution/os.h>
#include <revolution/os/OSFont.h>
#include <revolution/sc.h>
#include <revolution/vi.h>

static const char* sRegionNames[] = {
    "JPN", "USA", "EUR", "AUS", "BRA", "TWN", "KOR", "CHN", "HKG", "ASI", "LTN", "SAF", "ALL", nullptr,
};

static const char* sLocaleNames[] = {
    "ja_JP", "en_GB", "de_DE", "fr_FR", "es_ES", "it_IT", "nl_NL", "zh_CN", "zh_TW", "ko_KR", nullptr,
};

static char sLocaleEnUS[] = "en_US";
static char sLocaleEnAU[] = "en_AU";
static char sLocaleFrCA[] = "fr_CA";

static Region sConsoleRegion = REGION_UNKNOWN;
static Region sGameRegion = REGION_UNKNOWN;
static int sConsoleLang = -1;
static int sGameLang = -1;
static const char* sGameLocale;
static const char* sConsoleLocale;

const char* GetRegionName(Region region) {
    return sRegionNames[region];
}

Region GetConsoleRegion() {
    if (sConsoleRegion != REGION_UNKNOWN) {
        return sConsoleRegion;
    }

    switch (VIGetTvFormat()) {
    case VI_NTSC:
        switch (OSGetFontEncode()) {
        case OS_FONT_ENCODE_SJIS:
            return sConsoleRegion = REGION_JPN;
        case OS_FONT_ENCODE_ANSI:
            return sConsoleRegion = REGION_USA;
        }
        break;
    case VI_PAL:
    case VI_EURGB60:
        return sConsoleRegion = REGION_EUR;
    case VI_MPAL:
        return sConsoleRegion = REGION_BRA;
    }
    return sConsoleRegion;
}

static inline Region GetRegionFromGameCode(char code) {
    switch (code) {
    case 'J':
        return REGION_JPN;
    case 'E':
        return REGION_USA;
    case 'D':
    case 'F':
    case 'I':
    case 'P':
    case 'S':
        return REGION_EUR;
    case 'U':
        return REGION_AUS;
    case 'K':
        return REGION_KOR;
    case 'A':
        return REGION_ALL;
    }
    return REGION_JPN;
}

Region GetGameRegion() {
    if (sGameRegion != REGION_UNKNOWN) {
        return sGameRegion;
    }
    return sGameRegion = GetRegionFromGameCode(OSGetAppGamename()[3]);
}

int GetConsoleLang() {
    if (sConsoleLang >= 0) {
        return sConsoleLang;
    }
    return sConsoleLang = SCGetLanguage();
}

int GetGameLang() {
    if (sGameLang >= 0) {
        return sGameLang;
    }

    switch (GetGameRegion()) {
    case REGION_ALL:
        sGameLang = SCGetLanguage();
        if (sGameLang <= SC_LANG_ITALIAN) {
            return sGameLang;
        }
        return sGameLang = SC_LANG_ENGLISH;
    case REGION_JPN:
        return sGameLang = SC_LANG_JAPANESE;
    case REGION_USA:
        return sGameLang = SC_LANG_ENGLISH;
    case REGION_EUR:
        sGameLang = SCGetLanguage();
        switch (sGameLang) {
        case SC_LANG_ENGLISH:
        case SC_LANG_GERMAN:
        case SC_LANG_FRENCH:
        case SC_LANG_SPANISH:
        case SC_LANG_ITALIAN:
            return sGameLang;
        }
        return sGameLang = SC_LANG_ENGLISH;
    case REGION_AUS:
        return sGameLang = SC_LANG_ENGLISH;
    case REGION_TWN:
    case REGION_HKG:
        return sGameLang = SC_LANG_TRAD_CHINESE;
    case REGION_KOR:
        return sGameLang = SC_LANG_KOREAN;
    case REGION_CHN:
        return sGameLang = SC_LANG_SIMP_CHINESE;
    case REGION_LTN:
        return sGameLang = SC_LANG_SPANISH;
    }
    return sGameLang = SC_LANG_ENGLISH;
}

static inline Region GetContentRegion() {
    Region region = GetGameRegion();
    if (region == REGION_ALL) {
        region = GetConsoleRegion();
    }
    return region;
}

static inline const char* GetLocaleName(Region region, int lang) {
    if (region == REGION_EUR) {
        return sLocaleNames[lang];
    }

    switch (lang) {
    case SC_LANG_ENGLISH:
        switch (region) {
        case REGION_USA:
        case REGION_BRA:
            return sLocaleEnUS;
        case REGION_AUS:
            return sLocaleEnAU;
        }
        break;
    case SC_LANG_FRENCH:
        if (region == REGION_USA) {
            return sLocaleFrCA;
        }
        break;
    }
    return sLocaleNames[lang];
}

const char* GetGameLocale() {
    if (sGameLocale == nullptr) {
        Region region = GetContentRegion();
        int lang = GetGameLang();
        sGameLocale = GetLocaleName(region, lang);
    }
    return sGameLocale;
}

const char* GetConsoleLocale() {
    if (sConsoleLocale == nullptr) {
        Region region = GetContentRegion();
        int lang = GetConsoleLang();
        sConsoleLocale = GetLocaleName(region, lang);
    }
    return sConsoleLocale;
}
