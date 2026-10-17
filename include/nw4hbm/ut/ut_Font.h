#ifndef NW4HBM_UT_FONT_H
#define NW4HBM_UT_FONT_H
#include "nw4hbm/types_nw4hbm.h"

#include "nw4hbm/ut/ut_CharStrmReader.h"

#include "revolution/gx.h"

namespace nw4hbm {
namespace ut {

enum FontMapMethod {
    FONT_MAPMETHOD_DIRECT,
    FONT_MAPMETHOD_TABLE,
    FONT_MAPMETHOD_SCAN
};

enum FontEncoding {
    FONT_ENCODING_UTF8,
    FONT_ENCODING_UTF16,
    FONT_ENCODING_SJIS,
    FONT_ENCODING_CP1252,

    NUM_OF_FONT_ENCODING
};

struct CharWidths {
    s8 left;
    u8 glyphWidth;
    s8 charWidth;
};

struct Glyph {
    void* pTexture;
    CharWidths widths;
    u8 height;
    GXTexFmt texFormat;
    u16 texWidth;
    u16 texHeight;
    u16 cellX;
    u16 cellY;
};

class Font {
public:
    enum Type {
        TYPE_NULL,
        TYPE_ROM,
        TYPE_RESOURCE
    };

public:
    Font() : mReaderFunc(&CharStrmReader::ReadNextCharCP1252) {}
    virtual ~Font() {}

    virtual int GetWidth() const = 0;
    virtual int GetHeight() const = 0;
    virtual int GetAscent() const = 0;
    virtual int GetDescent() const = 0;
    virtual int GetBaselinePos() const = 0;
    virtual int GetCellHeight() const = 0;
    virtual int GetCellWidth() const = 0;
    virtual int GetMaxCharWidth() const = 0;
    virtual Type GetType() const = 0;
    virtual GXTexFmt GetTextureFormat() const = 0;
    virtual int GetLineFeed() const = 0;
    virtual CharWidths GetDefaultCharWidths() const = 0;
    virtual void SetDefaultCharWidths(const CharWidths& rWidths) = 0;
    virtual bool SetAlternateChar(u16 c) = 0;
    virtual void SetLineFeed(int linefeed) = 0;
    virtual int GetCharWidth(u16 c) const = 0;
    virtual CharWidths GetCharWidths(u16 c) const = 0;
    virtual void GetGlyph(Glyph* pGlyph, u16 c) const = 0;
    virtual FontEncoding GetEncoding() const = 0;

    void InitReaderFunc(FontEncoding encoding);

    CharStrmReader GetCharStrmReader() const {
        CharStrmReader reader(mReaderFunc);
        return reader;
    }

private:
    CharStrmReader::ReadFunc mReaderFunc;
};

}
}

#endif
