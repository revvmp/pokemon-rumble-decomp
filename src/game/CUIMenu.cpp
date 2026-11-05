#pragma section code_type ".text.menu_prefix"
#pragma section code_type ".text.menu_notify"
#pragma section code_type ".text"
#define CUIMENU_EMISSION_LAYOUT
#include <memory>
class IUIListItem;
namespace std {
namespace tr1 {
template <> shared_ptr<IUIListItem>::shared_ptr();
template <class T> inline shared_ptr<T>::shared_ptr() : ptr_(nullptr), s_(nullptr) {}
}
}
#include <game/CUIMenuItem.h>
#include <game/CUIMenu.h>
#include <game/CUIManager.h>
#include <game/CFiber.h>
#include <nw4r/lyt/lyt_textbox.h>
#include <game/StringFormat.h>
#include <algorithm>
#include <math.h>

namespace std {
namespace tr1 {
template <> __declspec(section ".text.menu_prefix") inline
shared_ptr<CUIMenuItem>::operator int bool_type::*() const {
    return ptr_ ? &bool_type::dummy_ : nullptr;
}
}
}

#pragma section code_type ".text.menu_prefix"

__declspec(section ".text.menu_prefix") CUIMenuItem::CUIMenuItem(const std::wstring& rTitle, int kind, float width) : mWidth(width), mTitle(rTitle), mKind(kind) {}

__declspec(section ".text.menu_prefix") CUIMenuItem::~CUIMenuItem() {}

__declspec(section ".text.menu_prefix") bool CUIMenuItem::IsSelectable() const { return mKind >= 0; }

__declspec(section ".text.menu_prefix") void CUIMenuItem::Update(CUIList*, CLayout* pLayout) {
    bool selectable = IsSelectable();
    pLayout->SetPaneVisible("TITLE_c", selectable);
    pLayout->SetPaneVisible("TITLE_DISABLED_c", !selectable);
    if (selectable) pLayout->SetText("TITLE", mTitle.c_str());
    else pLayout->SetText("TITLE_DISABLED", mTitle.c_str());
}

__declspec(section ".text.menu_prefix") CUIMenu::CUIMenu() : mLayoutReady(false), mCursorRow(-1) {}

__declspec(section ".text.menu_prefix") CUIMenu::~CUIMenu() {}

__declspec(section ".text.menu_prefix") void CUIMenu::Load(int columns, int rows, const char* pArchive, const char* pName, const std::tr1::shared_ptr<CLayout::FontMap>& rFonts, int background) {
    const char* archives[] = {pArchive, nullptr};
    Load(columns, rows, archives, pName, rFonts, background);
}

