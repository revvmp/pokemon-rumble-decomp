#include <game/CMsgRes.h>

#include <MSL_C/locale.h>
#include <string.h>
#include <wstring.h>


static const u32 sAsciiCollation[96] = {
    0x20040448, 0x20042448, 0x20044448, 0x20046448, 0x20048448, 0x2004A448,
    0x2004C448, 0x2004E448, 0x20050448, 0x20052448, 0x20054448, 0x20056448,
    0x20058448, 0x2005A448, 0x2005C448, 0x2005E448, 0x60060448, 0x60062448,
    0x60064448, 0x60066448, 0x60068448, 0x6006A448, 0x6006C448, 0x6006E448,
    0x60070448, 0x60072448, 0x20074448, 0x20076448, 0x20078448, 0x2007A448,
    0x2007C448, 0x2007E448, 0x20080448, 0xA0082468, 0xA0084468, 0xA0086468,
    0xA0088468, 0xA008A468, 0xA008C468, 0xA008E468, 0xA0090468, 0xA0092468,
    0xA0094468, 0xA0096468, 0xA0098468, 0xA009A468, 0xA009C468, 0xA009E468,
    0xA00A0468, 0xA00A2468, 0xA00A4468, 0xA00A6468, 0xA00A8468, 0xA00AA468,
    0xA00AC468, 0xA00AE468, 0xA00B0468, 0xA00B2468, 0xA00B4468, 0x200B6448,
    0x200B8448, 0x200BA448, 0x200BC448, 0x200BE448, 0x200C0448, 0xA00824A8,
    0xA00844A8, 0xA00864A8, 0xA00884A8, 0xA008A4A8, 0xA008C4A8, 0xA008E4A8,
    0xA00904A8, 0xA00924A8, 0xA00944A8, 0xA00964A8, 0xA00984A8, 0xA009A4A8,
    0xA009C4A8, 0xA009E4A8, 0xA00A04A8, 0xA00A24A8, 0xA00A44A8, 0xA00A64A8,
    0xA00A84A8, 0xA00AA4A8, 0xA00AC4A8, 0xA00AE4A8, 0xA00B04A8, 0xA00B24A8,
    0xA00B44A8, 0x200F6448, 0x200F8448, 0x200FA448, 0x200FC448, 0x000FE448,
};

static const u32 sLatin1Collation[96] = {
    0x20140448, 0x20142448, 0x20144448, 0x20146448, 0x20148448, 0x2014A448,
    0x2014C448, 0x2014E448, 0x20150448, 0x20152448, 0x20154448, 0x20156448,
    0x20158448, 0x2015A448, 0x2015C448, 0x2015E448, 0x20160448, 0x20162448,
    0x20164448, 0x20166448, 0x20168448, 0x2016A448, 0x2016C448, 0x2016E448,
    0x20170448, 0x20172448, 0x20174448, 0x20176448, 0x20178448, 0x2017A448,
    0x2017C448, 0x2017E448, 0xA0082868, 0xA0082668, 0xA0082A68, 0xA0082E68,
    0xA0082C68, 0xA0083268, 0xA018C468, 0xA0087468, 0xA008A868, 0xA008A668,
    0xA008AA68, 0xA008AC68, 0xA0092868, 0xA0092668, 0xA0092A68, 0xA0092C68,
    0xA0089668, 0xA009CE68, 0xA009E868, 0xA009E668, 0xA009EA68, 0xA009EE68,
    0xA009EC68, 0x201AE448, 0xA009F668, 0xA00AA868, 0xA00AA668, 0xA00AAA68,
    0xA00AAC68, 0xA00B2668, 0xA01BC468, 0xA01BE4A8, 0xA00828A8, 0xA00826A8,
    0xA0082AA8, 0xA0082EA8, 0xA0082CA8, 0xA00832A8, 0xA018C4A8, 0xA00874A8,
    0xA008A8A8, 0xA008A6A8, 0xA008AAA8, 0xA008ACA8, 0xA00928A8, 0xA00926A8,
    0xA0092AA8, 0xA0092CA8, 0xA00896A8, 0xA009CEA8, 0xA009E8A8, 0xA009E6A8,
    0xA009EAA8, 0xA009EEA8, 0xA009ECA8, 0x201EE448, 0xA009F6A8, 0xA00AA8A8,
    0xA00AA6A8, 0xA00AAAA8, 0xA00AACA8, 0xA00B26A8, 0xA01BC4A8, 0xA00B2CA8,
};

