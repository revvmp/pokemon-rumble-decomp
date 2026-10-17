#ifndef NW4R_UT_PAIR_FONT_H
#define NW4R_UT_PAIR_FONT_H
#include <nw4r/types_nw4r.h>

#include <nw4r/ut/ut_Font.h>

namespace nw4r {
namespace ut {

class PairFont : public Font {
public:
    PairFont(Font* pFirst, Font* pSecond);
    virtual ~PairFont();

    virtual int GetWidth() const;
    virtual int GetHeight() const;

    virtual int GetAscent() const;
    virtual int GetDescent() const;
    virtual int GetBaselinePos() const;
    virtual int GetBaselinePos(u16 ch) const;

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

private:
    Font* mFirst;
    Font* mSecond;
    bool mAlternateWithFirst;
};

}
}

#endif
