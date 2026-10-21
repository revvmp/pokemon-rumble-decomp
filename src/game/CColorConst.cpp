#include <game/CColorBtnTagProc.h>
#include <game/IColorConst.h>
#include <game/LayoutResources.h>
#include <nw4r/lyt/lyt_picture.h>
#include <nw4r/lyt/lyt_textBox.h>
#include <stdexcept>
#include <string>

#include <game/detail/CColorConst.h>

namespace std {

template <class charT, class traits, class Allocator>
void basic_string<charT, traits, Allocator>::reserve(size_type res_arg)
{
    if (res_arg > 0x7FFFFFFE) {
        throw length_error("basic_string::reserve length_error");
    }
    bool wasLong = is_long();
    size_type length;
    size_type capacity;
    if (!wasLong) {
        length = rep_.s.size_;
        capacity = sizeof(long_rep) - 1;
    } else {
        length = rep_.l.size_;
        capacity = rep_.l.cap_;
    }
    if (res_arg < length) {
        res_arg = length;
    }
    size_type newCapacity = sizeof(long_rep) - 1;
    if (res_arg + 1 > sizeof(long_rep) - 1) {
        newCapacity = (res_arg + 16) & ~15;
    }
    if (newCapacity == capacity) {
        return;
    }
    charT* pNew;
    bool nowLong;
    const charT* pOld;
    if (newCapacity == sizeof(long_rep) - 1) {
        pOld = rep_.l.data_;
        pNew = rep_.s.data_;
        nowLong = false;
    } else {
        if (newCapacity > capacity) {
            pNew = this->Allocator::allocate(newCapacity);
        } else {
            try {
                pNew = this->Allocator::allocate(newCapacity);
            } catch (...) {
                return;
            }
        }
        nowLong = true;
        pOld = wasLong ? rep_.l.data_ : rep_.s.data_;
    }
    memmove(pNew, pOld, length);
    pNew[length] = 0;
    if (wasLong) {
        this->Allocator::deallocate(const_cast<charT*>(pOld), capacity);
    }
    rep_.l.is_long_ = nowLong;
    if (!nowLong) {
        rep_.s.size_ = length;
    } else {
        rep_.l.data_ = pNew;
        rep_.l.size_ = length;
        rep_.l.cap_ = newCapacity;
    }
}

template <class charT, class traits, class Allocator>
basic_string<charT, traits, Allocator> operator+(const basic_string<charT, traits, Allocator>& lhs, const charT* rhs)
{
    basic_string<charT, traits, Allocator> result;
    size_t rhsLength = traits::length(rhs);
    size_t lhsLength;
    const charT* lhsData;
    lhs.get_buffer(lhsData, lhsLength);
    result.reserve(lhsLength + rhsLength);
    result.replace(0, 0, lhsData, lhsData + lhsLength).replace(lhsLength, 0, rhs, rhs + rhsLength);
    return result;
}

}

namespace {
std::tr1::shared_ptr<CColorConst> sColorConst;
}

inline void CColorConst::LoadPalettes(CLayout& rLayout)
{
    static const char* sTextPaneNames[CColorConst::PaletteCount] = {
        "PIINAME_NORMAL", "PIINAME_DEAD", "PIINAME_PREFIXED", "PIINAME_RARE",
        "PIINAME_MINE",   "PIINAME_OTHER_P", "GC_BUTTON_A",   "GC_BUTTON_B",
        "GC_STICK_SUB",   "GC_TRIGGER_Z", "GC_OTHER",
    };

    static const char* sPicturePaneNames[CColorConst::PaletteCount] = {
        nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr,
        "TYPE_NORMAL", "TYPE_BATTLE", "TYPE_HIKOU", "TYPE_POISON", "TYPE_JIMEN", "TYPE_IWA", "TYPE_MUSHI",
        "TYPE_GHOST", "TYPE_METAL", nullptr, "TYPE_FIRE", "TYPE_WATER", "TYPE_KUSA", "TYPE_ELECTRIC",
        "TYPE_SP", "TYPE_KOORI", "TYPE_DRAGON", "TYPE_AKU",
    };

    for (int i = 0; i < PaletteCount; i++) {
        if (sTextPaneNames[i] != nullptr) {
            nw4r::lyt::Pane* pPane = rLayout.FindPane(sTextPaneNames[i]);
            nw4r::lyt::TextBox* pTextBox = nw4r::ut::DynamicCast<nw4r::lyt::TextBox*>(pPane);
            mColors[i][0] = mColors[i][1] = pTextBox->GetTextColor(0);
            mColors[i][2] = mColors[i][3] = pTextBox->GetTextColor(1);
        }
    }
    for (int i = 0; i < PaletteCount; i++) {
        if (sPicturePaneNames[i] != nullptr) {
            nw4r::lyt::Pane* pPane = rLayout.FindPane(sPicturePaneNames[i]);
            nw4r::lyt::Picture* pPicture = nw4r::ut::DynamicCast<nw4r::lyt::Picture*>(pPane);
            mColors[i][0] = pPicture->GetVtxColor(0);
            mColors[i][1] = pPicture->GetVtxColor(1);
            mColors[i][2] = pPicture->GetVtxColor(2);
            mColors[i][3] = pPicture->GetVtxColor(3);
        }
    }
}

