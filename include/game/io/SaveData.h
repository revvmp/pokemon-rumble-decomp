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
    void SetFlags(u32 flags) { mValue |= flags; }
    void IncrementClearCount() {
        u32 count = (mValue >> 22) + 1;
        if (count < 1024) { mValue = (mValue & 0x3FFFFF) | (count << 22); }
    }

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
        u16 mSpecies : 9;
        u16 mForm : 5;
        u16 mSex : 2;
        u16 mMoveIds[2];
        u16 mLevel;
        u16 mBonusLow[4];
        u16 mPrefix;
        u16 mFlagsLow;
        u32 mRandomSeed;
        u16 mBonusHigh[4];
        u32 mCreationTimeHigh;
        u32 mCreationTimeLow;
        u32 mOriginalTrainerId;

        s64 GetCreationTime() const { return (static_cast<s64>(mCreationTimeHigh) << 32) | mCreationTimeLow; }
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

#if defined(SAVEDATA_EMISSION_LAYOUT)
    static u16 GetDataSize();
    void ClearData();
#else
    static u16 GetDataSize() { return sizeof(Data); }
    void ClearData() { memset(&mData, 0, sizeof(mData)); }
#endif

    Data mData;

    static VectorStructType sVectorTypes[1];
};

class SD_PiiCollectionData : public VectorStructs {
public:
    struct Data {
        u32 mValues[4];
    };

    SD_PiiCollectionData() { ClearData(); }

#if defined(SAVEDATA_EMISSION_LAYOUT)
    static u16 GetDataSize();
    void ClearData();
#else
    static u16 GetDataSize() { return sizeof(Data); }
    void ClearData() { memset(&mData, 0, sizeof(mData)); }
#endif

    Data mData;

    static VectorStructType sVectorTypes[1];
};

#if defined(CGAMESESSION_EMISSION_LAYOUT)
typedef std::__vec_deleter<SD_PiiPersonalData::Data, std::allocator<SD_PiiPersonalData::Data> > SnapshotCleanup0;
typedef std::__vec_deleter<SD_StatisticValue::Data, std::allocator<SD_StatisticValue::Data> > SnapshotCleanup1;
typedef std::__vec_deleter<SD_StatisticValue, std::allocator<SD_StatisticValue> > SnapshotCleanup2;
typedef std::__vec_deleter<SD_BattleRoyalData, std::allocator<SD_BattleRoyalData> > SnapshotCleanup3;
typedef std::__vec_deleter<SD_PiiCollectionData, std::allocator<SD_PiiCollectionData> > SnapshotCleanup4;
typedef std::__vec_deleter<SD_PiiPersonalData, std::allocator<SD_PiiPersonalData> > SnapshotCleanup5;
typedef std::__vec_deleter<SD_PiiCollectionData::Data, std::allocator<SD_PiiCollectionData::Data> > SnapshotCleanup6;

typedef std::__vec_split_buffer<SD_PiiPersonalData::Data, std::allocator<SD_PiiPersonalData::Data> > SnapshotSplit0;
typedef std::__vec_split_buffer<SD_StatisticValue::Data, std::allocator<SD_StatisticValue::Data> > SnapshotSplit1;
typedef std::__vec_split_buffer<SD_StatisticValue, std::allocator<SD_StatisticValue> > SnapshotSplit2;
typedef std::__vec_split_buffer<SD_BattleRoyalData, std::allocator<SD_BattleRoyalData> > SnapshotSplit3;
typedef std::__vec_split_buffer<SD_PiiCollectionData, std::allocator<SD_PiiCollectionData> > SnapshotSplit4;
typedef std::__vec_split_buffer<SD_PiiPersonalData, std::allocator<SD_PiiPersonalData> > SnapshotSplit5;
typedef std::__vec_split_buffer<SD_PiiCollectionData::Data, std::allocator<SD_PiiCollectionData::Data> > SnapshotSplit6;
#endif

class UsedPassword : public VectorStructs {
public:
    struct Data {
        int mPassword;
    };

    UsedPassword() { ClearData(); }

#if defined(SAVEDATA_EMISSION_LAYOUT)
    static u16 GetDataSize();
    void ClearData();
#else
    static u16 GetDataSize() { return sizeof(Data); }
    void ClearData() { memset(&mData, 0, sizeof(mData)); }
#endif

    Data mData;

    static VectorStructType sVectorTypes[1];
};

struct SlotDataCopyTag {
    SlotDataCopyTag() {}
    SlotDataCopyTag(SlotDataCopyTag&) {}
};

class SlotData : public VectorStructs {
public:
    struct Data : SlotDataCopyTag {
        u32 mHeader[3];
        s8 mRank;
        s8 mCollectionLevel;
        s8 mArea;
        s8 mMode;
        CMapRecord mMapRecords[3][5][8][10];
        u32 mCounterFirst;
        u32 mCounterSecond;
        s64 mTime;
        u32 mRegistryCount;
        u32 mElapsedSeconds;
        u16 mTicketMonsno;
        u8 mTicketFormno;
        u8 mReserved;
        u32 mWirelessMacHigh;
        u16 mWirelessMacLow;
        s8 mExtraBRMaxRound;
        s8 mExtraBRCurrentRound;
        u32 mLeadPiiIdHigh;
        u16 mLeadPiiMonsno;
        s8 mNormalArea;
        s8 mExtraArea;
        u32 mLeadPiiIdLow;
    };

    SlotData() { ClearData(); }
    SlotData(const SlotData& other);

#if defined(SAVEDATA_EMISSION_LAYOUT)
    static u16 GetDataSize();
    void ClearData();
#else
    static u16 GetDataSize() { return sizeof(Data); }
    void ClearData() { memset(&mData, 0, sizeof(mData)); }
#endif

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

#if defined(SAVEDATA_EMISSION_LAYOUT)
    static u16 GetDataSize();
    void ClearData();
#else
    static u16 GetDataSize() { return sizeof(Data); }
    void ClearData() { memset(&mData, 0, sizeof(mData)); }
#endif

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
void SetCurrentSlot(int slot);
int GetOption();
void SetOption(int option);


namespace std {
template <typename Allocator>
class __vec_constructor<SD_PiiPersonalData::Data, Allocator, false> : public __vec_constructor<SD_PiiPersonalData::Data, Allocator, true> {
    typedef __vec_constructor<SD_PiiPersonalData::Data, Allocator, true> base;
public:
    __vec_constructor() {}
    explicit __vec_constructor(typename base::size_type n) : base(n) {}
    __vec_constructor(const __vec_constructor& rOther) : base(rOther) {}
    template <typename Iterator> __vec_constructor(Iterator first, Iterator last) : base(first, last) {}
};
}
#endif