static const u32 sGreekCollation[80] = {
    0x00700448, 0x00702448, 0x00704448, 0x00706448, 0x20708448, 0x2070A448,
    0xC0722668, 0x2070E448, 0xC072A668, 0xC072E668, 0xC0732668, 0x00716448,
    0xC073E668, 0x0071A448, 0xC074A668, 0xC0752668, 0xC07330A8, 0xC0722468,
    0xC0724468, 0xC0726468, 0xC0728468, 0xC072A468, 0xC072C468, 0xC072E468,
    0xC0730468, 0xC0732468, 0xC0734468, 0xC0736468, 0xC0738468, 0xC073A468,
    0xC073C468, 0xC073E468, 0xC0740468, 0xC0742468, 0x00744448, 0xC0746468,
    0xC0748468, 0xC074A468, 0xC074C468, 0xC074E468, 0xC0750468, 0xC0752468,
    0xC0732C68, 0xC074AC68, 0xC07226A8, 0xC072A6A8, 0xC072E6A8, 0xC07326A8,
    0xC074B0A8, 0xC07224A8, 0xC07244A8, 0xC07264A8, 0xC07284A8, 0xC072A4A8,
    0xC072C4A8, 0xC072E4A8, 0xC07304A8, 0xC07324A8, 0xC07344A8, 0xC07364A8,
    0xC07384A8, 0xC073A4A8, 0xC073C4A8, 0xC073E4A8, 0xC07404A8, 0xC07424A8,
    0xC0746488, 0xC07464A8, 0xC07484A8, 0xC074A4A8, 0xC074C4A8, 0xC074E4A8,
    0xC07504A8, 0xC07524A8, 0xC0732CA8, 0xC074ACA8, 0xC073E6A8, 0xC074A6A8,
    0xC07526A8, 0x0079E448,
};

static const u32 sFullWidthCollation[94] = {
    0x20042450, 0x20044450, 0x20046450, 0x20048450, 0x2004A450, 0x2004C450,
    0x2004E450, 0x20050450, 0x20052450, 0x20054450, 0x20056450, 0x20058450,
    0x2005A450, 0x2005C450, 0x2005E450, 0x60060450, 0x60062450, 0x60064450,
    0x60066450, 0x60068450, 0x6006A450, 0x6006C450, 0x6006E450, 0x60070450,
    0x60072450, 0x20074450, 0x20076450, 0x20078450, 0x2007A450, 0x2007C450,
    0x2007E450, 0x20080450, 0xA0082470, 0xA0084470, 0xA0086470, 0xA0088470,
    0xA008A470, 0xA008C470, 0xA008E470, 0xA0090470, 0xA0092470, 0xA0094470,
    0xA0096470, 0xA0098470, 0xA009A470, 0xA009C470, 0xA009E470, 0xA00A0470,
    0xA00A2470, 0xA00A4470, 0xA00A6470, 0xA00A8470, 0xA00AA470, 0xA00AC470,
    0xA00AE470, 0xA00B0470, 0xA00B2470, 0xA00B4470, 0x200B6450, 0x200B8450,
    0x200BA450, 0x200BC450, 0x200BE450, 0x200C0450, 0xA00824B0, 0xA00844B0,
    0xA00864B0, 0xA00884B0, 0xA008A4B0, 0xA008C4B0, 0xA008E4B0, 0xA00904B0,
    0xA00924B0, 0xA00944B0, 0xA00964B0, 0xA00984B0, 0xA009A4B0, 0xA009C4B0,
    0xA009E4B0, 0xA00A04B0, 0xA00A24B0, 0xA00A44B0, 0xA00A64B0, 0xA00A84B0,
    0xA00AA4B0, 0xA00AC4B0, 0xA00AE4B0, 0xA00B04B0, 0xA00B24B0, 0xA00B44B0,
    0x200F6450, 0x200F8450, 0x200FA450, 0x200FC450,
};