__declspec(section ".text.menu_prefix") void CUIMenu::Load(int columns, int rows, const char* const* pArchives, const char* pName, const std::tr1::shared_ptr<CLayout::FontMap>& rFonts, int background) {
    CUIList::Load(pArchives, pName, rFonts, background);
    CLayout* pLayout = GetLayout();
    pLayout->CalculateMtx(true);
    mColumnCount = columns;
    mRowCount = rows;
    int layoutCount = GetLayoutCount();
    NErrMsg::PaneRectangle itemBounds;
    float itemWidth;
    for (int i = 0; i < layoutCount; ++i) {
        nw4r::lyt::Pane* pItem = pLayout->FindPane(FormatString("ITEM%d", i).c_str());
        NErrMsg::PaneRectangle bounds = NErrMsg::GetPaneRectangle(pItem);
        if (i == 0) {
            itemBounds = bounds;
            itemWidth = pItem->GetSize().width;
            mCellHeight = pItem->GetSize().height;
        } else itemBounds = NErrMsg::UnionRectangles(itemBounds, bounds);
    }
    mItemCenter.x = (itemBounds.left + itemBounds.right) / 2.0f;
    mItemCenter.y = (itemBounds.bottom + itemBounds.top) / 2.0f;
    float width = itemBounds.GetWidth();
    float height = itemBounds.GetHeight();
    nw4r::lyt::Pane* pBackground = pLayout->FindPane("BK");
    if (pBackground == nullptr) pBackground = pLayout->FindPane("BK0");
    NErrMsg::PaneRectangle backgroundBounds = NErrMsg::GetPaneRectangle(pBackground);
    mBounds.left = backgroundBounds.left + width / 2.0f;
    mBounds.right = backgroundBounds.right - width / 2.0f;
    mBounds.bottom = backgroundBounds.bottom + height / 2.0f;
    mBounds.top = backgroundBounds.top - height / 2.0f;
    nw4r::lyt::Pane* pTitle = pLayout->FindPane("TITLE");
    if (pTitle) {
        mTitlePosition.x = pTitle->GetTranslate().x;
        mTitlePosition.y = pTitle->GetTranslate().y;
        switch (pTitle->GetBasePositionH()) {
        case 0: mTitlePosition.x += width / 2.0f; break;
        case 2: mTitlePosition.x -= width / 2.0f; break;
        }
        switch (pTitle->GetBasePositionV()) {
        case 2: mTitlePosition.y += height / 2.0f; break;
        case 0: mTitlePosition.y -= height / 2.0f; break;
        }
        mMinimumWidth = 0.0f;
    }
    int fittedColumns = static_cast<int>(roundf(width / itemWidth));
    int fittedRows = static_cast<int>(roundf(height / mCellHeight));
    fittedColumns = std::max(1, fittedColumns);
    fittedRows = std::max(1, fittedRows);
    if (fittedColumns * fittedRows < layoutCount) fittedColumns = (layoutCount + fittedRows - 1) / fittedRows;
    if (fittedColumns >= 2) mColumnSpacing = (width - itemWidth * fittedColumns) / (fittedColumns - 1);
    else mColumnSpacing = 0.0f;
    if (fittedRows >= 2) mRowSpacing = (height - mCellHeight * fittedRows) / (fittedRows - 1);
    else mRowSpacing = 0.0f;
    mLayoutReady = true;
    UpdateSelection();
}

__declspec(section ".text.menu_prefix") void CUIMenu::UpdateSelection() {
    if (mLayoutReady) {
        CUIList::UpdateSelection();
        int first = GetFirstVisibleIndex();
        int visible = std::min(static_cast<int>(mItems.size()) - first, static_cast<int>(GetLayoutCount()));
        CLayout* pLayout = GetLayout();
        float itemWidth = 0.0f;
        for (unsigned long i = 0; i < visible; ++i) {
            std::tr1::shared_ptr<CUIMenuItem> item = std::tr1::dynamic_pointer_cast<CUIMenuItem>(mItems[i]);
            if (item) itemWidth = std::max(itemWidth, item->GetWidth());
        }
        float gridWidth = mColumnCount * (itemWidth + mColumnSpacing) - mColumnSpacing;
        float width = std::max(gridWidth, mMinimumWidth);
        float height = mRowCount * (mCellHeight + mRowSpacing) - mRowSpacing;
        NErrMsg::PaneRectangle bounds;
        bounds.left = mBounds.left - width / 2.0f;
        bounds.right = mBounds.right + width / 2.0f;
        bounds.bottom = mBounds.bottom - height / 2.0f;
        bounds.top = mBounds.top + height / 2.0f;
        nw4r::lyt::Pane* pBackground = pLayout->FindPane("BK");
        if (pBackground) NErrMsg::SetPaneRectangle(pBackground, bounds);
        else {
            for (int i = 0; ; ++i) {
                nw4r::lyt::Pane* pPane = pLayout->FindPane(FormatString("BK%d", i).c_str());
                if (!pPane) break;
                NErrMsg::SetPaneRectangle(pPane, bounds);
            }
        }
        nw4r::lyt::Pane* pTitle = pLayout->FindPane("TITLE");
        if (pTitle) {
            nw4r::math::VEC3 position;
            position.x = mTitlePosition.x;
            position.y = mTitlePosition.y;
            position.z = pTitle->GetTranslate().z;
            switch (pTitle->GetBasePositionH()) {
            case 0: position.x -= width / 2.0f; break;
            case 2: position.x += width / 2.0f; break;
            }
            switch (pTitle->GetBasePositionV()) {
            case 2: position.y -= height / 2.0f; break;
            case 0: position.y += height / 2.0f; break;
            }
            pTitle->SetTranslate(position);
        }
        for (int i = 0; i < visible; ++i) {
            nw4r::lyt::Pane* pPane = pLayout->FindPane(FormatString("ITEM%d", i).c_str());
            int column = i / mRowCount;
            int row = i % mRowCount;
            NErrMsg::PaneRectangle item;
            item.left = mItemCenter.x - gridWidth / 2.0f + column * (itemWidth + mColumnSpacing);
            item.right = item.left + itemWidth;
            item.top = mItemCenter.y + height / 2.0f - row * (mCellHeight + mRowSpacing);
            item.bottom = item.top - mCellHeight;
            NErrMsg::SetPaneRectangle(pPane, item);
        }
        pLayout->UpdateAttachedLayouts();
    }
}

