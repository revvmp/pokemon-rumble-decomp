#ifndef NW4R_UT_ROM_FONT_H
#define NW4R_UT_ROM_FONT_H
#include <nw4r/types_nw4r.h>

#include <nw4r/ut/ut_Font.h>

#include <revolution/os/OSFont.h>

namespace nw4r {
namespace ut {

class RomFont : public Font {
public:
    RomFont();
    virtual ~RomFont();

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
    virtual bool HasGlyph(u16 ch) const;
    virtual FontEncoding GetEncoding() const;

    bool Load(void* pBuffer);
    void* Unload();
    static u32 GetRequireBufferSize();

private:
    static const int CHAR_PTR_BUFFER_SIZE = 4;

private:
    void MakeCharPtr(char* pBuffer, u16 ch) const;
    u16 HandleUndefinedChar(u16 ch) const;

private:
    OSFontHeader* mFontHeader;
    CharWidths mDefaultWidths;
    u16 mAlternateChar;

    static u16 mFontEncode;
};

}
}

#endif