static const u32 sKanaCollation[90] = {
    0x80202528, 0x80202568, 0x80204528, 0x80204568, 0x80206528, 0x80206568,
    0x80208528, 0x80208568, 0x8020A528, 0x8020A568, 0x80402568, 0x80403968,
    0x80404568, 0x80405968, 0x80406568, 0x80407968, 0x80408568, 0x80409968,
    0x8040A568, 0x8040B968, 0x80602568, 0x80603968, 0x80604568, 0x80605968,
    0x80606568, 0x80607968, 0x80608568, 0x80609968, 0x8060A568, 0x8060B968,
    0x80802568, 0x80803968, 0x80804568, 0x80805968, 0x80806528, 0x80806568,
    0x80807968, 0x80808568, 0x80809968, 0x8080A568, 0x8080B968, 0x80A02568,
    0x80A04568, 0x80A06568, 0x80A08568, 0x80A0A568, 0x80C02568, 0x80C03968,
    0x80C03B68, 0x80C04568, 0x80C05968, 0x80C05B68, 0x80C06568, 0x80C07968,
    0x80C07B68, 0x80C08568, 0x80C09968, 0x80C09B68, 0x80C0A568, 0x80C0B968,
    0x80C0BB68, 0x80E02568, 0x80E04568, 0x80E06568, 0x80E08568, 0x80E0A568,
    0x81002528, 0x81002568, 0x81006528, 0x81006568, 0x8100A528, 0x8100A568,
    0x81202568, 0x81204568, 0x81206568, 0x81208568, 0x8120A568, 0x81402528,
    0x81402568, 0x81404568, 0x81408568, 0x8140A568, 0x8160C568, 0x80207968,
    0x80402528, 0x80408528, 0x81403968, 0x81405968, 0x81409968, 0x8140B968,
};

static const u32 sSpecialCollation[] = {
    0xA00824C8, 0xA008A4C8, 0xA009E4C8, 0xA00824E8, 0xA008A4E8, 0xA009E4E8, 0xA00A64E8, 0xA00B2C68,
};

namespace {

inline int IsDigit(int c) {
    return ((c < 0) || (c >= 0x100)) ? 0 : (int)(_current_locale.ctype_cmpt_ptr->ctype_map_ptr[c] & 0x8);
}

inline int IsHexDigit(int c) {
    return ((c < 0) || (c >= 0x100)) ? 0 : (int)(_current_locale.ctype_cmpt_ptr->ctype_map_ptr[c] & 0x400);
}

inline int ToLower(int c) {
    return ((c < 0) || (c >= 0x100)) ? c : (int)(_current_locale.ctype_cmpt_ptr->lower_map_ptr[c]);
}

inline int IsDigitW(wchar_t c) {
    return (c >= 0x100) ? 0 : (int)(_current_locale.ctype_cmpt_ptr->wctype_map_ptr[c] & 0x8);
}

inline int IsHexDigitW(wchar_t c) {
    return (c >= 0x100) ? 0 : (int)(_current_locale.ctype_cmpt_ptr->wctype_map_ptr[c] & 0x400);
}

inline wchar_t ToLowerW(wchar_t c) {
    return (c >= 0x100) ? c : _current_locale.ctype_cmpt_ptr->wlower_map_ptr[c];
}

inline bool IsSjisLeadByte(u8 c) {
    return (c >= 0x81 && c <= 0x9F) || (c >= 0xE0 && c <= 0xFC);
}

}

CMsgRes::CMsgRes(void* pResource, bool convertEscape)
    : mpHeader(nullptr), mpInfo(nullptr), mpData(nullptr), mpString(nullptr) {
    mpResource = pResource;
    Setup();
    if (convertEscape) {
        if (mpHeader->mEncoding == ENCODING_UTF16) {
            ConvertEscapeW();
        } else {
            ConvertEscape();
        }
    }
}

