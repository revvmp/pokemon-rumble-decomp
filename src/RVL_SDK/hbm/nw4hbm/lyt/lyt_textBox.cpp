#include "nw4hbm/lyt/lyt_textBox.h"
#include "nw4hbm/lyt/lyt_common.h"
#include "nw4hbm/lyt/lyt_layout.h"
#include "nw4hbm/lyt/lyt_material.h"
#include "nw4hbm/lyt/lyt_resourceAccessor.h"
#include "nw4hbm/ut/ut_ResFont.h"
#include "nw4hbm/ut/ut_TextWriterBase.h"

#include <string.h>
#include <wstring.h>

inline void* operator new(size_t, void* p) { return p; }

namespace {

using namespace nw4hbm;


u8 ClampColor(s16 value) {
    return value < 0 ? 0 : (value > 0xFF ? 0xFF : value);
}

ut::Color GetColor(const GXColorS10& rSrc) {
    GXColor dst;

    dst.r = ClampColor(rSrc.r);
    dst.g = ClampColor(rSrc.g);
    dst.b = ClampColor(rSrc.b);
    dst.a = ClampColor(rSrc.a);

    return dst;
}

template <typename T>
int CalcLineRectImpl(ut::Rect* pRect, ut::TextWriterBase<T>* pWriter, const T* pStr, int length, f32 maxWidth,
                     bool* pOver) {
    ut::PrintContext<T> context = {pWriter, pStr, 0.0f, 0.0f, 0};
    const ut::Font* pFont = pWriter->GetFont();

    f32 x = 0.0f;
    bool charSpace = false;

    ut::CharStrmReader reader = pFont->GetCharStrmReader();
    const T* pPrevPos = static_cast<const T*>(reader.GetCurrentPos());

    pRect->left = 0.0f;
    pRect->right = 0.0f;
    pRect->top = ut::Min(0.0f, pWriter->GetLineHeight());
    pRect->bottom = ut::Max(0.0f, pWriter->GetLineHeight());

    *pOver = false;
    reader.Set(pStr);

    ut::Rect prevRect = *pRect;

    for (u16 code = reader.Next(); static_cast<const T*>(reader.GetCurrentPos()) - pStr <= length;
         pPrevPos = static_cast<const T*>(reader.GetCurrentPos()), code = reader.Next(), prevRect = *pRect) {
        if (static_cast<int>(code) < ' ') {
            ut::Rect rect(x, 0.0f, 0.0f, 0.0f);

            context.str = static_cast<const T*>(reader.GetCurrentPos());
            context.flags = 0;
            context.flags |= charSpace ? 0 : ut::PrintContext<T>::FLAGS_CHARSPACE;

            pWriter->SetCursorX(x);

            typename ut::TagProcessorBase<T>::Operation operation =
                pWriter->GetTagProcessor().CalcRect(&rect, code, &context);

            reader.Set(context.str);

            pRect->left = ut::Min(pRect->left, rect.left);
            pRect->top = ut::Min(pRect->top, rect.top);
            pRect->right = ut::Max(pRect->right, rect.right);
            pRect->bottom = ut::Max(pRect->bottom, rect.bottom);

            x = pWriter->GetCursorX();

            if (pRect->GetWidth() > maxWidth) {
                *pOver = true;
                break;
            }

            if (operation == ut::TagProcessorBase<T>::OPERATION_END_DRAW) {
                return length;
            } else if (operation == ut::TagProcessorBase<T>::OPERATION_NO_CHAR_SPACE) {
                charSpace = false;
            } else if (operation == ut::TagProcessorBase<T>::OPERATION_CHAR_SPACE) {
                charSpace = true;
            } else if (operation == ut::TagProcessorBase<T>::OPERATION_NEXT_LINE) {
                break;
            }
        } else {
            if (charSpace) {
                x += pWriter->GetCharSpace();
            }

            charSpace = true;

            if (pWriter->IsWidthFixed()) {
                x += pWriter->GetFixedWidth();
            } else {
                x += pWriter->GetFont()->GetCharWidth(code) * pWriter->GetScaleH();
            }

            pRect->left = ut::Min(pRect->left, x);
            pRect->right = ut::Max(pRect->right, x);

            if (pRect->GetWidth() > maxWidth) {
                *pOver = true;
                break;
            }
        }
    }

    if (*pOver && pPrevPos) {
        *pRect = prevRect;
        return pPrevPos - pStr;
    } else {
        return static_cast<const T*>(reader.GetCurrentPos()) - pStr;
    }
}

template <typename T>
int CalcLineStrNum(f32* pWidth, ut::TextWriterBase<T>* pWriter, const T* pStr, int length, f32 maxWidth,
                   bool* pOver) {
    ut::Rect rect;
    ut::TextWriterBase<T> writer = *pWriter;
    writer.SetCursor(0.0f, 0.0f);

    int num = CalcLineRectImpl(&rect, &writer, pStr, length, maxWidth, pOver);

    *pWidth = rect.GetWidth();
    return num;
}

template <typename T>
void CalcStringRectImpl(ut::Rect* pRect, ut::TextWriterBase<T>* pWriter, const T* pStr, int length, f32 maxWidth) {
    int remain = length;
    const T* pPos = pStr;

    pRect->left = 0.0f;
    pRect->right = 0.0f;
    pRect->top = 0.0f;
    pRect->bottom = 0.0f;
    pWriter->SetCursor(0.0f, 0.0f);

    do {
        ut::Rect rect;
        bool over;
        int read = CalcLineRectImpl(&rect, pWriter, pPos, remain, maxWidth, &over);

        if (over) {
            CalcLineRectImpl(&rect, pWriter, L"\n", 1, maxWidth, &over);
        }

        pPos += read;
        remain -= read;

        pRect->left = ut::Min(pRect->left, rect.left);
        pRect->top = ut::Min(pRect->top, rect.top);
        pRect->right = ut::Max(pRect->right, rect.right);
        pRect->bottom = ut::Max(pRect->bottom, rect.bottom);
    } while (remain > 0);
}

template <typename T>
void CalcStringRect(ut::Rect* pRect, ut::TextWriterBase<T>* pWriter, const T* pStr, int length, f32 maxWidth) {
    ut::TextWriterBase<T> writer = *pWriter;
    CalcStringRectImpl(pRect, &writer, pStr, length, maxWidth);
}

}

