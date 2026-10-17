#include "nw4hbm/ut/ut_Font.h"

namespace nw4hbm {
namespace ut {

void Font::InitReaderFunc(FontEncoding encode) {
    switch (encode) {
    case FONT_ENCODING_UTF8: {
        mReaderFunc = &CharStrmReader::ReadNextCharUTF8;
        break;
    }

    case FONT_ENCODING_UTF16: {
        mReaderFunc = &CharStrmReader::ReadNextCharUTF16;
        break;
    }

    case FONT_ENCODING_SJIS: {
        mReaderFunc = &CharStrmReader::ReadNextCharSJIS;
        break;
    }

    case FONT_ENCODING_CP1252:
    default: {
        mReaderFunc = &CharStrmReader::ReadNextCharCP1252;
        break;
    }
    }
}

}
}