CMsgRes::~CMsgRes() {}

void CMsgRes::ConvertEscape() {
    if (mpHeader->mEncoding == ENCODING_UTF16) {
        return;
    }

    char* pSrc = reinterpret_cast<char*>(mpData->mData);
    char* pDst = pSrc;
    int digit;
    char* pEnd = reinterpret_cast<char*>(mpData) + mpData->mHeader.mSize;
    bool isTrailByte = false;
    bool isSjis = mpHeader->mEncoding == ENCODING_SJIS;

    while (pSrc < pEnd) {
        if (isTrailByte) {
            isTrailByte = false;
            *pDst++ = *pSrc++;
            continue;
        }

        char c = *pSrc;
        if (c == '\\') {
            signed char escape = pSrc[1];
            pSrc += 2;
            switch (escape) {
            case 'r':
                *pDst++ = '\r';
                break;
            case 'f':
                *pDst++ = '\f';
                break;
            case 'x': {
                int value;
                int i = 0;
                for (value = 0; i < 2 && IsHexDigit(digit = *pSrc); pSrc++, i++) {
                    value = (value << 4) | (IsDigit(digit) ? digit - '0' : ToLower(digit) - 'a' + 10);
                }
                *pDst++ = value;
                break;
            }
            default:
                *pDst++ = escape;
                break;
            }
        } else if (c == '\0') {
            isTrailByte = false;
            *pDst = '\0';
            pSrc++;
            pDst = pSrc;
        } else {
            if (isSjis) {
                isTrailByte = IsSjisLeadByte(c);
            }
            *pDst++ = *pSrc++;
        }
    }
}

void CMsgRes::ConvertEscapeW() {
    if (mpHeader->mEncoding != ENCODING_UTF16) {
        return;
    }

    wchar_t* pSrc = reinterpret_cast<wchar_t*>(mpData->mData);
    wchar_t* pDst = reinterpret_cast<wchar_t*>(mpData->mData);
    wchar_t* pEnd = reinterpret_cast<wchar_t*>(reinterpret_cast<u8*>(mpData) + mpData->mHeader.mSize);

    while (pSrc < pEnd) {
        wchar_t c = *pSrc;
        if (c == L'\\') {
            wchar_t escape = pSrc[1];
            pSrc += 2;
            switch (escape) {
            case L'r':
                *pDst++ = L'\r';
                break;
            case L'f':
                *pDst++ = L'\f';
                break;
            case L'x': {
                int value = 0;
                for (int i = 0; i < 2 && IsHexDigitW(*pSrc); pSrc++, i++) {
                    value = (value << 4) | (IsDigitW(*pSrc) ? *pSrc - L'0' : ToLowerW(*pSrc) - L'a' + 10);
                }
                *pDst++ = value;
                break;
            }
            case L'u': {
                int value = 0;
                for (int i = 0; i < 4 && IsHexDigitW(*pSrc); pSrc++, i++) {
                    value = (value << 4) | (IsDigitW(*pSrc) ? *pSrc - L'0' : ToLowerW(*pSrc) - L'a' + 10);
                }
                *pDst++ = value;
                break;
            }
            default:
                *pDst++ = escape;
                break;
            }
        } else if (c == L'\0') {
            *pDst = L'\0';
            pSrc++;
            pDst = pSrc;
        } else {
            *pDst++ = c;
            pSrc++;
        }
    }
}

void CMsgRes::Setup() {
    if (mpInfo != nullptr) {
        return;
    }

    u8* pBlock = static_cast<u8*>(mpResource);
    if (pBlock != nullptr) {
        mpHeader = reinterpret_cast<FileHeader*>(pBlock);
        s32 size = mpHeader->mFileSize;
        pBlock += sizeof(FileHeader);
        size -= sizeof(FileHeader);

        while (size > 0) {
            BlockHeader* pHeader = reinterpret_cast<BlockHeader*>(pBlock);
            if (memcmp(pHeader->mKind, "INF1", 4) == 0) {
                mpInfo = reinterpret_cast<InfoBlock*>(pHeader);
            } else if (memcmp(pHeader->mKind, "DAT1", 4) == 0) {
                mpData = reinterpret_cast<DataBlock*>(pHeader);
            } else if (memcmp(pHeader->mKind, "STR1", 4) == 0) {
                mpString = pHeader;
            }
            size -= pHeader->mSize;
            pBlock += pHeader->mSize;
        }
    }
}

