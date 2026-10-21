#ifndef GAME_SAVEDATA_H
#define GAME_SAVEDATA_H

#include <revolution/types.h>
#include <string.h>
#include <vector>

class CMapRecord {
public:
    CMapRecord() : mValue(0) {}

    bool IsFlagSet(u32 flags) const { return (mValue & flags) == flags; }
    int GetClearCount() const { return mValue >> 22; }

private:
    u32 mValue;
};

enum VectorStructType {
    VECTOR_STRUCT_NONE,
    VECTOR_STRUCT_PII_PERSONAL_DATA,
    VECTOR_STRUCT_BATTLE_ROYAL_DATA,
    VECTOR_STRUCT_SLOT_DATA,
    VECTOR_STRUCT_STATISTIC_VALUE_1,
    VECTOR_STRUCT_STATISTIC_VALUE_2,
    VECTOR_STRUCT_STATISTIC_VALUE_3,
    VECTOR_STRUCT_PII_COLLECTION_DATA,
    VECTOR_STRUCT_USED_PASSWORD
};

class SD_PiiPersonalData;
class SD_BattleRoyalData;
class SD_StatisticValue;
class SD_PiiCollectionData;
class UsedPassword;
class SlotData;

class VectorStructs {
public:
    virtual std::vector<SD_PiiPersonalData>* GetPiiPersonalData() { return nullptr; }
    virtual std::vector<SD_PiiCollectionData>* GetPiiCollectionData() { return nullptr; }
    virtual std::vector<SD_BattleRoyalData>* GetBattleRoyalData() { return nullptr; }
    virtual std::vector<SlotData>* GetSlotData() { return nullptr; }
    virtual std::vector<SD_StatisticValue>* GetStatisticValue1() { return nullptr; }
    virtual std::vector<SD_StatisticValue>* GetStatisticValue2() { return nullptr; }
    virtual std::vector<SD_StatisticValue>* GetStatisticValue3() { return nullptr; }
    virtual std::vector<UsedPassword>* GetUsedPassword() { return nullptr; }
};

class SD_PiiPersonalData : public VectorStructs {
public:
    struct Data {
        u32 mValues[11];
    };

    SD_PiiPersonalData() { ClearData(); }

    static u16 GetDataSize() { return sizeof(Data); }
    void ClearData() { memset(&mData, 0, sizeof(mData)); }

    Data mData;

    static VectorStructType sVectorTypes[1];
};

class SD_BattleRoyalData : public VectorStructs {
public:
    struct Data {
        f32 mValue;
    };

    SD_BattleRoyalData() { ClearData(); }

    static u16 GetDataSize() { return sizeof(Data); }
    void ClearData() { memset(&mData, 0, sizeof(mData)); }

    Data mData;

    static VectorStructType sVectorTypes[1];
};

class SD_StatisticValue : public VectorStructs {
public:
    struct Data {
        u32 mValue;
    };

    SD_StatisticValue() { ClearData(); }

    static u16 GetDataSize() { return sizeof(Data); }
    void ClearData() { memset(&mData, 0, sizeof(mData)); }

    Data mData;

    static VectorStructType sVectorTypes[1];
};

class SD_PiiCollectionData : public VectorStructs {
public:
    struct Data {
        u32 mValues[4];
    };

    SD_PiiCollectionData() { ClearData(); }

    static u16 GetDataSize() { return sizeof(Data); }
    void ClearData() { memset(&mData, 0, sizeof(mData)); }

    Data mData;

    static VectorStructType sVectorTypes[1];
};

class UsedPassword : public VectorStructs {
public:
    struct Data {
        int mPassword;
    };

    UsedPassword() { ClearData(); }

    static u16 GetDataSize() { return sizeof(Data); }
    void ClearData() { memset(&mData, 0, sizeof(mData)); }

    Data mData;

    static VectorStructType sVectorTypes[1];
};

class SlotData : public VectorStructs {
public:
    struct Data {
        u32 mHeader[3];
        s8 mFlags[4];
        CMapRecord mMapRecords[3][5][8][10];
        u32 mCounters[2];
        s64 mTime;
        union {
            struct {
                u32 mRegistryCount;
                u32 mElapsedSeconds;
                u16 mTicketMonsno;
                u8 mTicketFormno;
                u8 mReserved[1];
                u8 mLuckyNumberSeed[6];
                s8 mExtraBRMaxRound;
                s8 mExtraBRCurrentRound;
            };
            u8 mStatus[0x14];
        };
        u32 mLeadPiiIdHigh;
        u16 mLeadPiiMonsno;
        s8 mLastArea[2];
        u32 mLeadPiiIdLow;
    };

    SlotData() { ClearData(); }

    static u16 GetDataSize() { return sizeof(Data); }
    void ClearData() { memset(&mData, 0, sizeof(mData)); }

    virtual std::vector<SD_PiiPersonalData>* GetPiiPersonalData() { return &mPiiPersonalData; }
    virtual std::vector<SD_PiiCollectionData>* GetPiiCollectionData() { return &mPiiCollectionData; }
    virtual std::vector<SD_BattleRoyalData>* GetBattleRoyalData() { return &mBattleRoyalData; }
    virtual std::vector<SD_StatisticValue>* GetStatisticValue1() { return &mStatisticValue1; }
    virtual std::vector<SD_StatisticValue>* GetStatisticValue2() { return &mStatisticValue2; }
    virtual std::vector<SD_StatisticValue>* GetStatisticValue3() { return &mStatisticValue3; }
    virtual std::vector<UsedPassword>* GetUsedPassword() { return &mUsedPassword; }

    Data mData;
    std::vector<SD_PiiPersonalData> mPiiPersonalData;
    std::vector<SD_PiiCollectionData> mPiiCollectionData;
    std::vector<SD_BattleRoyalData> mBattleRoyalData;
    std::vector<SD_StatisticValue> mStatisticValue1;
    std::vector<SD_StatisticValue> mStatisticValue2;
    std::vector<SD_StatisticValue> mStatisticValue3;
    std::vector<UsedPassword> mUsedPassword;

    static VectorStructType sVectorTypes[8];
};

class SaveData : public VectorStructs {
public:
    struct Data {
        u8 mCurrentSlot;
        u8 mOption;
        u32 mSlotDigests[3][5];
    };

    SaveData() { ClearData(); }

    static u16 GetDataSize() { return sizeof(Data); }
    void ClearData() { memset(&mData, 0, sizeof(mData)); }

    virtual std::vector<SlotData>* GetSlotData() { return &mSlotData; }

    Data mData;
    std::vector<SlotData> mSlotData;

    static VectorStructType sVectorTypes[2];
};

SlotData* GetSaveData(int slot);
int Load(bool backup);
bool Save(int mode);
bool CheckNandSpace();
bool IsGameFlagSet(int flag);
void DeleteSaveFiles();
void SetFlag(int flag, int value);
void ResetSlot(int slot);
int GetCurrentSlot();
void SetCurrentSlot(u8 slot);
u8 GetOption();
void SetOption(u8 option);

#endif
