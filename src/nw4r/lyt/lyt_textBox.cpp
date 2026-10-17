#include <nw4r/ut/ut_algorithm.h>
#include <nw4r/lyt/lyt_common.h>
#include <nw4r/lyt/lyt_drawInfo.h>
#include <nw4r/ut/ut_CharStrmReader.h>
#include <nw4r/lyt/lyt_resources.h>
#include <nw4r/lyt/lyt_textBox.h>
#include <nw4r/lyt/lyt_layout.h>
#include <nw4r/ut/ut_Font.h>
#include <nw4r/ut/ut_TextWriterBase.h>
#include <nw4r/ut/ut_ResFont.h>
#include <nw4r/lyt/lyt_material.h>
#include <nw4r/lyt/lyt_resourceAccessor.h>
#include <revolution/gx/GXTransform.h>
#include <revolution/gx/GXPixel.h>
#include <revolution/gx/GXDispList.h>
#include <stdlib.h>
#include <string.h>
#include <wstring.h>
namespace nw4r {
namespace lyt {
NW4R_UT_RTTI_DEF_DERIVED(TextBox, Pane);

namespace {

void ReverseYAxis(math::MTX34 *pMtx) {

    pMtx->_01 = -pMtx->_01;
    pMtx->_11 = -pMtx->_11;
    pMtx->_21 = -pMtx->_21;
}

u8 ClampColor(s16 colVal) {

    return colVal < 0 ? 0 : (colVal > 0xFF ? 0xFF : colVal);
}

ut::Color GetColor(const GXColorS10 &src) {
    GXColor dst;
    dst.r = ClampColor(src.r);
    dst.g = ClampColor(src.g);
    dst.b = ClampColor(src.b);
    dst.a = ClampColor(src.a);
    return dst;
}

template <typename T>
void CalcStringRect(ut::Rect *pRect, ut::TextWriterBase<T> *pTextWriter, const T *str, int length, f32 maxWidth);
template <typename T>
int CalcLineStrNum(
    f32 *pWidth, ut::TextWriterBase<T> *pTextWriter, const T *str, int length, f32 maxWidth, bool *pbOver
);
template <typename T>
int CalcLineRectImpl(
    ut::Rect *pRect, ut::TextWriterBase<T> *pTextWriter, const T *str, int length, f32 maxWidth, bool *pbOver
);
template <typename T>
void CalcStringRectImpl(ut::Rect *pRect, ut::TextWriterBase<T> *pTextWriter, const T *str, int length, f32 maxWidth);

template <typename T>
void CalcStringRect(ut::Rect *pRect, ut::TextWriterBase<T> *pTextWriter, const T *str, int length, f32 maxWidth) {
    ut::TextWriterBase<T> myCopy = *pTextWriter;
    CalcStringRectImpl<T>(pRect, &myCopy, str, length, maxWidth);
}

template <typename T>
int CalcLineStrNum(
    f32 *pWidth, ut::TextWriterBase<T> *pTextWriter, const T *str, int length, f32 maxWidth, bool *pbOver
) {
    ut::Rect rect;
    ut::TextWriterBase<T> myCopy = *pTextWriter;
    myCopy.SetCursor(0.0f, 0.0f);
    int ret = CalcLineRectImpl(&rect, &myCopy, str, length, maxWidth, pbOver);

    *pWidth = rect.GetWidth();
    return ret;
}

template <typename T>
int CalcLineRectImpl(
    ut::Rect *pRect, ut::TextWriterBase<T> *pTextWriter, const T *str, int length, f32 maxWidth, bool *pbOver
) {
    ut::PrintContext<T> context = {pTextWriter, str, 0.0f, 0.0f, 0};
    f32 x = 0.0f;

    const ut::Font *font = pTextWriter->GetFont();
    bool bCharSpace = false;
    ut::CharStrmReader reader = font->GetCharStrmReader();

    const T *prStrPos = (const T *)reader.GetCurrentPos();

    pRect->left = 0.0f;
    pRect->right = 0.0f;
    pRect->top = ut::Min(0.0f, pTextWriter->GetLineHeight());
    pRect->bottom = ut::Max(0.0f, pTextWriter->GetLineHeight());

    *pbOver = false;

    reader.Set(str);
    ut::Rect prMaxRect = *pRect;
    u16 code = reader.Next();
    while (((const T *)reader.GetCurrentPos() - str) <= length) {
        if (code < ' ') {
            typename ut::TagProcessorBase<T>::Operation operation;
            ut::Rect rect(x, 0.0f, 0.0f, 0.0f);

            context.str = (const T *)reader.GetCurrentPos();
            context.flags = !bCharSpace;

            pTextWriter->SetCursorX(x);
            operation = pTextWriter->GetTagProcessor()->CalcRect(&rect, code, &context);
            reader.Set(context.str);

            pRect->left = ut::Min(pRect->left, rect.left);
            pRect->top = ut::Min(pRect->top, rect.top);
            pRect->right = ut::Max(pRect->right, rect.right);
            pRect->bottom = ut::Max(pRect->bottom, rect.bottom);

            x = pTextWriter->GetCursorX();

            if (pRect->GetWidth() > maxWidth) {
                *pbOver = true;
                break;
            }
            if (operation == ut::TagProcessorBase<T>::OPERATION_END_DRAW) {
                return length;
            } else if (operation == ut::TagProcessorBase<T>::OPERATION_NO_CHAR_SPACE) {
                bCharSpace = false;
            } else if (operation == ut::TagProcessorBase<T>::OPERATION_CHAR_SPACE) {
                bCharSpace = true;
            } else if (operation == ut::TagProcessorBase<T>::OPERATION_NEXT_LINE) {
                break;
            }

        } else {
            if (bCharSpace) {
                x += pTextWriter->GetCharSpace();
            }
            bCharSpace = true;
            if (pTextWriter->IsWidthFixed()) {
                x += pTextWriter->GetFixedWidth();
            } else {
                x += pTextWriter->GetFont()->GetCharWidth(code) * pTextWriter->GetScaleH();
            }
            pRect->left = ut::Min(pRect->left, x);
            pRect->right = ut::Max(pRect->right, x);

            if (pRect->GetWidth() > maxWidth) {
                *pbOver = true;
                break;
            }
        }
        prStrPos = (const T *)reader.GetCurrentPos();
        code = reader.Next();
        prMaxRect = *pRect;
    }

    if (*pbOver) {
        if (prStrPos) {
            *pRect = prMaxRect;
            return (prStrPos - str);
        }
    }
    return ((const T *)reader.GetCurrentPos() - str);
}
template <typename T>
void CalcStringRectImpl(ut::Rect *pRect, ut::TextWriterBase<T> *pTextWriter, const T *str, int length, f32 maxWidth) {
    pRect->left = 0.0f;
    pRect->right = 0.0f;
    pRect->top = 0.0f;
    pRect->bottom = 0.0f;
    pTextWriter->SetCursor(0.0f, 0.0f);
    int remain = length;
    const T *pos = str;
    do {
        ut::Rect rect;
        bool bOver;
        int read = CalcLineRectImpl(&rect, pTextWriter, pos, remain, maxWidth, &bOver);
        pos += read;
        remain -= read;
        pRect->left = ut::Min(pRect->left, rect.left);
        pRect->top = ut::Min(pRect->top, rect.top);
        pRect->right = ut::Max(pRect->right, rect.right);
        pRect->bottom = ut::Max(pRect->bottom, rect.bottom);
        if (bOver) {
            CalcLineRectImpl(&rect, pTextWriter, L"\n", 1, maxWidth, &bOver);
            pRect->left = ut::Min(pRect->left, rect.left);
            pRect->top = ut::Min(pRect->top, rect.top);
            pRect->right = ut::Max(pRect->right, rect.right);
            pRect->bottom = ut::Max(pRect->bottom, rect.bottom);
        }
    } while (remain > 0);
}
}

namespace {
u32 ExpandBorderColor(u32 color, u32 length) {
    switch (length) {
    case 1:
        color = ((color << 24) | (color << 16) | (color << 8) | 0xF) * 17;
        break;
    case 2:
        color = (((color & 0xF0) << 20) | ((color & 0xF0) << 12) |
                 ((color & 0xF0) << 4) | (color & 0xF)) * 17;
        break;
    case 3:
        color = (((color & 0xF00) << 16) | ((color & 0xF0) << 12) |
                 ((color & 0xF) << 8) | 0xF) * 17;
        break;
    case 4:
        color = (((color & 0xF000) << 12) | ((color & 0xF00) << 8) |
                 ((color & 0xF0) << 4) | (color & 0xF)) * 17;
        break;
    case 6:
        color = (color << 8) | 0xFF;
        break;
    }
    return color;
}
}

TextBox::TextBox(const res::TextBox *pBlock, const ResBlockSet &resBlockSet)
    : Pane(pBlock), mTextColors(), mFontSize() {
    u16 allocStrBufferLen = pBlock->textBufBytes / 2;
    if (allocStrBufferLen > 0) {
        allocStrBufferLen -= 1;
    }
    Init(allocStrBufferLen);
    if (pBlock->textStrBytes >= 2 && mTextBuf) {
        u16 resStringLen = pBlock->textStrBytes / 2 - 1;
        const wchar_t *pBlockText = detail::ConvertOffsToPtr<wchar_t>(pBlock, pBlock->textStrOffset);
        SetString(pBlockText, 0, resStringLen);
    }
    mTextColors[0] = pBlock->textCols[0];
    mTextColors[1] = pBlock->textCols[1];
    mFontSize = pBlock->fontSize;
    mTextPosition = pBlock->textPosition;
    mBits.textAlignment = pBlock->textAlignment;
    mCharSpace = pBlock->charSpace;
    mLineSpace = pBlock->lineSpace;

    int i = pBlock->fontIdx;
    const res::Font *fonts = detail::ConvertOffsToPtr<res::Font>(resBlockSet.pFontList, sizeof(res::FontList));
    const char *fontName = detail::ConvertOffsToPtr<char>(fonts, fonts[i].nameStrOffset);
    ut::Font *pFont = resBlockSet.pResAccessor->GetFont(fontName);
    if (pFont) {
        mpFont = pFont;
    } else {
        void *fontRes = resBlockSet.pResAccessor->GetResource('font', fontName, nullptr);
        if (fontRes) {
            ut::ResFont *pResFont = Layout::NewObj<ut::ResFont>();
            if (pResFont) {
                bool bSucces = pResFont->SetResource(fontRes);
                mpFont = pResFont;
                mBits.bAllocFont = true;
            }
        }
    }
    const u32 *matOffsTbl = detail::ConvertOffsToPtr<u32>(resBlockSet.pMaterialList, sizeof(res::MaterialList));
    const res::Material *pResMaterial =
        detail::ConvertOffsToPtr<res::Material>(resBlockSet.pMaterialList, matOffsTbl[pBlock->materialIdx]);
    mpMaterial = Layout::NewObj<Material>(pResMaterial, resBlockSet);
    mDrawCacheFlags |= 0x10;
}

void TextBox::Init(u16 allocStrLen) {
    mTextBuf = nullptr;
    mTextBufBytes = 0;
    mTextLen = 0;
    mpFont = nullptr;
    mFontSize = Size(0.0f, 0.0f);
    mLineSpace = 0.0f;
    mCharSpace = 0.0f;
    mpTagProcessor = nullptr;
    SetTextPositionH(1);
    SetTextPositionV(1);
    memset(&mBits, 0, sizeof(mBits));
    if (allocStrLen != 0) {
        AllocStringBuffer(allocStrLen);
    }
    mCachedPaneSize.width = 0.0f;
    mCachedPaneSize.height = 0.0f;
    mBorderWidth = 0.0f;
    mpDisplayListBuffer = Layout::NewArray<u32>(2048);
    GDInitGDLObj(&mDisplayList, mpDisplayListBuffer, 8192);
}

void TextBox::UpdateBorderFromExtUserData() {
    const res::ExtUserData* pData = FindExtUserDataByName("border-width");
    if (pData && static_cast<int>(pData->type) == 2 && pData->dataNum >= 1) {
        mBorderWidth = *detail::ConvertOffsToPtr<f32>(pData, pData->dataOffset);
    }
    pData = FindExtUserDataByName("border-color");
    if (pData && pData->type == 0 && pData->dataNum != 0) {
        const char* pString = detail::ConvertOffsToPtr<char>(pData, pData->dataOffset);
        char* pEnd;
        u32 color = strtoul(pString, &pEnd, 16);
        mBorderColors[0] = ExpandBorderColor(color, pEnd - pString);
        while (*pEnd == ' ') { ++pEnd; }
        if (*pEnd != '\0') {
            pString = pEnd;
            color = strtoul(pString, &pEnd, 16);
            u32 length = pEnd - pString;
            if (length != 0) { mBorderColors[1] = ExpandBorderColor(color, length); }
            else { mBorderColors[1] = mBorderColors[0]; }
        } else { mBorderColors[1] = mBorderColors[0]; }
        mBits.bEnableBorder = true;
    }
}

void TextBox::SetExtUserDataList(const res::ExtUserDataList* pList) {
    mpExtUserDataList = pList;
    UpdateBorderFromExtUserData();
}

TextBox::~TextBox() {
    SetFont(nullptr);
    if (mpMaterial && !mpMaterial->IsUserAllocated()) {
        Layout::DeleteObj(mpMaterial);
        mpMaterial = nullptr;
    }
    FreeStringBuffer();
    Layout::DeletePrimArray(static_cast<u32*>(mpDisplayListBuffer));
}

ut::Color TextBox::GetVtxColor(u32 index) const {
    return mTextColors[index / 2];
}

void TextBox::SetVtxColor(u32 index, ut::Color color) {
    SetTextColor(index / 2, color);
}

u8 TextBox::GetVtxColorElement(u32 index) const {
    return reinterpret_cast<const u8*>(mTextColors)[((index / 2) & ~3) + (index & 3)];
}

void TextBox::SetVtxColorElement(u32 index, u8 value) {
    reinterpret_cast<u8*>(mTextColors)[((index / 2) & ~3) + (index & 3)] = value;
}

ut::Rect TextBox::GetTextDrawRect() const {
    ut::TextWriterBase<wchar_t> temp;
    return GetTextDrawRect(&temp);
}

void TextBox::DrawSelf(const DrawInfo& rInfo) {
    if (!mTextBuf || !mpFont || !mpMaterial) { return; }
    ut::TextWriterBase<wchar_t> writer;
    ut::Rect textRect = GetTextDrawRect(&writer);
    ut::Color topColor = detail::MultipleAlpha(mTextColors[0], mGlbAlpha);
    ut::Color bottomColor = detail::MultipleAlpha(mTextColors[1], mGlbAlpha);
    writer.SetGradationMode(topColor != bottomColor ? ut::CharWriter::GRADMODE_V : ut::CharWriter::GRADMODE_NONE);
    writer.SetTextColor(topColor, bottomColor);
    ut::Color minColor = GetColor(mpMaterial->GetTevColor(0));
    ut::Color maxColor = GetColor(mpMaterial->GetTevColor(1));
    writer.SetColorMapping(minColor, maxColor);
    GXSetAlphaCompare(GX_ALWAYS, 0, GX_AOP_AND, GX_ALWAYS, 0);
    f32 alignMag = GetTextAlignMag();
    const wchar_t* pPosition = mTextBuf;
    writer.SetCursor(textRect.left, -textRect.top);
    f32 textWidth = textRect.GetWidth();
    if (mpTagProcessor || mTextLen > 61) {
        LoadMtx(rInfo);
        writer.SetupGX();
        int remaining = mTextLen;
        while (remaining > 0) {
            bool over;
            f32 lineWidth;
            int count = CalcLineStrNum(&lineWidth, &writer, pPosition, remaining, mSize.width, &over);
            writer.SetCursorX(textRect.left + alignMag * (textWidth - lineWidth));
            writer.PrintMutable(pPosition, count);
            if (over) { writer.PrintMutable(L"\n", 1); }
            pPosition += count;
            remaining -= count;
        }
    } else {
        if (mDrawCacheFlags != 0) {
            GDSetCurrent(&mDisplayList);
            GDSetCurrOffset(0);
            int remaining = mTextLen;
            while (remaining > 0) {
                bool over;
                f32 lineWidth;
                int count = CalcLineStrNum(&lineWidth, &writer, pPosition, remaining, mSize.width, &over);
                writer.SetCursorX(textRect.left + alignMag * (textWidth - lineWidth));
                writer.PrintMutableToDisplayList(pPosition, count);
                if (over) { writer.PrintMutableToDisplayList(L"\n", 1); }
                pPosition += count;
                remaining -= count;
            }
            GDPadCurr32();
            GDFlushCurrToMem();
            GDSetCurrent(nullptr);
            mDrawCacheFlags = 0;
        }
        if (mBorderWidth != 0.0f) {
            if (mBits.bEnableBorder) {
                ut::Color borderTop = detail::MultipleAlpha(mBorderColors[0], mGlbAlpha);
                ut::Color borderBottom = detail::MultipleAlpha(mBorderColors[1], mGlbAlpha);
                writer.SetTextColor(borderTop, borderBottom);
            }
            writer.SetupGXForDisplayList();
            math::MTX34 viewMatrix;
            const math::MTX34* pMatrix;
            if (rInfo.IsMultipleViewMtxOnDraw()) {
                math::MTX34Mult(&viewMatrix, &rInfo.GetViewMtx(), &mGlbMtx);
                pMatrix = &viewMatrix;
            } else { pMatrix = &mGlbMtx; }
            math::MTX34 translated;
            f32 offsets[3][3] = {
                {-0.707f, 0.0f, 0.707f},
                {-1.0f, 0.0f, 1.0f},
                {-0.707f, 0.0f, 0.707f}
            };
            GXSetCurrentMtx(GX_PNMTX0);
            for (int row = 0; row < 3; ++row) {
                for (int column = 0; column < 3; ++column) {
                    if (row == 1 && column == 1) { continue; }
                    math::VEC3 offset;
                    offset.x = mBorderWidth * offsets[column][row];
                    offset.y = mBorderWidth * offsets[row][column];
                    offset.z = 0.0f;
                    PSMTXMultVecSR(*pMatrix, offset, offset);
                    math::MTX34Trans(&translated, &offset, pMatrix);
                    ReverseYAxis(&translated);
                    GXLoadPosMtxImm(translated, GX_PNMTX0);
                    GXCallDisplayList(mDisplayList.start, mDisplayList.ptr - mDisplayList.start);
                }
            }
            if (mBits.bEnableBorder) { writer.SetTextColor(topColor, bottomColor); }
        }
        LoadMtx(rInfo);
        writer.SetupGXForDisplayList();
        GXCallDisplayList(mDisplayList.start, mDisplayList.ptr - mDisplayList.start);
    }
}

void TextBox::AllocStringBuffer(u16 minLen) {
    if (minLen == 0) { return; }
    u32 allocLen = minLen;
    u32 textBufBytes = (minLen + 1) * sizeof(wchar_t);
    if (textBufBytes > mTextBufBytes) {
        FreeStringBuffer();
        mTextBuf = Layout::NewArray<wchar_t>(allocLen + 1);
        if (mTextBuf) { mTextBufBytes = textBufBytes; }
    }
}

void TextBox::FreeStringBuffer() {
    if (mTextBuf) {
        Layout::DeletePrimArray(mTextBuf);
        mTextBuf = nullptr;
        mTextBufBytes = 0;
    }
}

u16 TextBox::SetString(const wchar_t* pString, u16 dstIndex) {
    return SetStringImpl(pString, dstIndex, wcslen(pString));
}

u16 TextBox::SetString(const wchar_t* pString, u16 dstIndex, u16 stringLength) {
    return SetStringImpl(pString, dstIndex, stringLength);
}

u16 TextBox::SetStringImpl(const wchar_t* pString, u16 dstIndex, u32 stringLength) {
    if (!mTextBuf) { return 0; }
    u16 bufferLength = GetStringBufferLength();
    if (dstIndex >= bufferLength) { return 0; }
    u32 copyLength = ut::Min<u32>(stringLength, bufferLength - dstIndex);
    bool changed = false;
    if (memcmp(&mTextBuf[dstIndex], pString, copyLength * sizeof(wchar_t)) != 0) {
        changed = true;
        memcpy(&mTextBuf[dstIndex], pString, copyLength * sizeof(wchar_t));
    }
    mTextLen = dstIndex + copyLength;
    if (mTextBuf[mTextLen] != L'\0') {
        mTextBuf[mTextLen] = L'\0';
        changed = true;
    }
    if (changed) { mDrawCacheFlags |= 0x20; }
    return copyLength;
}

const ut::Font* TextBox::GetFont() const { return mpFont; }

void TextBox::SetFont(const ut::Font *pFont) {
    if (mBits.bAllocFont) {
        Layout::DeleteObj(const_cast<ut::Font *>(mpFont));
        mBits.bAllocFont = false;
    }
    mpFont = pFont;
    mDrawCacheFlags |= 0x40;
    if (mpFont) {
        SetFontSize(Size(mpFont->GetWidth(), mpFont->GetHeight()));
    } else {
        SetFontSize(Size(0.0f, 0.0f));
    }
}

void TextBox::LoadMtx(const DrawInfo &drawInfo) {
    math::MTX34 mtx;
    if (drawInfo.IsMultipleViewMtxOnDraw()) {
        math::MTX34Mult(&mtx, &drawInfo.GetViewMtx(), &mGlbMtx);
    } else {
        math::MTX34Copy(&mtx, &mGlbMtx);
    }

    ReverseYAxis(&mtx);

    GXLoadPosMtxImm(mtx, GX_PNMTX0);
    GXSetCurrentMtx(GX_PNMTX0);
}

ut::Rect TextBox::GetTextDrawRect(ut::TextWriterBase<wchar_t>* pWriter) const {
    pWriter->SetCursor(0.0f, 0.0f);
    pWriter->SetFont(*mpFont);
    pWriter->SetFontSize(mFontSize.width, mFontSize.height);
    pWriter->SetLineSpace(mLineSpace);
    pWriter->SetCharSpace(mCharSpace);
    if (mpTagProcessor) { pWriter->SetTagProcessor(mpTagProcessor); }
    if (mSize.width != mCachedPaneSize.width || mSize.height != mCachedPaneSize.height) {
        mCachedPaneSize = mSize;
        mDrawCacheFlags |= 0x80;
    }
    if (mDrawCacheFlags != 0) {
        ut::Rect& textRect = mCachedTextDrawRect;
        CalcStringRect(&textRect, pWriter, mTextBuf, mTextLen, mSize.width);
        Size textSize(textRect.GetWidth(), textRect.GetHeight());
        math::VEC2 ltPos = GetVtxPos();
        ltPos.x += (mSize.width - textSize.width) * GetTextMagH();
        ltPos.y -= (mSize.height - textSize.height) * GetTextMagV();
        textRect.left = ltPos.x;
        textRect.top = ltPos.y;
        textRect.right = ltPos.x + textSize.width;
        textRect.bottom = ltPos.y - textSize.height;
    }
    return mCachedTextDrawRect;
}

f32 TextBox::GetTextMagH() const {
    switch (GetTextPositionH()) {
        default: return 0.0f;
        case 1:  return 0.5f;
        case 2:  return 1.0f;
    }
}

f32 TextBox::GetTextMagV() const {
    switch (GetTextPositionV()) {
        default: return 0.0f;
        case 1:  return 0.5f;
        case 2:  return 1.0f;
    }
}

f32 TextBox::GetTextAlignMag() const {
    switch (mBits.textAlignment) {
        default: return GetTextMagH();
        case 1:  return 0.0f;
        case 2:  return 0.5f;
        case 3:  return 1.0f;
    }
}
}
}