int CMsgRes::GetGroupID() const {
    if (mpInfo != nullptr) {
        return mpInfo->mGroupID;
    }
    return -1;
}

wchar_t* CMsgRes::GetMessage(int index) const {
    return reinterpret_cast<wchar_t*>(&mpData->mData[mpInfo->mEntries[index].mOffset]);
}

namespace {

inline void SetProlongedSoundKey(CollationKey& rKey, wchar_t c) {
    if (rKey.mCategory == COLLATION_KANA) {
        u32 category = rKey.mCategory;
        u32 secondary = rKey.mSecondary;
        u32 primary = rKey.mPrimary == 11 ? 11 : 1;
        rKey.Set(category, primary, secondary, 2, 8, 2);
    } else {
        rKey.Set(COLLATION_SYMBOL, c >> 8, c);
    }
}

inline void SetCharacterKey(CollationKey& rKey, wchar_t c) {
    if (0x0020 <= c && c < 0x007F) {
        rKey.mValue = sAsciiCollation[c - 0x0020];
        return;
    }
    if (0x00A0 <= c && c < 0x0100) {
        rKey.mValue = sLatin1Collation[c - 0x00A0];
        return;
    }
    if (0x3041 <= c && c < 0x3094) {
        rKey.mValue = sKanaCollation[c - 0x3041];
        return;
    }
    if (0x30A1 <= c && c < 0x30F7) {
        rKey.mValue = sKanaCollation[c - 0x30A1];
        return;
    }
    if (0x0380 <= c && c < 0x03D0) {
        rKey.mValue = sGreekCollation[c - 0x0380];
        return;
    }
    if (0xE000 <= c && c < 0xE070) {
        rKey.Set(COLLATION_PRIVATE, c >> 8, c);
        return;
    }
    if (0xFF01 <= c && c < 0xFF5F) {
        rKey.mValue = sFullWidthCollation[c - 0xFF01];
        return;
    }
    rKey.Set(COLLATION_SYMBOL, c >> 8, c);
}

}

int MakeCollationKeys(CollationKey* pKeys, const wchar_t* pString, int maxNum) {
    int num = 0;
    CollationKey key;
    key.Set(0, 0, 0);
    wchar_t c;
    bool isSecondHalf = false;

    while (isSecondHalf || (c = *pString++) != L'\0') {
        switch (c) {
        case 0x30FC:
            SetProlongedSoundKey(key, c);
            break;
        case 0x00C6:
            key.mValue = isSecondHalf ? sSpecialCollation[1] : sSpecialCollation[0];
            isSecondHalf = !isSecondHalf;
            break;
        case 0x00E6:
            key.mValue = isSecondHalf ? sSpecialCollation[4] : sSpecialCollation[3];
            isSecondHalf = !isSecondHalf;
            break;
        case 0x0152:
            key.mValue = isSecondHalf ? sSpecialCollation[1] : sSpecialCollation[2];
            isSecondHalf = !isSecondHalf;
            break;
        case 0x0153:
            key.mValue = isSecondHalf ? sSpecialCollation[4] : sSpecialCollation[5];
            isSecondHalf = !isSecondHalf;
            break;
        case 0x00DF:
            key.mValue = sSpecialCollation[6];
            isSecondHalf = !isSecondHalf;
            break;
        case 0x0178:
            key.mValue = sSpecialCollation[7];
            break;
        default:
            SetCharacterKey(key, c);
            break;
        }

        if (maxNum != 0) {
            *pKeys++ = key;
            maxNum--;
        }
        num++;
    }

    if (maxNum != 0) {
        key.Clear();
        *pKeys = key;
    }
    return num;
}

