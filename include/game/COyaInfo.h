#ifndef GAME_COYAINFO_H
#define GAME_COYAINFO_H

#include <revolution/types.h>

class COyaInfo {
public:
    COyaInfo();
    ~COyaInfo();

    void SetCount(u32 count);
    u32 GetCount() const { return mCount; }

private:
    u32 mStorage[4];
    u32 mCount;
};

#endif
