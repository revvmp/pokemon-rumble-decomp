#ifndef GAME_MESSAGE_H
#define GAME_MESSAGE_H

#include <game/CMsgRes.h>
#include <game/SystemMessage.h>
#include <memory>
#include <string>
#include <vector>

struct DateMessageIDs {
    int mMonthShort;
    int mMonthLong;
    int mLongYearDate;
    int mLongDate;
    int mShortYearDate;
    int mShortDate;
    int mFirstDaySuffix;
    int mWeekdayShort;
    int mWeekdayMedium;
    int mWeekdayLong;
    int mWeekdayAfterDate;
};

struct TimeMessageIDs {
    int mHourMinuteSecondA;
    int mHourMinuteA;
    int mHourMinuteSecondB;
    int mHourMinuteB;
    int mHourCycle;
    int mAm;
    int mPm;
    int mMeridiemAfterTime;
};

enum DateFormatFlags {
    DATE_NAMED_MONTH = 0x1,
    DATE_SHORT_WEEKDAY = 0x2,
    DATE_NO_YEAR = 0x8,
    DATE_WEEKDAY = 0x20,
    DATE_ABBREVIATED = 0x40,
};

enum TimeFormatFlags {
    TIME_FIRST_SET = 0x1,
    TIME_NO_SECONDS = 0x8,
    TIME_12_HOUR = 0x10,
    TIME_24_HOUR = 0x20,
};

void InitMsgResList(u32 num);
void SetMsgRes(int group, const std::tr1::shared_ptr<CMsgRes>& rMsgRes);
const wchar_t* GetMessageText(int id, const wchar_t* pDefault);
inline const wchar_t* GetMessageString(int id, const wchar_t* pFallback) {
    return GetMessageText(id, gEmptyText);
}
void SetDateMessageIDs(const DateMessageIDs& rIDs);
void SetTimeMessageIDs(const TimeMessageIDs& rIDs);
void FormatDate(std::wstring& rResult, int year, int month, int day, u32 flags);
void FormatTime(std::wstring& rResult, int hour, int minute, int second, u32 flags);

#endif
