#include <game/CUIPlayerID.h>
#include <game/CPlayer.h>
#include <game/CRemoteMemory.h>
#include <game/IIconCollection.h>
#include <game/IGameSession.h>
#include <game/Message.h>
#include <game/StringFormat.h>
CUIPlayerID::CUIPlayerID() : mDisplayMode(0), mPlayerNo(-1) {}
CUIPlayerID::~CUIPlayerID() {}
void CUIPlayerID::Setup(int player, int mode, const char* const* pArchives, const char* pLayoutName, const std::tr1::shared_ptr<CLayout::FontMap>& rFonts, bool background) {
    Load(pArchives, pLayoutName, rFonts, background);
    for (int i = 0; i < 4; ++i) {
        std::string name = FormatString("BK_P%d", i);
        GetLayout()->SetPaneVisible(name.c_str(), i == player);
    }
    GetLayout()->SetText("PLAYER", GetMessageString(0x10042 + player, gEmptyText));
    GetLayout()->SetText("LOADING", GetMessageString(0x60079, gEmptyText));
    mPlayerNo = player;
    mDisplayMode = mode;
}
void CUIPlayerID::Update() {
    if (mPlayerNo < 0) return;
    CPlayer* pPlayer = IGameSession::GetInstance()->GetPlayer(mPlayerNo);
    IController* pController = pPlayer->GetController();
    if (!pController) return;
    CLayout* pLayout = GetLayout();
    pLayout->SetPaneVisible("ENTRY", false);
    pLayout->SetPaneVisible("PLAYER", false);
    pLayout->SetPaneVisible("LOADING", false);
    pLayout->SetTexture("CT_ICON", GetIconCollection()->GetControllerIcon(pController), -1);
    if (pController->GetType() == IController::TYPE_WII_REMOTE && IGameSession::GetInstance()->GetRemoteMemory(pController->GetChannel())->IsBusy()) {
        pLayout->SetPaneVisible("LOADING", true);
    } else if (pPlayer->IsJoined()) {
        if (mDisplayMode == 0) {
            pLayout->SetPaneVisible("PLAYER", true);
        } else if (mDisplayMode == 2) {
            pLayout->SetTexture("ENTRY_BUTTON", GetIconCollection()->GetButtonIcon(pController, 32), -1);
            pLayout->SetText("ENTRY", GetMessageString(0x60078, gEmptyText));
            pLayout->SetPaneVisible("ENTRY", true);
        } else {
            pLayout->SetTexture("ENTRY_BUTTON", GetIconCollection()->GetButtonIcon(pController, 16), -1);
            pLayout->SetText("ENTRY", GetMessageString(0x60077, gEmptyText));
            pLayout->SetPaneVisible("ENTRY", true);
        }
    } else {
        pLayout->SetTexture("ENTRY_BUTTON", GetIconCollection()->GetButtonIcon(pController, 32), -1);
        pLayout->SetText("ENTRY", GetMessageString(0x60076, gEmptyText));
        pLayout->SetPaneVisible("ENTRY", true);
    }
}
