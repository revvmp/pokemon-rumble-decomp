#ifndef NW4HBM_UT_RES_FONT_BASE_H
#define NW4HBM_UT_RES_FONT_BASE_H
#include "nw4hbm/types_nw4hbm.h"

#include "nw4hbm/ut/ut_Font.h"

namespace nw4hbm {
namespace ut {

struct FontTextureGlyph {
    u8 cellWidth;
    u8 cellHeight;
    s8 baselinePos;
    u8 maxCharWidth;
    u32 sheetSize;
    u16 sheetNum;
    u16 sheetFormat;
    u16 sheetRow;
    u16 sheetLine;
    u16 sheetWidth;
    u16 sheetHeight;
    u8* sheetImage;
};

struct FontWidth {
    u16 indexBegin;
    u16 indexEnd;
    FontWidth* pNext;
    CharWidths widthTable[];
};

struct FontCodeMap {
    u16 ccodeBegin;
    u16 ccodeEnd;
    u16 mappingMethod;
    u16 reserved;
    FontCodeMap* pNext;
    u16 mapInfo[];
};

struct FontInformation {
    u8 fontType;
    s8 linefeed;
    u16 alterCharIndex;
    CharWidths defaultWidth;
    u8 encoding;
    FontTextureGlyph* pGlyph;
    FontWidth* pWidth;
    FontCodeMap* pMap;
    u8 height;
    u8 width;
    u8 ascent;
};

namespace detail {

class ResFontBase : public Font {
public:
    static const u16 GLYPH_INDEX_NOT_FOUND = 0xFFFF;

public:
    ResFontBase();
    virtual ~ResFontBase();

    virtual int GetWidth() const;
    virtual int GetHeight() const;

    virtual int GetAscent() const;
    virtual int GetDescent() const;
    virtual int GetBaselinePos() const;

    virtual int GetCellHeight() const;
    virtual int GetCellWidth() const;
    virtual int GetMaxCharWidth() const;

    virtual Type GetType() const;
    virtual GXTexFmt GetTextureFormat() const;
    virtual int GetLineFeed() const;

    virtual CharWidths GetDefaultCharWidths() const;
    virtual void SetDefaultCharWidths(const CharWidths& rWidths);

    virtual bool SetAlternateChar(u16 ch);
    virtual void SetLineFeed(int lf);

    virtual int GetCharWidth(u16 ch) const;
    virtual CharWidths GetCharWidths(u16 ch) const;
    virtual void GetGlyph(Glyph* pGlyph, u16 ch) const;
    virtual FontEncoding GetEncoding() const;

    bool IsManaging(const void* pBuffer) const {
        return mResource == pBuffer;
    }

protected:
    void SetResourceBuffer(void* pBuffer, FontInformation* pInfo);
    void* RemoveResourceBuffer();

    u16 GetGlyphIndex(u16 ch) const;

    u16 FindGlyphIndex(u16 ch) const;
    u16 FindGlyphIndex(const FontCodeMap* pMap, u16 ch) const;

    const CharWidths& GetCharWidthsFromIndex(u16 index) const;
    const CharWidths& GetCharWidthsFromIndex(const FontWidth* pWidth,
                                             u16 index) const;

    void GetGlyphFromIndex(Glyph* pGlyph, u16 index) const;

protected:
    void* mResource;
    FontInformation* mFontInfo;
};

}
}
}

#endif
