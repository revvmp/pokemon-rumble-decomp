#ifndef GAME_CREMOTEMEMORY_H
#define GAME_CREMOTEMEMORY_H

#include <revolution/types.h>

class COyaInfo;

class CRemoteMemory {
public:
    CRemoteMemory(int channel, COyaInfo* pOyaInfo);
    ~CRemoteMemory();
    void WaitUntilIdle(bool flag);
    void Update();

private:
    u8 mStorage[0x40];
};

#endif
