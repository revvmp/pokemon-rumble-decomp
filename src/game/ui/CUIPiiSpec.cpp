#include <game/ui/CUIPiiSpec.h>

#include <game/pokemon/COyaInfo.h>
#include <game/pokemon/CPiiTex.h>
#include <game/session/CPlayer.h>
#include <game/ui/CUIStar.h>
#include <game/ui/CUIType.h>
#include <game/ui/IColorConst.h>
#include <game/input/IController.h>
#include <game/session/IGameSession.h>
#include <game/ui/IIconCollection.h>
#include <game/text/Message.h>
#include <game/script/NPpuVM.h>
#include <game/pokemon/PiiPropData.h>
#include <game/text/StringFormat.h>
#include <nw4r/lyt/lyt_pane.h>

namespace {

int GetStarRank(int value) {
    if (value >= 153) {
        return 5;
    }
    if (value >= 134) {
        return 4;
    }
    if (value >= 100) {
        return 3;
    }
    if (value >= 70) {
        return 2;
    }
    if (value >= 1) {
        return 1;
    }
    return 0;
}

int GetDefenseStarRank(int value) {
    if (value >= 130) {
        return 5;
    }
    if (value >= 110) {
        return 4;
    }
    if (value >= 83) {
        return 3;
    }
    if (value >= 59) {
        return 2;
    }
    if (value >= 1) {
        return 1;
    }
    return 0;
}

}

CUIPiiSpec::CUIPiiSpec() : mPiiTexDelay(0) {
    for (int i = 0; i < 2; ++i) {
        mSpeciesTypes[i].reset(new CUIType);
    }
    for (int i = 0; i < 2; ++i) {
        mWazaTypes[i].reset(new CUIType);
        mWazaStars[i].reset(new CUIStar);
    }
}

CUIPiiSpec::~CUIPiiSpec() {}

void CUIPiiSpec::Load(const char* const* pArchives, const char* pLayoutName, const std::tr1::shared_ptr<CLayout::FontMap>& rFonts, int background) {
    BUILayout::Load(pArchives, pLayoutName, rFonts, background);
    SetText("BP_TITLE", GetMessageString(0x600C0, nullptr));
    SetText("HP_TITLE", GetMessageString(0x600C1, nullptr));
    SetText("prefix_title", GetMessageString(0x600C2, nullptr));
    SetText("AP_TITLE", GetMessageString(0x600C3, nullptr));
    SetText("DP_TITLE", GetMessageString(0x600C4, nullptr));
    mSpeciesTypes[0]->Initialize(background);
    mSpeciesTypes[1]->Initialize(background);
    mWazaTypes[0]->Initialize(background);
    mWazaTypes[1]->Initialize(background);
    mWazaStars[0]->Load(pArchives, "star", rFonts, background);
    mWazaStars[1]->Load(pArchives, "star", rFonts, background);
    AttachChild("TYPE0", mSpeciesTypes[0]);
    AttachChild("TYPE1", mSpeciesTypes[1]);
    AttachChild("WAZA0_TYPE", mWazaTypes[0]);
    AttachChild("WAZA1_TYPE", mWazaTypes[1]);
    AttachChild("WAZA0_STAR", mWazaStars[0]);
    AttachChild("WAZA1_STAR", mWazaStars[1]);
    if (GetLayout()->FindPane("AP_STAR")) {
        mApStar.reset(new CUIStar);
        mApStar->Load(pArchives, "apdp", rFonts, background);
        AttachChild("AP_STAR", mApStar);
    }
    if (GetLayout()->FindPane("DP_STAR")) {
        mDpStar.reset(new CUIStar);
        mDpStar->Load(pArchives, "apdp", rFonts, background);
        AttachChild("DP_STAR", mDpStar);
    }
    GetLayout()->SetPaneVisible("PII_TEX", false);
}

