#include <game/CDropPii.h>
#include <game/CHPBar.h>
#include <game/CMiniMapMarker.h>

CDropPii::CDropPii(std::tr1::shared_ptr<CPiiPersonalData> personalData, u32 type)
    : CPii(personalData, type | TYPE) {}

CDropPii::~CDropPii() {}

void CDropPii::Initialize(bool background)
{
    if (!GetHPBar()) {
        std::tr1::shared_ptr<CHPBar> hpBar(new CHPBar(1));
        hpBar->Initialize(-1, GetPersonalData().get(), background);
        SetHPBar(hpBar);
    }
    if (GetEffect() == nullptr) {
        std::tr1::shared_ptr<CMiniMapMarker> marker(new CMiniMapMarker);
        marker->Initialize(0, "minimap_drop", background);
        SetMiniMapMarker(marker);
    }
    CPii::Initialize(background);
}
