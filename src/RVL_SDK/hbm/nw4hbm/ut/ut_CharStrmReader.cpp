#include "nw4hbm/ut/ut_CharStrmReader.h"

namespace nw4hbm {
namespace ut {
namespace {

inline bool IsSJISLeadByte(u8 ch) {
    return ((0x81 <= ch && ch < 0xA0) || 0xE0 <= ch) ? true : false;
}

}

u16 CharStrmReader::ReadNextCharUTF8() {
    u16 ch;

    if (!(GetChar<u8>(0) & 0x80)) {
        ch = GetChar<u8>(0);
        StepStrm<u8>(1);
    } else if ((GetChar<u8>(0) & 0xE0) == 0xC0) {
        ch = ((GetChar<u8>(0) & 0x1F) << 6) | (GetChar<u8>(1) & 0x3F);
        StepStrm<u8>(2);
    } else {
        ch = ((GetChar<u8>(0) & 0x1F) << 12) | ((GetChar<u8>(1) & 0x3F) << 6) | (GetChar<u8>(2) & 0x3F);
        StepStrm<u8>(3);
    }

    return ch;
}

u16 CharStrmReader::ReadNextCharUTF16() {
    u16 ch = GetChar<u16>(0);
    StepStrm<u16>(1);
    return ch;
}

u16 CharStrmReader::ReadNextCharCP1252() {
    u16 ch = GetChar<u8>(0);
    StepStrm<u8>(1);
    return ch;
}

u16 CharStrmReader::ReadNextCharSJIS() {
    u16 ch;

    if (IsSJISLeadByte(GetChar<u8>(0))) {
        ch = (GetChar<u8>(0) << 8) | GetChar<u8>(1);
        StepStrm<u8>(2);
    } else {
        ch = GetChar<u8>(0);
        StepStrm<u8>(1);
    }

    return ch;
}

}
}
