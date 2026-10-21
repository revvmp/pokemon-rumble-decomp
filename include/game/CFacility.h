#ifndef GAME_CFACILITY_H
#define GAME_CFACILITY_H

#include <game/CItem.h>

class CFacility : public CItem {
public:
    enum { TYPE = 1 << 13 };
    explicit CFacility(u32 type);
    virtual ~CFacility();
    virtual void SetEffectEnable(bool enable) {
        if (enable) {
            mFlags |= 1;
        } else {
            mFlags &= ~1;
        }
    }
    virtual CMiniMapMarker* GetEffect() const {
        return (mFlags & 1) != 0 ? mMiniMapMarker.get() : nullptr;
    }
    virtual void Initialize(bool background);
private:
    u32 mFlags;
    std::tr1::shared_ptr<CMiniMapMarker> mMiniMapMarker;
};

#endif