__declspec(section ".text.menu_prefix") void CUIMenu::SetTitle(const wchar_t* pTitle) {
    if (pTitle) {
        nw4r::lyt::TextBox* pTextBox = GetLayout()->SetText("TITLE", pTitle);
        if (pTextBox) mMinimumWidth = pTextBox->GetTextDrawRect().GetWidth();
    }
}

#pragma opt_propagation off
__declspec(section ".text.menu_prefix") void CUIMenu::OnInput(const IUIInput* pInput) {
    if (mColumnCount >= 1) {
        int direction = 0;
        unsigned long trigger = pInput->GetTrig();
        unsigned long buttons = trigger | pInput->GetRepeat();
        if (buttons & 2) direction = 1;
        else if (buttons & 8) direction = -1;
        else if (buttons & 5) mCursorRow = -1;
        if (direction != 0) {
            const int itemCount = mItems.size();
            int paddedCount = ((mRowCount + itemCount - 1) / mRowCount) * mRowCount;
            int selected = GetSelectedIndex();
            int candidate = selected;
            if (mCursorRow < 0) mCursorRow = candidate % mRowCount;
            else if (mCursorRow < mRowCount) candidate = mCursorRow + (selected / mRowCount) * mRowCount;
            while (true) {
                candidate += mRowCount * direction;
                if (mWrapSelection && ((direction > 0 && (trigger & 2)) || (direction < 0 && (trigger & 8)))) candidate = (candidate + paddedCount) % paddedCount;
                if (candidate < 0 || candidate >= paddedCount || selected == candidate) break;
                int columnBegin = (candidate / mRowCount) * mRowCount;
                int columnEnd = std::min(columnBegin + mRowCount, itemCount);
                if (columnBegin <= candidate && candidate < columnEnd) {
                    std::tr1::shared_ptr<IUIListItem> item = GetItem(candidate);
                    if (item && item->IsSelectable()) {
                        SetSelectedIndex(candidate);
                        goto done;
                    }
                }
                for (int distance = 1; distance < mRowCount; ++distance) {
                    int below = candidate + distance;
                    int above = candidate - distance;
                    if (columnBegin <= below && below < columnEnd) {
                        std::tr1::shared_ptr<IUIListItem> item = GetItem(below);
                        if (item && item->IsSelectable()) {
                            SetSelectedIndex(below);
                            goto done;
                        }
                    }
                    if (columnBegin <= above && above < columnEnd) {
                        std::tr1::shared_ptr<IUIListItem> item = GetItem(above);
                        if (item && item->IsSelectable()) {
                            SetSelectedIndex(above);
                            goto done;
                        }
                    }
                }
            }
            return;
        }
    }
    CUIList::OnInput(pInput);
done:
    return;
}

#pragma opt_propagation reset

