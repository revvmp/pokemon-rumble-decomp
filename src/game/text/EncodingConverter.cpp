#include <game/text/EncodingConverter.h>

namespace {
extern const char* sEncodingNames[];
}

void CEncodingConverter::Initialize(nw4r::ut::FontEncoding input, nw4r::ut::FontEncoding output) {
    static ConvertFunction toUnicode[] = {
        nullptr, nullptr, &CEncodingConverter::ShiftJisToUnicode,
        &CEncodingConverter::SingleByteToUnicode
    };
    mToUnicode = toUnicode[input];

    static ConvertFunction fromUnicode[] = {
        nullptr, nullptr, &CEncodingConverter::UnicodeToShiftJis,
        &CEncodingConverter::UnicodeToSingleByte
    };

    mFromUnicode = fromUnicode[output];
    if (!mToUnicode && !mFromUnicode) {
        mConvert = &CEncodingConverter::Identity;
    } else if (mToUnicode) {
        mConvert = mToUnicode;
    } else if (mFromUnicode) {
        mConvert = mFromUnicode;
    } else {
        mConvert = &CEncodingConverter::ConvertThroughUnicode;
    }

    ENCInitContext(&mInputContext);
    ENCSetExternalEncoding(&mInputContext, sEncodingNames[input]);
    ENCSetAlternativeCharacter(&mInputContext, 0x20, 0x20);
    ENCInitContext(&mOutputContext);
    ENCSetExternalEncoding(&mOutputContext, sEncodingNames[output]);
    ENCSetAlternativeCharacter(&mOutputContext, 0x20, 0x20);
}

namespace {
const char* sEncodingNames[] = {
    "UTF-8", "UTF-16BE", "Shift_JIS", "windows-1252"
};
}

u16 CEncodingConverter::Identity(u16 value) {
    return value;
}

u16 CEncodingConverter::ConvertThroughUnicode(u16 value) {
    return (this->*mFromUnicode)((this->*mToUnicode)(value));
}

u16 CEncodingConverter::ShiftJisToUnicode(u16 value) {
    u8 input[2];
    u16 result;
    s32 inputSize;
    if ((0x8100 <= value && value <= 0x9F00) ||
        (0xE000 <= value && value <= 0xFC00)) {
        input[0] = value >> 8;
        input[1] = value;
        inputSize = 2;
    } else {
        input[0] = value;
        inputSize = 1;
    }
    s32 outputSize = 2;
    ENCConvertToInternalEncoding(&mInputContext, &result, &outputSize, input, &inputSize);
    return result;
}

u16 CEncodingConverter::SingleByteToUnicode(u16 value) {
    u16 result;
    u8 input = value;
    s32 outputSize = 2;
    s32 inputSize = 1;
    ENCConvertToInternalEncoding(&mInputContext, &result, &outputSize, &input, &inputSize);
    return result;
}

u16 CEncodingConverter::UnicodeToShiftJis(u16 value) {
    u8 result[2];
    s32 outputSize = 2;
    s32 inputSize = 2;
    ENCConvertFromInternalEncoding(&mOutputContext, result, &outputSize, &value, &inputSize);
    if (outputSize == 1) {
        return result[0];
    }
    return (result[0] << 8) | result[1];
}

u16 CEncodingConverter::UnicodeToSingleByte(u16 value) {
    u8 result;
    s32 outputSize = 1;
    s32 inputSize = 2;
    ENCConvertFromInternalEncoding(&mOutputContext, &result, &outputSize, &value, &inputSize);
    return result;
}
