#include <game/CUIType.h>
#include <game/IColorConst.h>
#include <game/LayoutResources.h>
#include <game/Message.h>

CUIType::CUIType() {}

CUIType::~CUIType() {}

void CUIType::Initialize(int background) {
    Load(gSharedLayoutArchives, "type", GetDefaultFontMap(), background);
}

void CUIType::SetType(int type) {
    GetLayout()->SetText("TYPE", GetMessageString(type + 0x40000, nullptr));
    nw4r::lyt::Pane* pPane = GetLayout()->FindPane("COLOR");
    GetColorConstants()->ApplyVertexColor(pPane, type + 11, 4);
}
