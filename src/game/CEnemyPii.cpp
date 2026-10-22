#include <game/CEnemyPii.h>
#include <game/CHPBar.h>
#include <game/CMiniMapMarker.h>

CEnemyPii::CEnemyPii(std::tr1::shared_ptr<CPiiPersonalData> personalData, u32 type)
    : CPii(personalData, type | TYPE) {}

CEnemyPii::~CEnemyPii() {}

void CEnemyPii::Initialize(bool background)
{
    if (!GetHPBar()) {
        std::tr1::shared_ptr<CHPBar> hpBar(new CHPBar(1));
        hpBar->Initialize(-1, GetPersonalData().get(), background);
        SetHPBar(hpBar);
    }
    if (GetEffect() == nullptr) {
        std::tr1::shared_ptr<CMiniMapMarker> marker(new CMiniMapMarker);
        marker->Initialize(1, "minimap_enemy", background);
        SetMiniMapMarker(marker);
    }
    CPii::Initialize(background);
}

bool CEnemyPii::LoadScript(int mode,
                         const std::tr1::shared_ptr<CPpuEnv>& rEnvironment)
{
    return CPii::LoadScript("CEnemyPii.pkc", mode, rEnvironment);
}