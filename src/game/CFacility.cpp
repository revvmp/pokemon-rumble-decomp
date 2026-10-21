#include <game/CFacility.h>
#include <game/CMiniMapMarker.h>

CFacility::CFacility(u32 type) : CItem(type | TYPE), mFlags(0) {}

CFacility::~CFacility() {}

void CFacility::Initialize(bool background)
{
    if (GetEffect() == nullptr) {
        std::tr1::shared_ptr<CMiniMapMarker> marker(new CMiniMapMarker);
        marker->Initialize(4, "minimap_facility", background);
        SetEffectEnable(true);
        mMiniMapMarker = marker;
    }
    CItem::Initialize(background);
}