void InitializeColorConstants(bool background)
{
    sColorConst.reset(new CColorConst);
    CColorConst* pColorConst = sColorConst.get();
    {
        std::tr1::shared_ptr<CLayout::FontMap> fonts = GetDefaultFontMap();
        CLayout layout;
        layout.Load(gSharedLayoutArchives, "colorconst", fonts, background);
        pColorConst->LoadPalettes(layout);
    }
    for (int i = 0; i < 5; i++) {
        int palette = i + 6;
        CColorBtnTagProc::SetColor(i, sColorConst->GetTextColor(palette, 0), sColorConst->GetTextColor(palette, 1));
    }
}

IColorConst* GetColorConstants()
{
    return sColorConst.get();
}

void FinalizeColorConstants()
{
    sColorConst.reset();
}

nw4r::ut::Color CColorConst::GetTextColor(int index, int end) const
{
    return mColors[index][end * 2];
}

nw4r::ut::Color CColorConst::GetVertexColor(int index, int end) const
{
    return mColors[index][end];
}

bool CColorConst::ApplyVertexColor(nw4r::lyt::Pane* pPane, int index, int mode) const
{
    if (pPane == nullptr) {
        return false;
    }
    nw4r::lyt::Picture* pPicture = nw4r::ut::DynamicCast<nw4r::lyt::Picture*>(pPane);
    if (pPicture != nullptr) {
        if (mode == 4) {
            pPicture->SetVtxColor(0, GetVertexColor(index, 0));
            pPicture->SetVtxColor(1, GetVertexColor(index, 1));
            pPicture->SetVtxColor(2, GetVertexColor(index, 2));
            pPicture->SetVtxColor(3, GetVertexColor(index, 3));
        } else {
            pPicture->SetVtxColor(mode, GetVertexColor(index, mode));
        }
        return true;
    }
    return false;
}

bool CColorConst::ApplyVertexColor(CLayout* pLayout, const char* pName, int index, int mode) const
{
    return ApplyVertexColor(pLayout->FindPane(pName), index, mode);
}

bool CColorConst::ApplyTextColor(nw4r::lyt::Pane* pPane, int index, int mode) const
{
    if (pPane == nullptr) {
        return false;
    }
    nw4r::lyt::TextBox* pTextBox = nw4r::ut::DynamicCast<nw4r::lyt::TextBox*>(pPane);
    if (pTextBox != nullptr) {
        if (mode == 2) {
            pTextBox->SetTextColor(0, GetTextColor(index, 0));
            pTextBox->SetTextColor(1, GetTextColor(index, 1));
        } else {
            pTextBox->SetTextColor(mode, GetTextColor(index, mode));
        }
        return true;
    }
    return false;
}

bool CColorConst::ApplyTextColor(CLayout* pLayout, const char* pName, int index, int mode) const
{
    bool applied = ApplyTextColor(pLayout->FindPane((std::string(pName) + "_c").c_str()), index, mode);
    if (applied) {
        return true;
    }
    return ApplyTextColor(pLayout->FindPane(pName), index, mode);
}
