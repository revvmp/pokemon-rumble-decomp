#ifndef NW4R_UT_FONT_H
#define NW4R_UT_FONT_H
#include <nw4r/types_nw4r.h>

#include <nw4r/ut/ut_CharStrmReader.h>

#include <revolution/GX.h>

namespace nw4r {
namespace ut {

enum FontEncoding {
    FONT_ENCODING_UTF8,
    FONT_ENCODING_UTF16,
    FONT_ENCODING_SJIS,
    FONT_ENCODING_CP1252,

    FONT_ENCODING_MAX
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
    enum Type { TYPE_NULL, TYPE_ROM, TYPE_RESOURCE, TYPE_PAIR };

public:
    Font() : mReadFunc(&CharStrmReader::ReadNextCharCP1252) {}
    virtual ~Font() {}

    virtual int GetWidth() const = 0;
    virtual int GetHeight() const = 0;

    virtual int GetAscent() const = 0;
    virtual int GetDescent() const = 0;
    virtual int GetBaselinePos() const = 0;
    virtual int GetBaselinePos(u16 ch) const {
        return GetBaselinePos();
    }

    virtual int GetCellHeight() const = 0;
    virtual int GetCellWidth() const = 0;
    virtual int GetMaxCharWidth() const = 0;

    virtual Type GetType() const = 0;
    virtual GXTexFmt GetTextureFormat() const = 0;
    virtual int GetLineFeed() const = 0;

    virtual CharWidths GetDefaultCharWidths() const = 0;
    virtual void SetDefaultCharWidths(const CharWidths& rWidths) = 0;

    virtual bool SetAlternateChar(u16 ch) = 0;
    virtual void SetLineFeed(int lf) = 0;

    virtual int GetCharWidth(u16 ch) const = 0;
    virtual CharWidths GetCharWidths(u16 ch) const = 0;
    virtual void GetGlyph(Glyph* pGlyph, u16 ch) const = 0;
    virtual bool HasGlyph(u16 ch) const = 0;
    virtual FontEncoding GetEncoding() const = 0;

    void InitReaderFunc(FontEncoding encode);

    CharStrmReader GetCharStrmReader() const __attribute__((never_inline)) {
        CharStrmReader reader(mReadFunc);
        return reader;
    }

private:
    CharStrmReader::ReadFunc mReadFunc;
};

}
}

#endif
