#include <game/CColorBtnTagProc.h>
#include <nw4r/lyt/lyt_textBox.h>
#include <nw4r/ut/ut_TextWriterBase.h>

CColorBtnTagProc CColorBtnTagProc::sInstance;
nw4r::ut::Color CColorBtnTagProc::sStartColors[5];
nw4r::ut::Color CColorBtnTagProc::sEndColors[5];

void CColorBtnTagProc::Attach(nw4r::lyt::Pane* pPane)
{
    if (pPane != nullptr) {
        nw4r::lyt::TextBox* pTextBox =
            nw4r::ut::DynamicCast<nw4r::lyt::TextBox*>(pPane);
        if (pTextBox != nullptr) {
            pTextBox->SetTagProcessor(&sInstance);
        }
    }
}

CColorBtnTagProc::~CColorBtnTagProc() {}

CColorBtnTagProc::Operation CColorBtnTagProc::Process(u16 ch,
                                                   ContextType* pContext)
{
    nw4r::ut::TextWriterBase<wchar_t>* pWriter = pContext->writer;
    if (ch == 1) {
        int tag = *pContext->str++;
        switch (tag) {
        case 2: {
            u16 glyph = *pContext->str;
            mSavedGradation = pWriter->GetGradationMode();
            mSavedStart = pWriter->GetTextColorStart();
            mSavedEnd = pWriter->GetTextColorEnd();
            pWriter->SetGradationMode(nw4r::ut::CharWriter::GRADMODE_V);

            switch (glyph) {
            case 0xE103:
                pWriter->SetTextColor(sStartColors[0], sEndColors[0]);
                break;
            case 0xE104:
                pWriter->SetTextColor(sStartColors[1], sEndColors[1]);
                break;
            case 0xE109:
                pWriter->SetTextColor(sStartColors[2], sEndColors[2]);
                break;
            case 0xE10C:
                pWriter->SetTextColor(sStartColors[3], sEndColors[3]);
                break;
            case 0xE102:
            case 0xE105:
            case 0xE106:
            case 0xE107:
            case 0xE108:
            case 0xE10A:
            case 0xE10B:
                pWriter->SetTextColor(sStartColors[4], sEndColors[4]);
                break;
            }
            return OPERATION_DEFAULT;
        }
        case 1:
            pWriter->SetTextColor(mSavedStart, mSavedEnd);
            pWriter->SetGradationMode(mSavedGradation);
            return OPERATION_DEFAULT;
        }
    }
    return TagProcessorBase<wchar_t>::Process(ch, pContext);
}

CColorBtnTagProc::Operation CColorBtnTagProc::CalcRect(
    nw4r::ut::Rect* pRect, u16 ch, ContextType* pContext)
{
    if (ch == 1) {
        u16 tag = *pContext->str++;
        if (static_cast<u32>(tag - 1) <= 1) {
            return OPERATION_DEFAULT;
        }
    }
    return TagProcessorBase<wchar_t>::CalcRect(pRect, ch, pContext);
}

void CColorBtnTagProc::SetColor(int index,
                               nw4r::ut::Color start,
                               nw4r::ut::Color end)
{
    sStartColors[index] = start;
    sEndColors[index] = end;
}
