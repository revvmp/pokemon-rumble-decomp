#ifndef GAME_CWARPZONE_H
#define GAME_CWARPZONE_H

#include <game/CItem.h>

class CMiniMapMarker;

class CWarpZone : public CItem {
public:
    enum { TYPE = 1 << 12 };
    explicit CWarpZone(u32 type);
    virtual ~CWarpZone();
    virtual CMiniMapMarker* GetEffect() const;
    virtual void SetEffectEnable(bool enable);
    virtual void Initialize(bool background);

private:
    u32 mEffectFlags;
    std::tr1::shared_ptr<CMiniMapMarker> mMarker;
};

#endif
