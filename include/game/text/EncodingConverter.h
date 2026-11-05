#ifndef GAME_ENCODING_CONVERTER_H
#define GAME_ENCODING_CONVERTER_H

#include <revolution/enc.h>
#include <nw4r/ut/ut_Font.h>

class CEncodingConverter {
public:
    typedef u16 (CEncodingConverter::*ConvertFunction)(u16);

    void Initialize(nw4r::ut::FontEncoding input, nw4r::ut::FontEncoding output);
    u16 Convert(u16 value) { return (this->*mConvert)(value); }

private:
    u16 Identity(u16 value);
    u16 ConvertThroughUnicode(u16 value);
    u16 ShiftJisToUnicode(u16 value);
    u16 SingleByteToUnicode(u16 value);
    u16 UnicodeToShiftJis(u16 value);
    u16 UnicodeToSingleByte(u16 value);

    ENCContext mInputContext;
    ENCContext mOutputContext;
    ConvertFunction mToUnicode;
    ConvertFunction mFromUnicode;
    ConvertFunction mConvert;
};

#endif