namespace nw4hbm {
namespace lyt {

NW4R_UT_RTTI_DEF_DERIVED(TextBox, Pane);

TextBox::TextBox(const res::TextBox* pRes, const ResBlockSet& rBlockSet) : Pane(pRes) {
    u16 allocStrLen = pRes->textBufBytes / sizeof(wchar_t);
    if (allocStrLen) {
        allocStrLen = allocStrLen - 1;
    }

    Init(allocStrLen);

    if (pRes->textStrBytes >= sizeof(wchar_t) && mTextBuf) {
        const wchar_t* pResText = detail::ConvertOffsToPtr<wchar_t>(pRes, pRes->textStrOffset);
        const u16 resStrLen = pRes->textStrBytes / sizeof(wchar_t) - 1;

        SetString(pResText, 0, resStrLen);
    }

    for (int i = 0; i < 2; i++) {
        mTextColors[i] = pRes->textCols[i];
    }

    mFontSize = pRes->fontSize;
    mTextPosition = pRes->textPosition;
    mCharSpace = pRes->charSpace;
    mLineSpace = pRes->lineSpace;

    const res::Font* pFonts = detail::ConvertOffsToPtr<res::Font>(rBlockSet.pFontList, sizeof(res::FontList));
    const char* pFontName = detail::ConvertOffsToPtr<char>(pFonts, pFonts[pRes->fontIdx].nameStrOffset);

    if (ut::Font* pFont = rBlockSet.pResAccessor->GetFont(pFontName)) {
        mpFont = pFont;
    } else if (void* pFontRes = rBlockSet.pResAccessor->GetResource('font', pFontName, nullptr)) {
        if (void* pMem = Layout::AllocMemory(sizeof(ut::ResFont))) {
            ut::ResFont* pResFont = new (pMem) ut::ResFont();
            pResFont->SetResource(pFontRes);

            mpFont = pResFont;
            mTextBoxFlag.allocFont = true;
        }
    }

    if (void* pMem = Layout::AllocMemory(sizeof(Material))) {
        const u32* pMatOffsTbl = detail::ConvertOffsToPtr<u32>(rBlockSet.pMaterialList, sizeof(res::MaterialList));
        const res::Material* pResMaterial =
            detail::ConvertOffsToPtr<res::Material>(rBlockSet.pMaterialList, pMatOffsTbl[pRes->materialIdx]);

        mpMaterial = new (pMem) Material(pResMaterial, rBlockSet);
    }
}

void TextBox::Init(u16 allocStrLen) {
    mTextBuf = nullptr;
    mTextBufBytes = 0;
    mTextLen = 0;

    mpFont = nullptr;
    mFontSize = Size(0.0f, 0.0f);

    SetTextPositionH(HORIZONTALPOSITION_CENTER);
    SetTextPositionV(VERTICALPOSITION_CENTER);

    mLineSpace = 0.0f;
    mCharSpace = 0.0f;
    mpTagProcessor = nullptr;

    memset(&mTextBoxFlag, 0, sizeof(mTextBoxFlag));

    if (allocStrLen != 0) {
        AllocStringBuffer(allocStrLen);
    }
}

TextBox::~TextBox() {
    SetFont(nullptr);

    if (mpMaterial && !mpMaterial->IsUserAllocated()) {
        mpMaterial->~Material();
        Layout::FreeMemory(mpMaterial);
        mpMaterial = nullptr;
    }

    FreeStringBuffer();
}

ut::Color TextBox::GetVtxColor(u32 idx) const {
    return GetTextColor(idx / 2);
}

void TextBox::SetVtxColor(u32 idx, ut::Color color) {
    SetTextColor(idx / 2, color);
}

void TextBox::SetTextColor(u32 type, ut::Color color) {
    mTextColors[type] = color;
}

u8 TextBox::GetVtxColorElement(u32 idx) const {
    return reinterpret_cast<const u8*>(&mTextColors[idx / 8])[idx % 4];
}

void TextBox::SetVtxColorElement(u32 idx, u8 value) {
    reinterpret_cast<u8*>(&mTextColors[idx / 8])[idx % 4] = value;
}

void TextBox::DrawSelf(const DrawInfo& rInfo) {
    if (!mTextBuf || !mpFont || !mpMaterial) {
        return;
    }

    LoadMtx(rInfo);

    ut::TextWriterBase<wchar_t> writer;
    writer.SetFont(*mpFont);
    writer.SetFontSize(mFontSize.width, mFontSize.height);
    writer.SetLineSpace(mLineSpace);
    writer.SetCharSpace(mCharSpace);

    ut::Color topColor = detail::MultipleAlpha(mTextColors[0], mGlbAlpha);
    ut::Color bottomColor = detail::MultipleAlpha(mTextColors[1], mGlbAlpha);
    writer.SetGradationMode(topColor != bottomColor ? ut::CharWriter::GRADMODE_V : ut::CharWriter::GRADMODE_NONE);
    writer.SetTextColor(topColor, bottomColor);

    ut::Color minColor = GetColor(mpMaterial->GetTevColor(TEVCOLOR_REG0));
    ut::Color maxColor = GetColor(mpMaterial->GetTevColor(TEVCOLOR_REG1));
    writer.SetColorMapping(minColor, maxColor);

    if (mpTagProcessor) {
        writer.SetTagProcessor(mpTagProcessor);
    }

    writer.SetupGX();

    ut::Rect textRect = GetTextDrawRect(&writer);

    f32 magH = GetTextMagH();
    wchar_t* pPos = mTextBuf;
    f32 textWidth = textRect.GetWidth();

    writer.SetCursor(textRect.left, textRect.top);

    for (int remain = mTextLen; remain > 0;) {
        f32 lineWidth;
        bool over;
        int lineStrNum = CalcLineStrNum(&lineWidth, &writer, pPos, remain, mSize.width, &over);
        f32 offsetX = magH * (textWidth - lineWidth);

        writer.SetCursorX(textRect.left + offsetX);
        writer.Print(pPos, lineStrNum);

        if (over) {
            writer.Print(L"\n");
        }

        pPos += lineStrNum;
        remain -= lineStrNum;
    }
}

u16 TextBox::GetStringBufferLength() const {
    if (mTextBufBytes == 0) {
        return 0;
    }

    return mTextBufBytes / sizeof(wchar_t) - 1;
}

void TextBox::AllocStringBuffer(u16 minLen) {
    if (minLen == 0) {
        return;
    }

    u16 allocBytes = sizeof(wchar_t) * (minLen + 1);
    if (allocBytes <= mTextBufBytes) {
        return;
    }

    FreeStringBuffer();

    mTextBuf = static_cast<wchar_t*>(Layout::AllocMemory(allocBytes));
    if (mTextBuf) {
        mTextBufBytes = allocBytes;
    }
}

void TextBox::FreeStringBuffer() {
    if (mTextBuf) {
        Layout::FreeMemory(mTextBuf);
        mTextBuf = nullptr;
        mTextBufBytes = 0;
    }
}

u16 TextBox::SetString(const wchar_t* pStr, u16 dstIdx) {
    return SetString(pStr, dstIdx, wcslen(pStr));
}

u16 TextBox::SetString(const wchar_t* pStr, u16 dstIdx, u16 strLen) {
    if (!mTextBuf) {
        return 0;
    }

    u16 bufLen = GetStringBufferLength();
    if (dstIdx >= bufLen) {
        return 0;
    }

    const u16 copyLen = ut::Min<u16>(strLen, bufLen - dstIdx);
    memcpy(&mTextBuf[dstIdx], pStr, sizeof(wchar_t) * copyLen);

    mTextLen = dstIdx + copyLen;
    mTextBuf[mTextLen] = L'\0';

    return copyLen;
}

void TextBox::SetFont(const ut::Font* pFont) {
    if (mTextBoxFlag.allocFont) {
        mpFont->~Font();
        Layout::FreeMemory(const_cast<ut::Font*>(mpFont));
        mTextBoxFlag.allocFont = false;
    }

    mpFont = pFont;

    if (mpFont) {
        SetFontSize(Size(mpFont->GetWidth(), mpFont->GetHeight()));
    } else {
        SetFontSize(Size(0.0f, 0.0f));
    }
}

ut::Rect TextBox::GetTextDrawRect(ut::TextWriterBase<wchar_t>* pWriter) const {
    ut::Rect textRect;

    pWriter->SetCursor(0.0f, 0.0f);
    CalcStringRect(&textRect, pWriter, mTextBuf, mTextLen, mSize.width);

    math::VEC2 basePt = Pane::GetVtxPos();

    textRect.MoveTo(basePt.x + (mSize.width - textRect.GetWidth()) * GetTextMagH(),
                    basePt.y + (mSize.height - textRect.GetHeight()) * GetTextMagV());

    return textRect;
}

f32 TextBox::GetTextMagH() const {
    f32 mag = 0.0f;

    switch (GetTextPositionH()) {
    default:
    case HORIZONTALPOSITION_LEFT:
        mag = 0.0f;
        break;
    case HORIZONTALPOSITION_CENTER:
        mag = 0.5f;
        break;
    case HORIZONTALPOSITION_RIGHT:
        mag = 1.0f;
        break;
    }

    return mag;
}

f32 TextBox::GetTextMagV() const {
    f32 mag = 0.0f;

    switch (GetTextPositionV()) {
    default:
    case VERTICALPOSITION_TOP:
        mag = 0.0f;
        break;
    case VERTICALPOSITION_CENTER:
        mag = 0.5f;
        break;
    case VERTICALPOSITION_BOTTOM:
        mag = 1.0f;
        break;
    }

    return mag;
}

}
}
