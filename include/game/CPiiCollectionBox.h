#ifndef GAME_CPIICOLLECTIONBOX_H
#define GAME_CPIICOLLECTIONBOX_H

#include <game/SaveData.h>
#include <memory>
#include <vector>

class CPiiCollectionBox {
public:
    static std::tr1::shared_ptr<CPiiCollectionBox> Create(const std::vector<SD_PiiCollectionData::Data>& rData);
    int GetCollectionType(u16 monsno);
<<<<<<< HEAD
    std::vector<u16> GetManyDefeatedPii();
=======
    void SaveSnapshot();
    void RestoreSnapshot();
    void RecordCollection(u16 monsno, u8 form, int type, bool rare);
>>>>>>> 042cc7fd4 (CDlgPiiList: reconstruct Setup, selection/accept handlers, remote transfer and sell flows)
    void ExportCollectionData(std::vector<SD_PiiCollectionData::Data>& rOut);

private:
    struct Entry {
        int mCollectionType;
        int mBestBRR[3];
        int mDefeatedCount;
        u32 mCollectedForms[2];
    };

    std::vector<Entry> mEntries;
    std::vector<Entry> mSnapshot;
};

#endif