void CUIPiiSpec::SetPii(std::tr1::shared_ptr<CPiiPersonalData> pii, int player) {
    WazaDescData* pWazaData = IGameSession::GetInstance()->GetWazaDescData();
    CPlayer* pPlayer = IGameSession::GetInstance()->GetPlayer(player);
    IController* pController = pPlayer->GetController();
    GetLayout()->SetPaneVisible("PREFIX", pii->GetPrefix() != 0);
    GetLayout()->SetPaneVisible("PREFIX_NAME", pii->GetPrefix() != 0);
    GetLayout()->SetPaneVisible("NAME", pii->GetPrefix() == 0);
    GetLayout()->SetPaneVisible("PREFIX_TEXT", pii->GetPrefix() != 0);
    int colorIndex = IGameSession::GetInstance()->AddPPDToPlayerPiiBox(player, std::tr1::shared_ptr<CPiiPersonalData>(pii));
    if (pii->GetPrefix() != 0) {
        GetLayout()->SetText("PREFIX", pii->GetPrefixText());
        GetLayout()->SetText("PREFIX_NAME", pii->GetSpeciesData()->GetName());
        GetColorConstants()->ApplyTextColor(GetLayout(), "PREFIX", colorIndex, 2);
        GetColorConstants()->ApplyTextColor(GetLayout(), "PREFIX_NAME", colorIndex, 2);
        GetLayout()->SetText("PREFIX_DESC", GetMessageString(0x60049 + pii->GetPrefix(), nullptr));
    } else {
        GetLayout()->SetText("NAME", pii->GetSpeciesData()->GetName());
        GetColorConstants()->ApplyTextColor(GetLayout(), "NAME", colorIndex, 2);
    }
    GetLayout()->SetText("BP", FormatString(L"%d", pii->GetBP()).c_str());
    if (IGameSession::GetInstance()->GetOyaInfo()->GetCount() != pii->GetOriginalTrainerId()) {
        GetLayout()->SetPaneVisible("CAP", pii->GetCappedLevel() < pii->GetLevel());
        GetLayout()->SetPaneVisible("CAP_OK", pii->GetCappedLevel() >= pii->GetLevel());
    } else {
        GetLayout()->SetPaneVisible("CAP", false);
        GetLayout()->SetPaneVisible("CAP_OK", false);
    }
    int primaryType = pii->GetSpeciesData()->GetPrimaryType();
    int secondaryType = pii->GetSpeciesData()->GetSecondaryType();
    mSpeciesTypes[0]->SetType(primaryType);
    mSpeciesTypes[0]->SetVisible(true);
    mSpeciesTypes[1]->SetType(secondaryType);
    mSpeciesTypes[1]->SetVisible(primaryType != secondaryType);
    GetLayout()->SetText("HP_CUR", FormatString(L"%d", pii->GetHP()).c_str());
    GetLayout()->SetText("HP_MAX", FormatString(L"%d", pii->GetMaxHP()).c_str());
    nw4r::lyt::Pane* pHpGreen = GetLayout()->FindPane("HP_GREEN");
    nw4r::lyt::Pane* pHpRed = GetLayout()->FindPane("HP_RED");
    if (pHpGreen && pHpRed) {
        nw4r::lyt::Size size = pHpRed->GetSize();
        size.width *= pii->GetHPRatio();
        pHpGreen->SetSize(size);
    }
    if (mApStar) {
        const PiiSpeciesData* pSpecies = pii->GetSpeciesData();
        int form = pSpecies->GetFormNumber();
        int species = pSpecies->GetSpecies();
        mApStar->SetStars(GetStarRank(IGameSession::GetInstance()->GetPiiPropData()->GetProperty(species, form, 0).ToInt()), false);
    }
    if (mDpStar) {
        const PiiSpeciesData* pSpecies = pii->GetSpeciesData();
        int form = pSpecies->GetFormNumber();
        int species = pSpecies->GetSpecies();
        mDpStar->SetStars(GetDefenseStarRank(IGameSession::GetInstance()->GetPiiPropData()->GetProperty(species, form, 1).ToInt()), false);
    }
    for (int i = 0; i < 2; ++i) {
        int waza = pii->GetWazaID(i);
        bool hasWaza = waza != 0;
        GetLayout()->SetPaneVisible(FormatString("WAZA%d", i).c_str(), hasWaza);
        if (hasWaza) {
            GetLayout()->SetText(FormatString("WAZA%d_NAME", i).c_str(), IGameSession::GetInstance()->GetWazaName(waza).c_str());
            int type = pWazaData->GetCell(waza, 2)->ToInt();
            mWazaTypes[i]->SetType(type);
            int power = pWazaData->GetCell(waza, 9)->ToInt();
            mWazaStars[i]->SetStars(power, type == primaryType || type == secondaryType);
            GetLayout()->SetText(FormatString("WAZA%d_DESC", i).c_str(), GetMessageString(pWazaData->GetCell(waza, 10)->ToInt(), nullptr));
            if (pController) {
                GetLayout()->SetTexture(FormatString("WAZA%d_BUTTON", i).c_str(), GetIconCollection()->GetButtonIcon(pController, i == 0 ? 0x20 : 0x10), -1);
            }
        }
    }
    if (mPii != pii) {
        mPiiTex.reset();
        nw4r::lyt::Pane* pPane = GetLayout()->FindPane("PII_TEX");
        if (pPane) {
            GetLayout()->SetVisible(pPane, false);
            mPii = pii;
            mPiiTexDelay = 20;
        }
    }
}

void CUIPiiSpec::Calc() {
    BUILayout::Calc();
    if (!mPiiTex) {
        if (mPii) {
            if (mPiiTexDelay <= 0) {
                GetLayout()->SetPaneVisible("PII_TEX", true);
                mPiiTex.reset(new CPiiTex(mPii, 256, 256));
                mPiiTex->Calc();
                GetLayout()->SetTexture("PII_TEX", mPiiTex->GetTexMap(), -1);
            } else {
                --mPiiTexDelay;
            }
        }
    } else {
        mPiiTex->Calc();
    }
}

void CUIPiiSpec::CalcMtx() {
    BUILayout::CalcMtx();
    if (mPiiTex) {
        mPiiTex->CalcMtx();
    }
}

void CUIPiiSpec::CalcView() {
    BUILayout::CalcView();
    if (mPiiTex) {
        mPiiTex->CalcView();
    }
}
