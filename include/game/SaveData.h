#ifndef GAME_SAVEDATA_H
#define GAME_SAVEDATA_H

#include <revolution/types.h>

class CMapRecord {
public:
    bool IsFlagSet(u32 flags) const { return (mValue & flags) == flags; }
    int GetClearCount() const { return mValue >> 22; }

private:
    u32 mValue;
};

struct SaveData {
    u8 mHeader[0x18];
    CMapRecord mMapRecords[3][5][8][10];
};

SaveData* GetSaveData(int slot);

#endif
