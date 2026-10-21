#include <game/CUIList.h>
#include <game/MathUtil.h>

IUIListItem::~IUIListItem() {}
bool IUIListItem::IsSelectable() const { return true; }
void IUIListItem::Update(CUIList*, CLayout*) {}
CUIList::CUIList() : mInputDirection(0), mScrollMode(0), mSelectedIndex(0), mFirstVisibleIndex(0), mScrollRangeBegin(0), mScrollRangeEnd(0), mSelectionInputEnabled(false), mRequireFocusForHighlight(false), mActivateOnHold(false), mWrapSelection(true), mpLastSelection(nullptr) {}
CUIList::~CUIList() {}
unsigned long CUIList::GetLayoutCount() const { return mLayouts.size(); }
void CUIList::SetSelectedIndex(int index) { mSelectedIndex = index; UpdateSelection(); }
int CUIList::GetSelectedIndex() {
    if (mItems.size() <= mSelectedIndex) {
        if (mItems.empty()) mSelectedIndex = 0;
        else mSelectedIndex = mItems.size() - 1;
    }
    return mSelectedIndex;
}

void CUIList::Load(const char* pArchive, const char* pName, const std::tr1::shared_ptr<CLayout::FontMap>& rFonts, bool background) {
    const char* archives[] = {pArchive, nullptr};
    Load(archives, pName, rFonts, background);
}

std::tr1::shared_ptr<IUIListItem> CUIList::GetSelectedItem() {
    GetSelectedIndex();
    if (!mItems.empty()) return mItems[mSelectedIndex];
    return std::tr1::shared_ptr<IUIListItem>();
}
CLayout* CUIList::GetSelectedLayout() {
    UpdateScrollRange();
    int index = mSelectedIndex - mFirstVisibleIndex;
    if (index >= 0 && index < static_cast<int>(mLayouts.size())) return mLayouts[index].get();
    return nullptr;
}
void CUIList::SetScrollRange(unsigned long begin,unsigned long end) {
    mScrollRangeBegin = Clamp(begin,0UL,mLayouts.size()-1);
    mScrollRangeEnd = Clamp(end,static_cast<unsigned long>(mScrollRangeBegin+1),mLayouts.size());
    UpdateSelection();
}
void CUIList::SetSelectionInputEnabled(bool enabled) { mSelectionInputEnabled = enabled; }
void CUIList::SetActivateOnHold(bool enabled) { mActivateOnHold = enabled; }
int CUIList::GetFirstVisibleIndex() { UpdateScrollRange(); return mFirstVisibleIndex; }
void CUIList::SetScrollMode(int mode) { mScrollMode = mode; UpdateSelection(); }
void CUIList::UpdateScrollRange() {
    GetSelectedIndex();
    unsigned long layouts = mLayouts.size();
    unsigned long items = mItems.size();
    if (items <= layouts) mFirstVisibleIndex = 0;
    else if (mScrollMode != 1) {
        int first = mFirstVisibleIndex;
        int begin = mScrollRangeBegin;
        int selected = mSelectedIndex;
        int end = mScrollRangeEnd;
        if (selected < first+begin) first = selected-begin;
        else if (first+end <= selected) first = selected-end+1;
        mFirstVisibleIndex = Clamp(first,0,static_cast<int>(items-layouts));
    } else mFirstVisibleIndex = mSelectedIndex / layouts * layouts;
}
void CUIList::Calc() {
    BUILayout::Calc();
    for (std::vector<std::tr1::shared_ptr<CLayout> >::iterator i=mLayouts.begin();i!=mLayouts.end();++i) (*i)->Calc();
}
void CUIList::Draw() {
    BUILayout::Draw();
    for (std::vector<std::tr1::shared_ptr<CLayout> >::iterator i=mLayouts.begin();i!=mLayouts.end();++i) (*i)->Draw();
}

void CUIList::OnFocus(BUI*) {
    if (IsFocused()) {
        GetLayout()->CreateMotion("normal_hover");
        GetLayout()->QueueMotion("hover");
    } else {
        GetLayout()->CreateMotion("hover_normal");
        GetLayout()->QueueMotion("normal");
    }
    UpdateSelection();
}
