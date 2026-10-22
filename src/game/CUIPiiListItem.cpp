#include <game/CUIPiiListItem.h>
#include <game/IGameSession.h>
#include <game/NPpuVM.h>
#include <game/CLayout.h>
#include <game/IColorConst.h>
#include <game/StringFormat.h>
#include <nw4r/lyt/lyt_pane.h>

CUIPiiListItem::CUIPiiListItem(const std::tr1::shared_ptr<CPiiPersonalData>& rPersonalData, int player)
    : mFlags(1), mPPD(rPersonalData), mPlayer(player), mSpeciesTypeMask(0), mMoveTypeMask(0)
{
    const PiiSpeciesData* pSpecies = rPersonalData->GetSpeciesData();
    mSpeciesTypeMask |= 1 << pSpecies->GetPrimaryType();
    mSpeciesTypeMask |= 1 << pSpecies->GetSecondaryType();
    WazaDescData* pWazaData = IGameSession::GetInstance()->GetWazaDescData();
    for (int i = 0; i < 2; ++i) {
        int waza = rPersonalData->GetWazaID(i);
        if (waza != 0) {
            mMoveTypeMask |= 1 << pWazaData->GetCell(waza, 2)->ToInt();
        }
    }
    if (rPersonalData->IsInWiiRemote()) {
        mFlags |= FLAG_REMOTE_MARKED;
    }
}

CUIPiiListItem::~CUIPiiListItem() {}

void CUIPiiListItem::SetSellMarked(bool marked)
{
    if (IsSelectable()) {
        if (marked) mFlags |= FLAG_SELL_MARKED;
        else mFlags &= ~FLAG_SELL_MARKED;
    }
}

void CUIPiiListItem::SetRemoteMarked(bool marked)
{
    if (IsSelectable()) {
        if (marked) mFlags |= FLAG_REMOTE_MARKED;
        else mFlags &= ~FLAG_REMOTE_MARKED;
    }
}

bool CUIPiiListItem::IsSelectable() const
{
    return (mFlags & 1) != 0;
}

void CUIPiiListItem::Update(CUIList* pList, CLayout* pLayout)
{
    pLayout->SetText("NAME", mPPD->GetSpeciesData()->GetName());
    pLayout->SetText("VALUE", FormatString(L"%d", mPPD->GetBP()).c_str());
    GetColorConstants()->ApplyTextColor(pLayout, "NAME", IGameSession::GetInstance()->GetPPDColorIndex(mPlayer, mPPD), 2);
    pLayout->SetPaneVisible("NEW", mPPD->IsSpecHidden());
    nw4r::lyt::Pane* pGreen = pLayout->FindPane("HP_GREEN");
    nw4r::lyt::Pane* pRed = pLayout->FindPane("HP_RED");
    if (pGreen && pRed) {
        pLayout->SetVisible(pGreen, !mPPD->IsSpecHidden());
        pLayout->SetVisible(pRed, !mPPD->IsSpecHidden());
        nw4r::lyt::Size size = pRed->GetSize();
        size.width *= mPPD->GetHPRatio();
        pGreen->SetSize(size);
    }
    pLayout->SetPaneVisible("FAVORITE", mPPD->IsFavorite());
    pLayout->SetPaneVisible("IN_WIIREMOTE", (mFlags & 4) != 0);
    pLayout->SetPaneVisible("CHECK", (mFlags & 2) != 0);
}
