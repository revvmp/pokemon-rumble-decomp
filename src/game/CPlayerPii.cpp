#pragma dont_inline on

#include <game/CPlayerPii.h>
#include <game/CHPBar.h>
#include <game/CMiniMapMarker.h>
#include <game/CPlayer.h>
#include <game/IGameMap.h>
#include <game/CPpuEnv.h>

CPlayerPii::CPlayerPii(CPlayer* pPlayer, std::tr1::shared_ptr<CPiiPersonalData> personalData, u32 type)
    : CPii(personalData, type | TYPE), mpPlayer(pPlayer) {}

CPlayerPii::~CPlayerPii() {}

void CPlayerPii::Initialize(bool background)
{
    mpPlayer->SetPii(this);
    SetSizeMode(1);
    CPii::Initialize(background);
    std::tr1::shared_ptr<CHPBar> hpBar(new CHPBar(5 - mpPlayer->GetPlayerNo()));
    hpBar->Initialize(mpPlayer->GetPlayerNo(), GetPersonalData().get(), background);
    SetHPBar(hpBar);
    std::tr1::shared_ptr<CMiniMapMarker> marker(new CMiniMapMarker);
    marker->Initialize(5, "minimap_player", background);
    SetMiniMapMarker(marker);
}

bool CPlayerPii::LoadScript(int mode, const std::tr1::shared_ptr<CPpuEnv>& rEnvironment)
{
    SetEnvironment(rEnvironment.get());
    return CPii::LoadScript("CPlayerPii.pkc", mode, rEnvironment);
}

void CPlayerPii::Calc()
{
    CPii::Calc();
    if (mPositionHistory.size() == 120) {
        mPositionHistory.pop_front();
    }
    mPositionHistory.push_back(GetPositionRef());
}

const char* CPlayerPii::GetShadowName() const
{
    struct ShadowNames {
        const char* mNames[4];
        ShadowNames& operator=(const ShadowNames&);
    };
    enum ShadowAddress {
        SHADOW_ADDRESS_MIN = 0,
        SHADOW_ADDRESS_MAX = 0xFFFFFFFF
    };
    union {
        const CPlayerPii* pObject;
        ShadowAddress value;
    } address;

    address.pObject = this;
    address.value = static_cast<ShadowAddress>(reinterpret_cast<u32>(address.pObject));
    CPlayer* pPlayer = reinterpret_cast<const CPlayerPii*>(address.value)->mpPlayer;
    const ShadowNames names = {{"5:/misc.arc/p1.brres", "5:/misc.arc/p2.brres", "5:/misc.arc/p3.brres", "5:/misc.arc/p4.brres"}};
    return names.mNames[pPlayer->GetPlayerNo()];
}

bool CPlayerPii::IsVisible() const
{
    if (BWorldObj::IsVisible()) {
        IGameMap* pMap = GetActiveGameMap();
        if (pMap && !pMap->IsPausing()) {
            return true;
        }
    }
    return false;
}
