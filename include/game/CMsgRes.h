#ifndef GAME_CMSGRES_H
#define GAME_CMSGRES_H

#include <revolution/types.h>
#include <string.h>

class CMsgRes {
public:
    enum Encoding {
        ENCODING_CP1252 = 1,
        ENCODING_UTF16 = 2,
        ENCODING_SJIS = 3
    };

    struct FileHeader {
        char mSignature[8];
        u32 mFileSize;
        u32 mBlockNum;
        u8 mEncoding;
        u8 mPadding[15];
    };

    struct BlockHeader {
        char mKind[4];
        u32 mSize;
    };

    struct MessageInfo {
        u32 mOffset;
        u32 mAttribute;
    };

    struct InfoBlock {
        BlockHeader mHeader;
        u16 mEntryNum;
        u16 mEntrySize;
        u16 mGroupID;
        u8 mDefaultColor;
        u8 mReserved;
        MessageInfo mEntries[];
    };

    struct DataBlock {
        BlockHeader mHeader;
        u8 mData[];
    };

    CMsgRes(void* pResource, bool convertEscape);
    ~CMsgRes();

    int GetGroupID() const;
    wchar_t* GetMessage(int index) const;

private:
    void ConvertEscape();
    void ConvertEscapeW();
    void Setup();

    FileHeader* mpHeader;
    InfoBlock* mpInfo;
    void* mpResource;
    DataBlock* mpData;
    BlockHeader* mpString;
};

enum CollationCategory {
    COLLATION_SYMBOL = 1,
    COLLATION_PRIVATE = 2,
    COLLATION_DIGIT = 3,
    COLLATION_KANA = 4,
    COLLATION_LETTER = 5
};

union CollationKey {
    struct {
        u32 mCategory : 3;
        u32 mPrimary : 8;
        u32 mSecondary : 8;
        u32 mAccent : 4;
        u32 mCase : 4;
        u32 mWidth : 3;
    };
    u32 mValue;

    void Clear() { memset(this, 0, sizeof(*this)); }

    void Set(u32 category, u32 primary, u32 secondary, u32 accent = 2, u32 caseLevel = 2, u32 width = 2) {
        Clear();
        mCategory = category;
        mPrimary = primary;
        mSecondary = secondary;
        mAccent = accent;
        mCase = caseLevel;
        mWidth = width;
    }

    int Compare(const CollationKey& rOther) const {
        if (mCategory != rOther.mCategory) {
            return mCategory - rOther.mCategory;
        }
        if (mPrimary != rOther.mPrimary) {
            return mPrimary - rOther.mPrimary;
        }
        if (mSecondary != rOther.mSecondary) {
            return mSecondary - rOther.mSecondary;
        }
        if (mAccent != rOther.mAccent) {
            return mAccent - rOther.mAccent;
        }
        if (mCase != rOther.mCase) {
            return mCase - rOther.mCase;
        }
        if (mWidth != rOther.mWidth) {
            return mWidth - rOther.mWidth;
        }
        return 0;
    }
};

int MakeCollationKeys(CollationKey* pKeys, const wchar_t* pString, int maxNum);
int CompareCollationKeys(const CollationKey* pKeysA, const CollationKey* pKeysB, int level);
int CompareCollation(const wchar_t* pStringA, const wchar_t* pStringB);

enum TextLocale {
    TEXT_LOCALE_EUROPE = 0,
    TEXT_LOCALE_JAPAN = 1,
    TEXT_LOCALE_KOREA = 3,
    TEXT_LOCALE_TRADITIONAL_CHINESE = 4,
    TEXT_LOCALE_SIMPLIFIED_CHINESE = 5
};

enum CaseType {
    CASE_LOWER = 1,
    CASE_UPPER = 2
};

void SetTextLocale(int locale);
wchar_t ConvertCase(wchar_t c, int type);

#endif
