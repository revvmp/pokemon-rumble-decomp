#include <nw4r/ut/ut_TagProcessorBase.h>
#include <nw4r/ut/ut_Rect.h>
#include <nw4r/ut/ut_TextWriterBase.h>

namespace nw4r {
namespace ut {

template <typename T>
TagProcessorBase<T>::TagProcessorBase() {}

template <typename T>
TagProcessorBase<T>::~TagProcessorBase() {}

template <typename T>
typename TagProcessorBase<T>::Operation TagProcessorBase<T>::Process(u16 ch, ContextType* pCtx) {
    switch (ch) {
    case '\n':
        ProcessLinefeed(pCtx);
        return OPERATION_NEXT_LINE;
    case '\t':
        ProcessTab(pCtx);
        return OPERATION_NO_CHAR_SPACE;
    }

    return OPERATION_DEFAULT;
}

template <typename T>
typename TagProcessorBase<T>::Operation TagProcessorBase<T>::CalcRect(Rect* pRect, u16 ch, ContextType* pCtx) {
    switch (ch) {
    case '\n': {
        const TextWriterBase<T>& rWriter = *pCtx->writer;
        pRect->right = rWriter.GetCursorX();
        pRect->top = rWriter.GetCursorY();
        ProcessLinefeed(pCtx);
        pRect->left = rWriter.GetCursorX();
        pRect->bottom = rWriter.GetCursorY() + pCtx->writer->GetFontHeight();
        pRect->Normalize();
        return OPERATION_NEXT_LINE;
    }
    case '\t': {
        const TextWriterBase<T>& rWriter = *pCtx->writer;
        pRect->left = rWriter.GetCursorX();
        ProcessTab(pCtx);
        pRect->right = rWriter.GetCursorX();
        pRect->top = rWriter.GetCursorY();
        pRect->bottom = pRect->top + rWriter.GetFontHeight();
        pRect->Normalize();
        return OPERATION_NO_CHAR_SPACE;
    }
    }

    return OPERATION_DEFAULT;
}

template <typename T>
void TagProcessorBase<T>::ProcessLinefeed(ContextType* pCtx) {
    TextWriterBase<T>& rWriter = *pCtx->writer;
    f32 x = pCtx->x;
    f32 y = rWriter.GetCursorY() + rWriter.GetLineHeight();

    rWriter.SetCursorX(x);
    rWriter.SetCursorY(y);
}

template <typename T>
void TagProcessorBase<T>::ProcessTab(ContextType* pCtx) {
    TextWriterBase<T>& rWriter = *pCtx->writer;
    int tabWidth = rWriter.GetTabWidth();

    if (tabWidth <= 0) {
        return;
    }

    f32 charWidth = rWriter.IsWidthFixed() ? rWriter.GetFixedWidth() : rWriter.GetFontWidth();
    f32 offset = rWriter.GetCursorX() - pCtx->x;
    f32 tabPixel = tabWidth * charWidth;
    int tabCount = static_cast<int>(offset / tabPixel) + 1;
    f32 x = pCtx->x + tabPixel * tabCount;

    rWriter.SetCursorX(x);
}

template class TagProcessorBase<char>;
template class TagProcessorBase<wchar_t>;

}
}
