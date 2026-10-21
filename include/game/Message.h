#ifndef GAME_MESSAGE_H
#define GAME_MESSAGE_H

#include <game/CMsgRes.h>
#include <game/SystemMessage.h>
#include <memory>
#include <vector>

struct DateMessageIDs {
    int mIDs[11];
};

struct TimeMessageIDs {
    int mIDs[8];
};

void InitMsgResList(u32 num);
void SetMsgRes(int group, const std::tr1::shared_ptr<CMsgRes>& rMsgRes);
const wchar_t* GetMessageText(int id, const wchar_t* pDefault);
inline const wchar_t* GetMessageString(int id, const wchar_t* pFallback) {
    return GetMessageText(id, gEmptyText);
}
void SetDateMessageIDs(const DateMessageIDs& rIDs);
void SetTimeMessageIDs(const TimeMessageIDs& rIDs);

#endif
