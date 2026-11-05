#include <game/ui/CPiiInfoWorldObjStat.h>

#include <game/world/BWorldObj.h>
#include <game/pokemon/CPiiPersonalData.h>
#include <game/text/Font.h>
#include <game/ui/IColorConst.h>
#include <game/session/IGameSession.h>
#include <game/ui/LayoutResources.h>
#include <game/text/Message.h>
#include <game/pokemon/PiiSpeciesData.h>
#include <game/text/StringFormat.h>
#include <nw4r/lyt/lyt_layout.h>
#include <nw4r/lyt/lyt_pane.h>

CPiiInfoWorldObjStat::CPiiInfoWorldObjStat() : mFlags(0) {}

CPiiInfoWorldObjStat::~CPiiInfoWorldObjStat() {}

void CPiiInfoWorldObjStat::Load(std::tr1::shared_ptr<CPiiPersonalData> data, bool background) {
    std::tr1::shared_ptr<CLayout::FontMap> fonts = GetDefaultFontMap();
    mLayout.Load(gSharedLayoutArchives, "wos_piiinfo", fonts, background);
    mLayout.SetText("NEW_c", GetMessageString(0x600F5, L""));

    int species;
    int color;
    if (data->GetPrefix() != 0) {
        mLayout.SetPaneVisible("PREFIX", true);
        mLayout.SetText("PREFIX", data->GetPrefixText());
        bool rare = data->IsRareColor();
        color = 2;
        if (rare) {
            color = 3;
        }
        GetColorConstants()->ApplyTextColor(&mLayout, "PREFIX", color, 2);
    } else {
        color = 0;
        mLayout.SetPaneVisible("PREFIX", false);
    }

    species = data->GetSpeciesData()->GetSpecies();
    mLayout.SetPaneVisible("NEW", IGameSession::GetInstance()->GetCollectionType(species) != 3);
    GetColorConstants()->ApplyTextColor(&mLayout, "NAME", color, 2);
    mLayout.SetText("NAME", data->GetSpeciesData()->GetName());
    mLayout.SetText("BP", FormatString(L"%d", data->GetBP()).c_str());
    mMotion = mLayout.CreateMotion("show");
    mLayout.SetVisible(false);
}

void CPiiInfoWorldObjStat::Update(BWorldObj* pObj) {
    if (pObj != nullptr && pObj->IsVisible()) {
        if (mFlags & FLAG_HAS_POSITION) {
            return;
        }
        mPosition = pObj->GetPositionRef();
        mFlags |= FLAG_HAS_POSITION;
    }
}

void CPiiInfoWorldObjStat::CalcView(const nw4r::math::MTX34& rView, const nw4r::math::MTX44& rProjection) {
    mLayout.SetVisible(false);
    bool hasPosition = mFlags & FLAG_HAS_POSITION;
    mFlags &= ~FLAG_SHOWN;
    if (!hasPosition) {
        return;
    }

    nw4r::math::VEC3 position = mPosition;
    nw4r::math::VEC3Transform(&position, &rView, &position);
    nw4r::math::VEC3TransformCoord(&position, &rProjection, &position);
    if (!(-1.2f < position.x) || !(position.x < 1.2f) || !(-1.2f < position.y) || !(position.y < 1.2f)) {
        return;
    }

    const nw4r::ut::Rect& rRect = mLayout.GetViewRect();
    f32 width = rRect.right - rRect.left;
    position.x = position.x * width / 2.0f;
    f32 height = rRect.bottom - rRect.top;
    position.y = -position.y * height / 2.0f;
    position.z = 0.0f;
    mLayout.GetLayout()->GetRootPane()->SetTranslate(position);
    mLayout.SetVisible(true);
    mLayout.Calc();
    if (mMotion && !mMotion->IsEnded()) {
        mFlags |= FLAG_SHOWN;
    }
}

bool CPiiInfoWorldObjStat::IsVisible() const {
    return mFlags & FLAG_SHOWN;
}

void CPiiInfoWorldObjStat::Draw() {
    mLayout.Draw();
}

int CPiiInfoWorldObjStat::GetType() const {
    return TYPE_PII_INFO;
}
