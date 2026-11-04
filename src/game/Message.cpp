#include <game/Message.h>
#include <revolution/os.h>
#include <string.h>
#include <wchar.h>
#include <MSL_C/locale.h>

namespace std {
template <> inline
__vec_constructor<tr1::shared_ptr<CMsgRes>, allocator<tr1::shared_ptr<CMsgRes> >, false>::__vec_constructor(size_type n) {
    if (n != 0) {
        pod_type constructTag;
        pod_type allocateTag;
        *reinterpret_cast<volatile unsigned char*>(&allocateTag) = 0;
        allocate(n, allocateTag);
        *reinterpret_cast<volatile unsigned char*>(&constructTag) = 0;
        base::construct_at_end(n, constructTag);
    }
}
}

static std::tr1::shared_ptr<std::vector<std::tr1::shared_ptr<CMsgRes> > > sMsgResList;

void InitMsgResList(u32 num) {
    sMsgResList.reset(new std::vector<std::tr1::shared_ptr<CMsgRes> >(num));
}

void SetMsgRes(int group, std::tr1::shared_ptr<CMsgRes> messages) {
    if (sMsgResList && group < static_cast<int>(sMsgResList->size())) {
        (*sMsgResList)[group] = messages;
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

inline int ToUpperWide(wchar_t c) {
    return c >= 256 ? c : _current_locale.ctype_cmpt_ptr->wupper_map_ptr[c];
}

inline long GetMessageNumber(int id) {
    return wcstol(GetMessageText(id, L"0"), nullptr, 10);
}

static DateMessageIDs sDateMessageIDs = {-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1};

static const wchar_t* sMonthNames[12] = {
    L"1", L"2", L"3", L"4", L"5", L"6", L"7", L"8", L"9", L"10", L"11", L"12",
};

static const wchar_t* sWeekdayNames[7] = {
    L"\u65E5", L"\u6708", L"\u706B", L"\u6C34", L"\u6728", L"\u91D1", L"\u571F",
};

void SetDateMessageIDs(const DateMessageIDs& rIDs) {
    sDateMessageIDs = rIDs;
}

void FormatDate(std::wstring& rResult, int year, int month, int day, u32 flags) {
    wchar_t text[256];

    if (flags & DATE_NAMED_MONTH) {
        const wchar_t* pMonth = GetMessageText(
            month + ((flags & DATE_ABBREVIATED) ? sDateMessageIDs.mMonthShort : sDateMessageIDs.mMonthLong) - 1, nullptr);
        if (pMonth == nullptr) {
            pMonth = sMonthNames[month - 1];
        }

        const wchar_t* pSuffix = day == 1 ? GetMessageText(sDateMessageIDs.mFirstDaySuffix, L"") : L"";

        if (flags & DATE_NO_YEAR) {
            const wchar_t* pFormat = GetMessageText(sDateMessageIDs.mLongDate, L"%1$ls/%2$d");
            swprintf(text, 256, pFormat, pMonth, day, pSuffix);
        } else {
            const wchar_t* pFormat = GetMessageText(sDateMessageIDs.mLongYearDate, L"%1$d/%2$ls/%3$d");
            swprintf(text, 256, pFormat, year, pMonth, day, pSuffix);
        }
    } else if (flags & DATE_NO_YEAR) {
        const wchar_t* pFormat = GetMessageText(sDateMessageIDs.mShortDate, L"%1$d/%2$d");
        swprintf(text, 256, pFormat, month, day);
    } else {
        const wchar_t* pFormat = GetMessageText(sDateMessageIDs.mShortYearDate, L"%1$d/%2$d/%3$d");
        swprintf(text, 256, pFormat, year, month, day);
    }

    if (!(flags & DATE_WEEKDAY)) {
        rResult = text;
    } else {
        OSCalendarTime calendar;
        memset(&calendar, 0, sizeof(calendar));
        calendar.year = year;
        calendar.mon = month - 1;
        calendar.mday = day;
        OSTicksToCalendarTime(OSCalendarTimeToTicks(&calendar), &calendar);
        int weekday = calendar.wday;

        const wchar_t* pWeekday = GetMessageText(
            weekday + ((flags & DATE_ABBREVIATED)
                           ? sDateMessageIDs.mWeekdayShort
                           : (flags & DATE_SHORT_WEEKDAY) ? sDateMessageIDs.mWeekdayMedium : sDateMessageIDs.mWeekdayLong),
            nullptr);
        if (pWeekday == nullptr) {
            pWeekday = sWeekdayNames[weekday];
        }

        if (GetMessageNumber(sDateMessageIDs.mWeekdayAfterDate) == 0) {
            rResult = std::wstring(pWeekday) + L' ' + text;
        } else {
            rResult = std::wstring(text) + L' ' + pWeekday;
        }
    }

    const_cast<wchar_t*>(rResult.data())[0] = ToUpperWide(rResult.data()[0]);
}

static TimeMessageIDs sTimeMessageIDs = {-1, -1, -1, -1, -1, -1, -1, -1};

void SetTimeMessageIDs(const TimeMessageIDs& rIDs) {
    sTimeMessageIDs = rIDs;
}

void FormatTime(std::wstring& rResult, int hour, int minute, int second, u32 flags) {
    wchar_t text[256];

    const wchar_t* pMeridiem;
    int hourCycle;
    if (flags & TIME_12_HOUR) {
        hourCycle = 12;
    } else if (flags & TIME_24_HOUR) {
        hourCycle = 24;
    } else {
        hourCycle = GetMessageNumber(sTimeMessageIDs.mHourCycle) == 24 ? 24 : 12;
    }

    int displayHour = hour % hourCycle;
    if (hourCycle == 12 && displayHour == 0) {
        displayHour = 12;
    }

    if (flags & TIME_FIRST_SET) {
        if (flags & TIME_NO_SECONDS) {
            const wchar_t* pFormat = GetMessageText(sTimeMessageIDs.mHourMinuteA, L"%d:%02d");
            swprintf(text, 256, pFormat, displayHour, minute);
        } else {
            const wchar_t* pFormat = GetMessageText(sTimeMessageIDs.mHourMinuteSecondA, L"%d:%02d:%02d");
            swprintf(text, 256, pFormat, displayHour, minute, second);
        }
    } else if (flags & TIME_NO_SECONDS) {
        const wchar_t* pFormat = GetMessageText(sTimeMessageIDs.mHourMinuteB, L"%d:%02d");
        swprintf(text, 256, pFormat, displayHour, minute);
    } else {
        const wchar_t* pFormat = GetMessageText(sTimeMessageIDs.mHourMinuteSecondB, L"%d:%02d:%02d");
        swprintf(text, 256, pFormat, displayHour, minute, second);
    }

    if (hourCycle == 24) {
        rResult = text;
    } else {
        if (hour < 12) {
            pMeridiem = GetMessageText(sTimeMessageIDs.mAm, L"AM");
        } else {
            pMeridiem = GetMessageText(sTimeMessageIDs.mPm, L"PM");
        }

        if (GetMessageNumber(sTimeMessageIDs.mMeridiemAfterTime) == 0) {
            rResult = std::wstring(pMeridiem) + L' ' + text;
        } else {
            rResult = std::wstring(text) + L' ' + pMeridiem;
        }
    }
}
