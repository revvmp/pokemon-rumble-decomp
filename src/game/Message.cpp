#include <game/Message.h>

static std::tr1::shared_ptr<std::vector<std::tr1::shared_ptr<CMsgRes> > > sMsgResList;

void InitMsgResList(u32 num) {
    sMsgResList.reset(new std::vector<std::tr1::shared_ptr<CMsgRes> >(num));
}

void SetMsgRes(int group, const std::tr1::shared_ptr<CMsgRes>& rMsgRes) {
    if (sMsgResList && group < static_cast<int>(sMsgResList->size())) {
        (*sMsgResList)[group] = rMsgRes;
    }
}

inline CMsgRes* GetMsgRes(int group) {
    if (sMsgResList && group >= 0 && group < sMsgResList->size() && (*sMsgResList)[group]) {
        return (*sMsgResList)[group].get();
    }
    return nullptr;
}

const wchar_t* GetMessageText(int id, const wchar_t* pDefault) {
    int group = id >> 16;
    CMsgRes* pMsgRes = GetMsgRes(group);
    if (pMsgRes == nullptr) {
        return pDefault;
    }

    const wchar_t* pText = pMsgRes->GetMessage(static_cast<u16>(id));
    if (pText == nullptr) {
        return pDefault;
    }
    return pText;
}

static DateMessageIDs sDateMessageIDs = {-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1};

void SetDateMessageIDs(const DateMessageIDs& rIDs) {
    sDateMessageIDs = rIDs;
}

static TimeMessageIDs sTimeMessageIDs = {-1, -1, -1, -1, -1, -1, -1, -1};

void SetTimeMessageIDs(const TimeMessageIDs& rIDs) {
    sTimeMessageIDs = rIDs;
}
