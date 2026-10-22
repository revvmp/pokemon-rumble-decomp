#include <game/ButtonMessage.h>

#include <game/CPlayer.h>
#include <game/IController.h>
#include <game/IGameSession.h>
#include <game/Message.h>
#include <game/MessageFormat.h>

std::wstring GetButtonMessage(int player, u32 button, int messageId) {
    IController* pController = IGameSession::GetInstance()->GetPlayer(player)->GetController();
    if (pController == nullptr) {
        return L"";
    }
    const wchar_t* pSymbol = pController->GetButtonSymbol(button);
    if (pSymbol == nullptr || *pSymbol == 0) {
        return L"";
    }
    return FormatMessage(GetMessageText(messageId, gEmptyText), pSymbol) + L"\n";
}

CPlayer* FindPlayerHoldingPii(const std::tr1::shared_ptr<CPiiPersonalData>& rPii, int exceptPlayer) {
    for (int i = 0; i < 4; i++) {
        if (i == exceptPlayer) {
            continue;
        }
        CPlayer* pPlayer = IGameSession::GetInstance()->GetPlayer(i);
        if (pPlayer->IsJoined() && (pPlayer->GetPersonalData() == rPii || pPlayer->GetPendingPersonalData() == rPii)) {
            return pPlayer;
        }
    }
    return nullptr;
}

std::tr1::shared_ptr<CPiiPersonalData> CPlayer::GetPersonalData() const {
    return mState.mPersonalData;
}

std::tr1::shared_ptr<CPiiPersonalData> CPlayer::GetPendingPersonalData() const {
    return mState.mPendingPersonalData;
}
