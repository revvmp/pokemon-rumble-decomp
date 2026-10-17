#include <nw4r/ut/ut_PairFont.h>
#include <nw4r/ut/ut_algorithm.h>

namespace nw4r {
namespace ut {

PairFont::PairFont(Font* pFirst, Font* pSecond)
    : mFirst(pFirst), mSecond(pSecond), mAlternateWithFirst(true) {
    InitReaderFunc(mFirst->GetEncoding());
}

PairFont::~PairFont() {}

int PairFont::GetWidth() const {
    return Max(mFirst->GetWidth(), mSecond->GetWidth());
}

int PairFont::GetHeight() const {
    return Max(mFirst->GetHeight(), mSecond->GetHeight());
}

int PairFont::GetAscent() const {
    return mFirst->GetHeight() >= mSecond->GetHeight() ? mFirst->GetAscent() : mSecond->GetAscent();
}

int PairFont::GetDescent() const {
    return mFirst->GetHeight() >= mSecond->GetHeight() ? mFirst->GetDescent() : mSecond->GetDescent();
}

int PairFont::GetBaselinePos() const {
    return mFirst->GetHeight() >= mSecond->GetHeight() ? mFirst->GetBaselinePos() : mSecond->GetBaselinePos();
}

int PairFont::GetBaselinePos(u16 ch) const {
    if (mFirst->HasGlyph(ch)) {
        return mFirst->GetBaselinePos(ch);
    } else if (mSecond->HasGlyph(ch)) {
        return mSecond->GetBaselinePos(ch);
    } else if (mAlternateWithFirst) {
        return mFirst->GetBaselinePos(ch);
    } else {
        return mSecond->GetBaselinePos(ch);
    }
}

int PairFont::GetCellHeight() const {
    return mFirst->GetHeight() >= mSecond->GetHeight() ? mFirst->GetCellHeight() : mSecond->GetCellHeight();
}

int PairFont::GetCellWidth() const {
    return mFirst->GetWidth() >= mSecond->GetWidth() ? mFirst->GetCellWidth() : mSecond->GetCellWidth();
}

int PairFont::GetMaxCharWidth() const {
    return Max(mFirst->GetMaxCharWidth(), mSecond->GetMaxCharWidth());
}

Font::Type PairFont::GetType() const {
    return TYPE_PAIR;
}

GXTexFmt PairFont::GetTextureFormat() const {
    return mFirst->GetTextureFormat();
}

int PairFont::GetLineFeed() const {
    return Max(mFirst->GetLineFeed(), mSecond->GetLineFeed());
}

CharWidths PairFont::GetDefaultCharWidths() const {
    if (mFirst->GetWidth() >= mSecond->GetWidth()) {
        return mFirst->GetDefaultCharWidths();
    } else {
        return mSecond->GetDefaultCharWidths();
    }
}

void PairFont::SetDefaultCharWidths(const CharWidths& rWidths) {
    mFirst->SetDefaultCharWidths(rWidths);
    mSecond->SetDefaultCharWidths(rWidths);
}

bool PairFont::SetAlternateChar(u16 ch) {
    bool first = mFirst->SetAlternateChar(ch);
    bool second = mSecond->SetAlternateChar(ch);

    if (first) {
        mAlternateWithFirst = true;
        return true;
    } else if (second) {
        mAlternateWithFirst = false;
        return true;
    }

    return false;
}

void PairFont::SetLineFeed(int lf) {
    if (mFirst->GetHeight() >= mSecond->GetHeight()) {
        mFirst->SetLineFeed(lf);
    } else {
        mSecond->SetLineFeed(lf);
    }
}

int PairFont::GetCharWidth(u16 ch) const {
    if (mFirst->HasGlyph(ch)) {
        return mFirst->GetCharWidth(ch);
    } else if (mSecond->HasGlyph(ch)) {
        return mSecond->GetCharWidth(ch);
    } else if (mAlternateWithFirst) {
        return mFirst->GetCharWidth(ch);
    } else {
        return mSecond->GetCharWidth(ch);
    }
}

CharWidths PairFont::GetCharWidths(u16 ch) const {
    if (mFirst->HasGlyph(ch)) {
        return mFirst->GetCharWidths(ch);
    } else if (mSecond->HasGlyph(ch)) {
        return mSecond->GetCharWidths(ch);
    } else if (mAlternateWithFirst) {
        return mFirst->GetCharWidths(ch);
    } else {
        return mSecond->GetCharWidths(ch);
    }
}

void PairFont::GetGlyph(Glyph* pGlyph, u16 ch) const {
    if (mFirst->HasGlyph(ch)) {
        mFirst->GetGlyph(pGlyph, ch);
    } else if (mSecond->HasGlyph(ch)) {
        mSecond->GetGlyph(pGlyph, ch);
    } else if (mAlternateWithFirst) {
        mFirst->GetGlyph(pGlyph, ch);
    } else {
        mSecond->GetGlyph(pGlyph, ch);
    }
}

bool PairFont::HasGlyph(u16 ch) const {
    return mFirst->HasGlyph(ch) || mSecond->HasGlyph(ch);
}

FontEncoding PairFont::GetEncoding() const {
    return mFirst->GetEncoding();
}

}
}