namespace {

std::tr1::shared_ptr<IUIListItem> RunMenuList(const wchar_t* pTitle, std::vector<std::tr1::shared_ptr<IUIListItem> >& rItems, int columns, int rows, int defaultIndex, const MenuCallback* pCallbacks, IUIInput* pInput, std::tr1::shared_ptr<CUIManager> manager, const char* const* pArchives, const char* pLayoutName, const std::tr1::shared_ptr<CLayout::FontMap>& rFonts, int background);
void NotifyCallbacks(const BUI::CCallbackListOwner& rOwner, BUI* const& rSender);

}

__declspec(section ".text.menu_prefix") int RunUIMenu(const UIMenuParam& rParam, CUIMenuSoundCallback& rCallbacks, IUIInput* pInput, std::tr1::shared_ptr<CUIManager> manager, std::tr1::shared_ptr<CLayout::FontMap> fonts, bool background) {
    std::tr1::shared_ptr<CLayout> pMeasure(new CLayout());
    pMeasure->Load(rParam.mpArchives, (std::string(rParam.mpLayoutName) + "_item").c_str(), fonts, background);
    NErrMsg::PaneRectangle titleBounds = NErrMsg::GetPaneRectangle(pMeasure->FindPane("TITLE_SIZE"));
    NErrMsg::PaneRectangle itemBounds = NErrMsg::GetPaneRectangle(pMeasure->FindPane("SIZE"));
    float padding = itemBounds.GetWidth() - titleBounds.GetWidth();
    std::vector<std::tr1::shared_ptr<IUIListItem> > items;
    items.reserve(rParam.mItemCount);
    for (unsigned long i = 0; i < rParam.mItemCount; ++i) {
        const UIMenuItem& rItem = rParam.mpItems[i];
        float width = padding + pMeasure->SetText("TITLE", rItem.mText)->GetTextDrawRect().GetWidth();
        std::tr1::shared_ptr<CUIMenuItem> pItem(new CUIMenuItem(std::wstring(rItem.mText), rItem.mKind, width));
        items.push_back(pItem);
    }
    std::tr1::shared_ptr<IUIListItem> selected = RunMenuList(rParam.mTitle, items, rParam.mColumnCount, rParam.mRowCount, rParam.mDefaultIndex, &rCallbacks, pInput, manager, rParam.mpArchives, rParam.mpLayoutName, fonts, background);
    std::tr1::shared_ptr<CUIMenuItem> pSelected = std::tr1::dynamic_pointer_cast<CUIMenuItem>(selected);
    if (!pSelected) return -1;
    return pSelected->GetKind();
}

