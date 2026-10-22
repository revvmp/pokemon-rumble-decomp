#ifndef GAME_CPIICOLLECTIONBOX_H
#define GAME_CPIICOLLECTIONBOX_H

#include <game/SaveData.h>
#include <memory>
#include <vector>

class CPiiCollectionBox {
public:
    CPiiCollectionBox() { mEntries.resize(496); }
    static std::tr1::shared_ptr<CPiiCollectionBox> Create(const std::vector<SD_PiiCollectionData::Data>& rData);
    int GetCollectionType(u16 monsno);
    void SetCollectionBRR(u16 monsno, int type, int value);
    int GetDefeatCount(u16 monsno);
    std::vector<u16> GetManyDefeatedPii();
    void SaveSnapshot();
    void RestoreSnapshot();
    void RecordCollection(u16 monsno, u8 form, int type, bool rare);
    int GetDefeatCount(u16 monsno, int difficulty);
    u32 GetNormalColorMask(u16 monsno);
    u32 GetRareColorMask(u16 monsno);
    void ExportCollectionData(std::vector<SD_PiiCollectionData::Data>& rOut);

private:
    struct Entry {
        Entry() : mCollectionType(0), mDefeatedCount(0), mNormalForms(0), mRareForms(0) {
            for (int i = 0; i < 3; ++i) {
                mBestBRR[i] = 0;
            }
        }

        int mCollectionType;
        int mBestBRR[3];
        int mDefeatedCount;
        u32 mNormalForms;
        u32 mRareForms;
    };

    static bool CompareDefeatCount(const Entry* pLeft, const Entry* pRight);

    std::vector<Entry> mEntries;
    std::vector<Entry> mSnapshot;
};

#endif
