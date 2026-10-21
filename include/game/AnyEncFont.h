#ifndef GAME_ANY_ENC_FONT_H
#define GAME_ANY_ENC_FONT_H

#include <game/EncodingConverter.h>

template <typename T> class CFontConversionGuard {
public:
    explicit CFontConversionGuard(T& rDepth) : mDepth(rDepth) { ++mDepth; }
    ~CFontConversionGuard() { --mDepth; }

private:
    T& mDepth;
};

template <typename T> class CAnyEncFont : public T {
public:
    CAnyEncFont() : mEncoding(nw4r::ut::FONT_ENCODING_UTF16), mConversionDepth(0) {}
    virtual ~CAnyEncFont() {}

    virtual bool SetAlternateChar(u16 ch) {
        if (mConversionDepth > 0) return T::SetAlternateChar(ch);
        CFontConversionGuard<int> guard(mConversionDepth);
        return T::SetAlternateChar(mConverter.Convert(ch));
    }
    virtual int GetCharWidth(u16 ch) const {
        if (mConversionDepth > 0) return T::GetCharWidth(ch);
        CFontConversionGuard<int> guard(mConversionDepth);
        return T::GetCharWidth(mConverter.Convert(ch));
    }
    virtual nw4r::ut::CharWidths GetCharWidths(u16 ch) const {
        if (mConversionDepth > 0) return T::GetCharWidths(ch);
        CFontConversionGuard<int> guard(mConversionDepth);
        return T::GetCharWidths(mConverter.Convert(ch));
    }
    virtual void GetGlyph(nw4r::ut::Glyph* pGlyph, u16 ch) const {
        if (mConversionDepth > 0) {
            T::GetGlyph(pGlyph, ch);
            return;
        }
        CFontConversionGuard<int> guard(mConversionDepth);
        T::GetGlyph(pGlyph, mConverter.Convert(ch));
    }
    virtual bool HasGlyph(u16 ch) const {
        if (mConversionDepth > 0) return T::HasGlyph(ch);
        CFontConversionGuard<int> guard(mConversionDepth);
        return T::HasGlyph(mConverter.Convert(ch));
    }
    virtual nw4r::ut::FontEncoding GetEncoding() const { return mEncoding; }

    void SetEncoding(nw4r::ut::FontEncoding encoding) {
        mEncoding = encoding;
        this->InitReaderFunc(mEncoding);
        mConverter.Initialize(mEncoding, T::GetEncoding());
    }

private:
    mutable CEncodingConverter mConverter;
    nw4r::ut::FontEncoding mEncoding;
    mutable int mConversionDepth;
};

#endif