namespace {

inline void NotifyMenuCallbacks(const BUI::CCallbackListOwner& rOwner, BUI* const& rSender) {
    if (rOwner.get()) {
        for (BUI::CallbackList::iterator i = rOwner.get()->begin(); i != rOwner.get()->end(); ++i) (**i)(rSender);
    }
}

void OnMenuAccept(BUI* pSender, bool* pAccepted, const MenuCallback* pCallbacks) {
    *pAccepted = true;
    if (pCallbacks) NotifyMenuCallbacks(pCallbacks->mAcceptCallbacks, pSender);
}

void OnMenuSelection(BUI* pSender, const MenuCallback* pCallbacks) {
    if (pCallbacks) NotifyMenuCallbacks(pCallbacks->mSelectionCallbacks, pSender);
}

__declspec(section ".text.menu_notify") inline void NotifyCallbacks(const BUI::CCallbackListOwner& rOwner, BUI* const& rSender) {
    bool requestComparison = false;
    if (requestComparison) {
        typedef std::vector<std::tr1::shared_ptr<IUIListItem> >::iterator ItemIterator;
        (void)(*static_cast<ItemIterator*>(nullptr) != *static_cast<ItemIterator*>(nullptr));
    }

    if (rOwner.get()) {
        for (BUI::CallbackList::iterator i = rOwner.get()->begin(); i != rOwner.get()->end(); ++i) (**i)(rSender);
    }
}

#pragma section code_type ".text"

__declspec(section ".text.menu_prefix") std::tr1::shared_ptr<IUIListItem> RunMenuList(const wchar_t* pTitle, std::vector<std::tr1::shared_ptr<IUIListItem> >& rItems, int columns, int rows, int defaultIndex, const MenuCallback* pCallbacks, IUIInput* pInput, std::tr1::shared_ptr<CUIManager> manager, const char* const* pArchives, const char* pLayoutName, const std::tr1::shared_ptr<CLayout::FontMap>& rFonts, int background) {
    std::tr1::shared_ptr<BUILayout> pRoot(new BUILayout());
    pRoot->Load(pArchives, (std::string(pLayoutName) + "_root").c_str(), rFonts, background);
    std::tr1::shared_ptr<CUIMenu> pMenu(new CUIMenu());
    pMenu->Load(columns, rows, pArchives, pLayoutName, rFonts, background);
    manager->AddChild(pRoot);
    pRoot->AttachChild("MENU", pMenu);
    bool accepted = false;
    pMenu->GetAcceptCallbacks().Add(std::tr1::bind(OnMenuAccept, std::tr1::placeholders::_1, &accepted, pCallbacks));
    pMenu->GetSelectionCallbacks().Add(std::tr1::bind(OnMenuSelection, std::tr1::placeholders::_1, pCallbacks));
    pMenu->GetItems() = rItems;
    pMenu->SetTitle(pTitle);
    if (defaultIndex >= 0) {
        int index = 0;
        for (std::vector<std::tr1::shared_ptr<IUIListItem> >::iterator i = rItems.begin(); i != rItems.end(); ++i) {
            std::tr1::shared_ptr<CUIMenuItem> pItem = std::tr1::dynamic_pointer_cast<CUIMenuItem>(*i);
            if (pItem && defaultIndex == pItem->GetKind()) {
                if (pItem->IsSelectable()) pMenu->SetSelectedIndex(index);
                else defaultIndex = -1;
                break;
            }
            ++index;
        }
    }
    if (defaultIndex < 0) {
        int index = 0;
        for (std::vector<std::tr1::shared_ptr<IUIListItem> >::iterator i = rItems.begin(); i != rItems.end(); ++i) {
            if (*i && (*i)->IsSelectable()) {
                pMenu->SetSelectedIndex(index);
                break;
            }
            ++index;
        }
    }
    pMenu->UpdateSelection();
    BUI* pPreviousFocus = manager->GetFocusedUI();
    manager->SetFocusedUI(pMenu.get());
    pRoot->GetLayout()->CreateMotion("begin")->Wait();
    std::tr1::shared_ptr<IUIListItem> result;
    for (;;) {
        bool noInput = !pInput;
        if (noInput) break;
        if (!pInput->IsConnected()) break;
        if (pInput->GetTrig() & 0x10) {
            if (pCallbacks) NotifyCallbacks(pCallbacks->mScrollCallbacks, pMenu.get());
            break;
        }
        manager->DispatchFocusedInput(pInput);
        if (accepted) {
            pMenu->GetSelectedLayout()->CreateMotion("selected", "HOVER", 0.0f, 1.0f)->Wait();
            result = pMenu->GetSelectedItem();
            break;
        }
        YieldFiber();
    }
    pRoot->GetLayout()->CreateMotion("end")->Wait();
    manager->RemoveChild(pRoot);
    manager->SetFocusedUI(pPreviousFocus);
    return result;
}

}

namespace std {
namespace tr1 {
template <> shared_ptr<IUIListItem>::shared_ptr() : ptr_(nullptr), s_(nullptr) {}
}
}

namespace std {
namespace tr1 {
template <> __declspec(section ".text.menu_prefix") CUIMenuItem* shared_ptr<CUIMenuItem>::operator->() const;
template <> __declspec(section ".text.menu_prefix") IUIListItem* shared_ptr<IUIListItem>::operator->() const;
template <> __declspec(section ".text") detail::shared_ptr_deleter<CUIMenuItem, std::detail::default_delete<CUIMenuItem> >::~shared_ptr_deleter();
}
template <> __declspec(section ".text.menu_prefix") vector<tr1::shared_ptr<IUIListItem> >::vector();
template <> __declspec(section ".text.menu_prefix") void vector<tr1::shared_ptr<IUIListItem> >::reserve(unsigned long);
}
