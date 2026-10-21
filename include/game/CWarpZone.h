#ifndef GAME_CWARPZONE_H
#define GAME_CWARPZONE_H

#include <game/CItem.h>

class CWarpZone : public CItem {
public:
    enum { TYPE = 1 << 12 };
    explicit CWarpZone(u32 type);
    virtual ~CWarpZone();
private:
    u32 mDestination[3];
};

#endif
