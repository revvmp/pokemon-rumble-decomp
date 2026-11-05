#ifndef GAME_SYSTEM_MESSAGE_DATA_H
#define GAME_SYSTEM_MESSAGE_DATA_H

#include <game/text/CMsgRes.h>

template <unsigned long TextLength> struct SystemMessageResource {
    CMsgRes::FileHeader mFileHeader;
    CMsgRes::BlockHeader mInfoHeader;
    u16 mEntryCount;
    u16 mEntrySize;
    u16 mGroupID;
    u8 mDefaultColor;
    u8 mReserved;
    CMsgRes::MessageInfo mMessages[7];
    u8 mInfoPadding[24];
    CMsgRes::BlockHeader mDataHeader;
    wchar_t mText[TextLength];
};

extern SystemMessageResource<300> gJapaneseSystemMessages;
extern SystemMessageResource<364> gEnglishSystemMessages;
extern SystemMessageResource<380> gEuropeanEnglishSystemMessages;
extern SystemMessageResource<444> gGermanSystemMessages;
extern SystemMessageResource<460> gFrenchSystemMessages;
extern SystemMessageResource<428> gSpanishSystemMessages;
extern SystemMessageResource<444> gItalianSystemMessages;

#endif