int CompareCollationKeys(const CollationKey* pKeysA, const CollationKey* pKeysB, int level) {
    CollationKey terminator;
    terminator.Clear();

    switch (level) {
    case 1: {
        CollationKey a = *pKeysA++;
        CollationKey b = *pKeysB++;
        while (a.mCategory == b.mCategory && a.mPrimary == b.mPrimary && a.mSecondary == b.mSecondary) {
            if (a.Compare(terminator) == 0) {
                return 0;
            }
            a = *pKeysA++;
            b = *pKeysB++;
        }
        return a.Compare(b);
    }
    case 2: {
        CollationKey a = *pKeysA++;
        CollationKey b = *pKeysB++;
        while (a.mAccent == b.mAccent) {
            if (a.Compare(terminator) == 0) {
                return 0;
            }
            a = *pKeysA++;
            b = *pKeysB++;
        }
        return a.mAccent - b.mAccent;
    }
    case 3: {
        CollationKey a = *pKeysA++;
        CollationKey b = *pKeysB++;
        while (a.mCase == b.mCase) {
            if (a.Compare(terminator) == 0) {
                return 0;
            }
            a = *pKeysA++;
            b = *pKeysB++;
        }
        return a.mCase - b.mCase;
    }
    case 4: {
        CollationKey a = *pKeysA++;
        CollationKey b = *pKeysB++;
        while (a.mWidth == b.mWidth) {
            if (a.Compare(terminator) == 0) {
                return 0;
            }
            a = *pKeysA++;
            b = *pKeysB++;
        }
        return a.mWidth - b.mWidth;
    }
    }
    return 0;
}

int CompareCollation(const wchar_t* pStringA, const wchar_t* pStringB) {
    int numA = MakeCollationKeys(nullptr, pStringA, 0) + 1;
    int numB = MakeCollationKeys(nullptr, pStringB, 0) + 1;
    CollationKey* pKeysA = new CollationKey[numA];
    CollationKey* pKeysB = new CollationKey[numB];
    MakeCollationKeys(pKeysA, pStringA, numA);
    MakeCollationKeys(pKeysB, pStringB, numB);

    int result = CompareCollationKeys(pKeysA, pKeysB, 1);
    if (result == 0) {
        result = CompareCollationKeys(pKeysA, pKeysB, 2);
        if (result == 0) {
            result = CompareCollationKeys(pKeysA, pKeysB, 3);
            if (result == 0) {
                result = wcscmp(pStringA, pStringB);
            }
        }
    }

    delete[] pKeysA;
    delete[] pKeysB;
    return result;
}

static int sTextLocale = TEXT_LOCALE_JAPAN;

static const wchar_t* sUpperLatin1 =
    L"\x00C0\x00C1\x00C2\x00C3\x00C4\x00C5\x00C6\x00C7"
    L"\x00C8\x00C9\x00CA\x00CB\x00CC\x00CD\x00CE\x00CF"
    L"\x00D0\x00D1\x00D2\x00D3\x00D4\x00D5\x00D6\x00D7"
    L"\x00D8\x00D9\x00DA\x00DB\x00DC\x00DD\x00DE\x00DF"
    L"\x00C0\x00C1\x00C2\x00C3\x00C4\x00C5\x00C6\x00C7"
    L"\x00C8\x00C9\x00CA\x00CB\x00CC\x00CD\x00CE\x00CF"
    L"\x00D0\x00D1\x00D2\x00D3\x00D4\x00D5\x00D6\x00F7"
    L"\x00D8\x00D9\x00DA\x00DB\x00DC\x00DD\x00DE\x0178";

