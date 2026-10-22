#include <game/CPiiCollectionBox.h>
#include <algorithm>

namespace {

const long kMaxDefeatCount = 9999999;

struct PackedCollectionData {
    u32 mCollectionType : 3;
    u32 mBestBRR0 : 3;
    u32 mBestBRR1 : 3;
    u32 mBestBRR2 : 3;
    u8 mHasFirstForm : 1;
    u32 mReserved : 19;
    u32 mDefeatedCount;
    u32 mNormalForms;
    u32 mRareForms;
};

}

void CPiiCollectionBox::RecordCollection(u16 monsno, u8 form, int type, bool rare) {
    Entry* pEntry = &mEntries[monsno];
    if (pEntry->mCollectionType < type) {
        pEntry->mCollectionType = type;
    }
    if (type == 3) {
        if (rare) {
            pEntry->mCollectedForms[1] |= 1 << (form % 32);
        } else {
            pEntry->mCollectedForms[0] |= 1 << (form % 32);
        }
    }
    if (type == 2) {
        pEntry->mDefeatedCount = std::min<long>(pEntry->mDefeatedCount + 1, kMaxDefeatCount);
    }
}

int CPiiCollectionBox::GetCollectionType(u16 monsno) {
    return mEntries[monsno].mCollectionType;
}

u32 CPiiCollectionBox::GetNormalColorMask(u16 monsno) {
    return mEntries[monsno].mCollectedForms[0];
}

u32 CPiiCollectionBox::GetRareColorMask(u16 monsno) {
    return mEntries[monsno].mCollectedForms[1];
}

void CPiiCollectionBox::SetCollectionBRR(u16 monsno, int type, int value) {
    Entry& rEntry = mEntries[monsno];
    if (rEntry.mBestBRR[type] < value) {
        rEntry.mBestBRR[type] = value;
    }
}

int CPiiCollectionBox::GetDefeatCount(u16 monsno, int difficulty) {
    return mEntries[monsno].mBestBRR[difficulty];
}

int CPiiCollectionBox::GetDefeatCount(u16 monsno) {
    return mEntries[monsno].mDefeatedCount;
}

std::tr1::shared_ptr<CPiiCollectionBox> CPiiCollectionBox::Create(const std::vector<SD_PiiCollectionData::Data>& rData) {
    std::tr1::shared_ptr<CPiiCollectionBox> box(new CPiiCollectionBox);
    for (u16 monsno = 1; monsno < 496; monsno++) {
        if (monsno < rData.size()) {
            const SD_PiiCollectionData::Data& rSrc = rData[monsno];
            Entry& rEntry = box->mEntries[monsno];
            rEntry.mCollectionType = rSrc.mValues[0] >> 29;
            rEntry.mBestBRR[0] = (rSrc.mValues[0] >> 26) & 7;
            rEntry.mBestBRR[1] = (rSrc.mValues[0] >> 23) & 7;
            rEntry.mBestBRR[2] = (rSrc.mValues[0] >> 20) & 7;
            rEntry.mDefeatedCount = rSrc.mValues[1];
            rEntry.mCollectedForms[0] = rSrc.mValues[2];
            rEntry.mCollectedForms[1] = rSrc.mValues[3];
            if ((rSrc.mValues[0] >> 19) & 1) {
                rEntry.mCollectedForms[1] |= 1;
            }
        }
    }
    return box;
}

void CPiiCollectionBox::ExportCollectionData(std::vector<SD_PiiCollectionData::Data>& rOut) {
    std::vector<SD_PiiCollectionData::Data> records;
    records.reserve(496);
    PackedCollectionData packed;
    for (u16 monsno = 0; monsno < 496; monsno++) {
        const Entry* pEntry = &mEntries[monsno];
        packed.mCollectionType = pEntry->mCollectionType;
        packed.mBestBRR0 = pEntry->mBestBRR[0];
        packed.mBestBRR1 = pEntry->mBestBRR[1];
        packed.mBestBRR2 = pEntry->mBestBRR[2];
        packed.mDefeatedCount = pEntry->mDefeatedCount;
        packed.mNormalForms = pEntry->mCollectedForms[0];
        packed.mRareForms = pEntry->mCollectedForms[1];
        packed.mHasFirstForm = 0;
        packed.mReserved = 0;
        records.push_back(reinterpret_cast<const SD_PiiCollectionData::Data&>(packed));
    }
    rOut.swap(records);
}

bool CPiiCollectionBox::CompareDefeatCount(const Entry* pLeft, const Entry* pRight) {
    if (pLeft->mDefeatedCount != pRight->mDefeatedCount) {
        return pLeft->mDefeatedCount > pRight->mDefeatedCount;
    }
    return pLeft < pRight;
}

std::vector<u16> CPiiCollectionBox::GetManyDefeatedPii() {
    std::vector<Entry*> entries;
    entries.reserve(mEntries.size());
    for (std::vector<Entry>::iterator it = mEntries.begin(); it != mEntries.end(); ++it) {
        if (it != mEntries.begin()) {
            entries.push_back(&*it);
        }
    }
    std::sort(entries.begin(), entries.end(), CompareDefeatCount);
    std::vector<u16> monsnos;
    monsnos.reserve(entries.size());
    for (std::vector<Entry*>::iterator it = entries.begin(); it != entries.end(); ++it) {
        monsnos.push_back(*it - &mEntries[0]);
    }
    return monsnos;
}

void CPiiCollectionBox::SaveSnapshot() {
    mSnapshot = mEntries;
}

void CPiiCollectionBox::RestoreSnapshot() {
    mEntries = mSnapshot;
}
