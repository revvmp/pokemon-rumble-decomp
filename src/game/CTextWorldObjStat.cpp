#include <game/CTextWorldObjStat.h>

#include <game/BWorldObj.h>
#include <game/LayoutResources.h>
#include <nw4r/lyt/lyt_layout.h>
#include <nw4r/lyt/lyt_pane.h>

static const char* sLayoutNames[CTextWorldObjStat::KIND_COUNT] = {"wos_money", "wos_spstat", "wos_chit", "wos_time"};

CTextWorldObjStat::CTextWorldObjStat() : mFlags(0), mType(TYPE_BASE) {}

CTextWorldObjStat::~CTextWorldObjStat() {}

void CTextWorldObjStat::Load(int kind, const wchar_t* pText, bool background) {
    std::tr1::shared_ptr<CLayout::FontMap> fonts = GetDefaultFontMap();
    mLayout.Load(gSharedLayoutArchives, sLayoutNames[kind], fonts, background);
    mLayout.SetText("TEXT", pText);
    mMotion = mLayout.CreateMotion("show");
    mLayout.SetVisible(false);
    mType = TYPE_BASE + kind;
}

void CTextWorldObjStat::Update(BWorldObj* pObj) {
    if (pObj != nullptr && pObj->IsVisible()) {
        mPosition = pObj->GetPositionRef();
        mFlags |= FLAG_HAS_POSITION;
    }
}

void CTextWorldObjStat::CalcView(const nw4r::math::MTX34& rView, const nw4r::math::MTX44& rProjection) {
    mLayout.SetVisible(false);
    bool hasPosition = mFlags & FLAG_HAS_POSITION;
    mFlags &= ~FLAG_SHOWN;
    if (!hasPosition) {
        return;
    }

    nw4r::math::VEC3 position = mPosition;
    nw4r::math::VEC3Transform(&position, &rView, &position);
    nw4r::math::VEC3TransformCoord(&position, &rProjection, &position);
    if (!(-1.0f < position.x && position.x < 1.0f && -1.0f < position.y && position.y < 1.0f)) {
        return;
    }

    const nw4r::ut::Rect& rRect = mLayout.GetViewRect();
    position.x *= rRect.right - rRect.left;
    position.x *= 0.5f;
    position.y = -position.y * (rRect.bottom - rRect.top);
    position.y *= 0.5f;
    position.z = 0.0f;
    mLayout.GetLayout()->GetRootPane()->SetTranslate(position);
    mLayout.SetVisible(true);
    mLayout.Calc();
    if (mMotion && !mMotion->IsEnded()) {
        mFlags |= FLAG_SHOWN;
    }
}

bool CTextWorldObjStat::IsVisible() const {
    return mFlags & FLAG_SHOWN;
}

void CTextWorldObjStat::Draw() {
    mLayout.Draw();
}

int CTextWorldObjStat::GetType() const {
    return mType;
}