static const wchar_t* sLowerLatin1 =
    L"\x00E0\x00E1\x00E2\x00E3\x00E4\x00E5\x00E6\x00E7"
    L"\x00E8\x00E9\x00EA\x00EB\x00EC\x00ED\x00EE\x00EF"
    L"\x00F0\x00F1\x00F2\x00F3\x00F4\x00F5\x00F6\x00D7"
    L"\x00F8\x00F9\x00FA\x00FB\x00FC\x00FD\x00FE\x00DF"
    L"\x00E0\x00E1\x00E2\x00E3\x00E4\x00E5\x00E6\x00E7"
    L"\x00E8\x00E9\x00EA\x00EB\x00EC\x00ED\x00EE\x00EF"
    L"\x00F0\x00F1\x00F2\x00F3\x00F4\x00F5\x00F6\x00F7"
    L"\x00F8\x00F9\x00FA\x00FB\x00FC\x00FD\x00FE\x00FF";

static const wchar_t* sUpperLatinExtA =
    L"\x0100\x0100\x0102\x0102\x0104\x0104\x0106\x0106"
    L"\x0108\x0108\x010A\x010A\x010C\x010C\x010E\x010E"
    L"\x0110\x0110\x0112\x0112\x0114\x0114\x0116\x0116"
    L"\x0118\x0118\x011A\x011A\x011C\x011C\x011E\x011E"
    L"\x0120\x0120\x0122\x0122\x0124\x0124\x0126\x0126"
    L"\x0128\x0128\x012A\x012A\x012C\x012C\x012E\x012E"
    L"\x0130\x0130\x0132\x0132\x0134\x0134\x0136\x0136"
    L"\x0138\x0139\x0139\x013B\x013B\x013D\x013D\x013F"
    L"\x013F\x0141\x0141\x0143\x0143\x0145\x0145\x0147"
    L"\x0147\x0149\x014A\x014A\x014C\x014C\x014E\x014E"
    L"\x0150\x0150\x0152\x0152\x0154\x0154\x0156\x0156"
    L"\x0158\x0158\x015A\x015A\x015C\x015C\x015E\x015E"
    L"\x0160\x0160\x0162\x0162\x0164\x0164\x0166\x0166"
    L"\x0168\x0168\x016A\x016A\x016C\x016C\x016E\x016E"
    L"\x0170\x0170\x0172\x0172\x0174\x0174\x0176\x0176"
    L"\x0178\x0179\x0179\x017B\x017B\x017D\x017D\x017F";

static const wchar_t* sLowerLatinExtA =
    L"\x0101\x0101\x0103\x0103\x0105\x0105\x0107\x0107"
    L"\x0109\x0109\x010B\x010B\x010D\x010D\x010F\x010F"
    L"\x0111\x0111\x0113\x0113\x0115\x0115\x0117\x0117"
    L"\x0119\x0119\x011B\x011B\x011D\x011D\x011F\x011F"
    L"\x0121\x0121\x0123\x0123\x0125\x0125\x0127\x0127"
    L"\x0129\x0129\x012B\x012B\x012D\x012D\x012F\x012F"
    L"\x0131\x0131\x0133\x0133\x0135\x0135\x0137\x0137"
    L"\x0138\x013A\x013A\x013C\x013C\x013E\x013E\x0140"
    L"\x0140\x0142\x0142\x0144\x0144\x0146\x0146\x0148"
    L"\x0148\x0149\x014B\x014B\x014D\x014D\x014F\x014F"
    L"\x0151\x0151\x0153\x0153\x0155\x0155\x0157\x0157"
    L"\x0159\x0159\x015B\x015B\x015D\x015D\x015F\x015F"
    L"\x0161\x0161\x0163\x0163\x0165\x0165\x0167\x0167"
    L"\x0169\x0169\x016B\x016B\x016D\x016D\x016F\x016F"
    L"\x0171\x0171\x0173\x0173\x0175\x0175\x0177\x0177"
    L"\x00FF\x017A\x017A\x017C\x017C\x017E\x017E\x017F";

