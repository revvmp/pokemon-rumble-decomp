#include <game/CPiiCollectionBox.h>
#include <algorithm>

namespace {

const long kMaxDefeatCount = 9999999;

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
