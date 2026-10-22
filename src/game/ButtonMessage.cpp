#include <game/ButtonMessage.h>

#include <game/CPlayer.h>
#include <game/IController.h>
#include <game/IGameSession.h>
#include <game/Message.h>
#include <game/MessageFormat.h>

template NIntl::basic_formatter<wchar_t>::basic_formatter(const wchar_t*);
template NIntl::basic_formatter<wchar_t>::~basic_formatter();

namespace std {

template <class charT, class traits, class Allocator>
basic_string<charT, traits, Allocator> operator+(const basic_string<charT, traits, Allocator>& lhs, const charT* rhs)
{
    basic_string<charT, traits, Allocator> result;
    size_t rhsLength = traits::length(rhs);
    size_t lhsLength;
    const charT* lhsData;
    lhs.get_buffer(lhsData, lhsLength);
    result.reserve(lhsLength + rhsLength);
    result.replace(0, 0, lhsData, lhsData + lhsLength).replace(lhsLength, 0, rhs, rhs + rhsLength);
    return result;
}

}

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

#pragma scheduling off
std::tr1::shared_ptr<CPiiPersonalData> CPlayer::GetPersonalData() const {
    return mState.mPersonalData;
}

std::tr1::shared_ptr<CPiiPersonalData> CPlayer::GetPendingPersonalData() const {
    return mState.mPendingPersonalData;
}
#pragma scheduling reset