static const wchar_t* sUpperGreek =
    L"\x0380\x0381\x0382\x0383\x0384\x0385\x0386\x0387"
    L"\x0388\x0389\x038A\x038B\x038C\x038D\x038E\x038F"
    L"\x03AA\x0391\x0392\x0393\x0394\x0395\x0396\x0397"
    L"\x0398\x0399\x039A\x039B\x039C\x039D\x039E\x039F"
    L"\x03A0\x03A1\x03A2\x03A3\x03A4\x03A5\x03A6\x03A7"
    L"\x03A8\x03A9\x03AA\x03AB\x0386\x0388\x0389\x038A"
    L"\x03AB\x0391\x0392\x0393\x0394\x0395\x0396\x0397"
    L"\x0398\x0399\x039A\x039B\x039C\x039D\x039E\x039F"
    L"\x03A0\x03A1\x03A3\x03A3\x03A4\x03A5\x03A6\x03A7"
    L"\x03A8\x03A9\x03AA\x03AB\x038C\x038E\x038F\x03CF";

static const wchar_t* sLowerGreek =
    L"\x0380\x0381\x0382\x0383\x0384\x0385\x03AC\x0387"
    L"\x03AD\x03AE\x03AF\x038B\x03CC\x038D\x03CD\x03CE"
    L"\x0390\x03B1\x03B2\x03B3\x03B4\x03B5\x03B6\x03B7"
    L"\x03B8\x03B9\x03BA\x03BB\x03BC\x03BD\x03BE\x03BF"
    L"\x03C0\x03C1\x03A2\x03C3\x03C4\x03C5\x03C6\x03C7"
    L"\x03C8\x03C9\x03CA\x03CB\x03AC\x03AD\x03AE\x03AF"
    L"\x03B0\x03B1\x03B2\x03B3\x03B4\x03B5\x03B6\x03B7"
    L"\x03B8\x03B9\x03BA\x03BB\x03BC\x03BD\x03BE\x03BF"
    L"\x03C0\x03C1\x03C2\x03C3\x03C4\x03C5\x03C6\x03C7"
    L"\x03C8\x03C9\x03CA\x03CB\x03CC\x03CD\x03CE\x03CF";

void SetTextLocale(int locale) {
    sTextLocale = locale;
}

wchar_t ConvertCase(wchar_t c, int type) {
    switch (type) {
    case CASE_UPPER:
        if (c < 0x0080) {
            return _current_locale.ctype_cmpt_ptr->wupper_map_ptr[c];
        }
        if (0x00C0 <= c && c < 0x0100) {
            return sUpperLatin1[c - 0x00C0];
        }
        if (0x0100 <= c && c < 0x0180) {
            return sUpperLatinExtA[c - 0x0100];
        }
        if (0x0380 <= c && c < 0x03D0) {
            return sUpperGreek[c - 0x0380];
        }
        if (0xFF41 <= c && c < 0xFF5B) {
            return c - 0x20;
        }
        if (0x0430 <= c && c < 0x0450) {
            return c - 0x20;
        }
        if (0x0450 <= c && c < 0x0460) {
            return c - 0x50;
        }
        return c;
    case CASE_LOWER:
        if (c < 0x0080) {
            return _current_locale.ctype_cmpt_ptr->wlower_map_ptr[c];
        }
        if (0x00C0 <= c && c < 0x0100) {
            return sLowerLatin1[c - 0x00C0];
        }
        if (0x0100 <= c && c < 0x0180) {
            return sLowerLatinExtA[c - 0x0100];
        }
        if (0x0380 <= c && c < 0x03D0) {
            return sLowerGreek[c - 0x0380];
        }
        if (0xFF21 <= c && c < 0xFF3B) {
            return c + 0x20;
        }
        if (0x0410 <= c && c < 0x0430) {
            return c + 0x20;
        }
        if (0x0400 <= c && c < 0x0410) {
            return c + 0x50;
        }
        return c;
    }
    return 0;
}

extern "C" wchar_t towupper(wchar_t c) {
    return ConvertCase(c, CASE_UPPER);
}

extern "C" int toupper(int c) {
    if (sTextLocale == TEXT_LOCALE_EUROPE) {
        return ConvertCase(c, CASE_UPPER);
    }
    return ((c < 0) || (c >= 0x100)) ? c : (int)(_current_locale.ctype_cmpt_ptr->upper_map_ptr[c]);
}
