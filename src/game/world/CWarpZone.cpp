#include <game/world/CWarpZone.h>
#include <game/ui/CMiniMapMarker.h>

CWarpZone::CWarpZone(u32 type) : CItem(type | 0x1000), mEffectFlags(0) {}

CWarpZone::~CWarpZone() {}

void CWarpZone::Initialize(bool background) {
    if (GetEffect() == nullptr) {
        std::tr1::shared_ptr<CMiniMapMarker> marker(new CMiniMapMarker);
        marker->Initialize(3, "minimap_warp", background);
        SetEffectEnable(true);
        mMarker = marker;
    }
    CItem::Initialize(background);
}

CMiniMapMarker* CWarpZone::GetEffect() const {
    if ((mEffectFlags & 1) != 0) {
        return mMarker.get();
    }
    return nullptr;
}

void CWarpZone::SetEffectEnable(bool enable) {
    if (enable) {
        mEffectFlags |= 1;
    } else {
        mEffectFlags &= ~1;
    }
}
