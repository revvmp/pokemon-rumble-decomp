#include <game/pokemon/CBossPii.h>
#include <game/ui/CHPBar.h>
#include <game/ui/CMiniMapMarker.h>

CBossPii::CBossPii(std::tr1::shared_ptr<CPiiPersonalData> personalData, u32 type)
    : CEnemyPii(personalData, type | TYPE) {}
CBossPii::~CBossPii() {}
void CBossPii::Initialize(bool background)
{
    if (!GetHPBar()) {
        std::tr1::shared_ptr<CHPBar> hpBar(new CHPBar(0x6A));
        hpBar->Initialize(-1, GetPersonalData().get(), background);
        SetHPBar(hpBar);
    }
    if (GetEffect() == nullptr) {
        std::tr1::shared_ptr<CMiniMapMarker> marker(new CMiniMapMarker);
        marker->Initialize(2, "minimap_boss", background);
        SetMiniMapMarker(marker);
    }
    CEnemyPii::Initialize(background);
}
