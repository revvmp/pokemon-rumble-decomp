#include <game/CUIList.h>
#include <game/StringFormat.h>
#include <game/MathUtil.h>
#include <game/CUIManager.h>
#include <nw4r/lyt/lyt_pane.h>
#include <algorithm>

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

void CUIList::Load(const char* const* pArchives, const char* pName, const std::tr1::shared_ptr<CLayout::FontMap>& rFonts, bool background) {
    BUILayout::Load(pArchives,pName,rFonts,background);
    std::string itemName(pName);
    itemName += "_item";
    std::vector<std::tr1::shared_ptr<CLayout> > layouts;
    for (;;) {
        nw4r::lyt::Pane* pPane = GetLayout()->FindPane(FormatString("ITEM%d", layouts.size()).c_str());
        if (pPane == nullptr) break;
        std::tr1::shared_ptr<CLayout> layout(new CLayout());
        layout->Load(pArchives,itemName.c_str(),rFonts,background);
        layouts.push_back(layout);
        GetLayout()->Attach(pPane,layout.get());
    }
    if (layouts.empty()) {
        CLayout::CInitializeException error(3,pArchives,pName,rFonts);
        CLayout::HandleInitializeError(error);
        return;
    }
    {
        mLayouts.swap(layouts);
        mScrollRangeEnd = mLayouts.size();
        GetFocusCallbacks().Add(std::tr1::bind(&CUIList::OnFocus,this,std::tr1::placeholders::_1));
        Calc();
        UpdateSelection();
    }
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
    if (mItems.size() <= mLayouts.size()) mFirstVisibleIndex = 0;
    else if (mScrollMode != 1) {
        int selected = mSelectedIndex;
        int first = mFirstVisibleIndex;
        int begin = mScrollRangeBegin;
        int end = mScrollRangeEnd;
        if (selected < first+begin) first = selected-begin;
        else if (first+end <= selected) first = selected-end+1;
        int last = mItems.size()-mLayouts.size();
        mFirstVisibleIndex = Clamp(first,0,last);
    } else mFirstVisibleIndex = mSelectedIndex / mLayouts.size() * mLayouts.size();
}
void CUIList::UpdateSelection() {
    UpdateScrollRange();
    GetLayout()->SetPaneVisible("BACKWARD", mFirstVisibleIndex != 0);
    GetLayout()->SetPaneVisible("FORWARD", mFirstVisibleIndex + mLayouts.size() < mItems.size());
    bool scroll = static_cast<int>(mItems.size()) > static_cast<int>(mLayouts.size());
    GetLayout()->SetPaneVisible("VSCROLL", scroll);
    GetLayout()->SetPaneVisible("HSCROLL", scroll);
    float position = mItems.empty() ? 0.0f : static_cast<float>(static_cast<unsigned long>(mFirstVisibleIndex)) / mItems.size();
    float size = mItems.size() <= mLayouts.size() ? 1.0f : static_cast<float>(mLayouts.size()) / mItems.size();
    if (1.0f-position < size) size = 1.0f-position;
    nw4r::lyt::Pane* pThumb = GetLayout()->FindPane("VSCROLL_THUMB");
    if (pThumb) {
        nw4r::lyt::Pane* pTrack = GetLayout()->FindPane("VSCROLL_BK");
        if (pTrack) {
            nw4r::lyt::Size trackSize = pTrack->GetSize();
            pThumb->SetSize(nw4r::lyt::Size(trackSize.width, trackSize.height*size));
            nw4r::math::VEC3 translation(0.0f, -trackSize.height*position, 0.0f);
            pThumb->SetTranslate(translation);
        }
    }
    pThumb = GetLayout()->FindPane("HSCROLL_THUMB");
    if (pThumb) {
        nw4r::lyt::Pane* pTrack = GetLayout()->FindPane("HSCROLL_BK");
        if (pTrack) {
            nw4r::lyt::Size trackSize = pTrack->GetSize();
            pThumb->SetSize(nw4r::lyt::Size(trackSize.width*size, trackSize.height));
            nw4r::math::VEC3 translation(trackSize.width*position, 0.0f, 0.0f);
            pThumb->SetTranslate(translation);
        }
    }
    GetLayout()->SetPaneVisible("PAGE_NUM",mScrollMode == 1);
    GetLayout()->SetText("PAGE_NUM_CUR",FormatString(L"%d",mFirstVisibleIndex/mLayouts.size()+1).c_str());
    GetLayout()->SetText("PAGE_NUM_MAX",FormatString(L"%d",(mItems.size()+mLayouts.size()-1)/mLayouts.size()).c_str());
    for (unsigned long i=0;i<mLayouts.size();++i) {
        unsigned long index=mFirstVisibleIndex+i;
        bool visible=index<mItems.size();
        mLayouts[i]->SetVisible(visible);
        if (visible) {
            bool selected=index==mSelectedIndex;
            if (selected && mRequireFocusForHighlight && !IsFocused()) selected=false;
            mLayouts[i]->SetPaneVisible("HOVER",selected);
            if (index==mSelectedIndex) mLayouts[i]->CreateMotion("hover","HOVER",0.0f,1.0f);
            mItems[index]->Update(this,mLayouts[i].get());
        }
    }
    std::tr1::shared_ptr<IUIListItem> previous=mLastSelection.lock();
    std::tr1::shared_ptr<IUIListItem> current=GetSelectedItem();
    bool changed=previous.get()!=mpLastSelection || previous.get()!=current.get();
    mLastSelection=current;
    mpLastSelection=current.get();
    if (changed && mSelectionCallbacks.get()) {
        for (CallbackList::iterator i=mSelectionCallbacks.get()->begin();i!=mSelectionCallbacks.get()->end();++i) (**i)(this);
    }
}

void CUIList::Calc() {
    BUILayout::Calc();
    for (std::vector<std::tr1::shared_ptr<CLayout> >::iterator i=mLayouts.begin();i!=mLayouts.end();++i) (*i)->Calc();
}
void CUIList::Draw() {
    BUILayout::Draw();
    for (std::vector<std::tr1::shared_ptr<CLayout> >::iterator i=mLayouts.begin();i!=mLayouts.end();++i) (*i)->Draw();
}

namespace {
const int sFocusDirections[2][4] = {{0,2,3,1},{3,1,0,2}};
const u32 sDirectionButtons[2][4] = {{1,4,8,2},{8,2,1,4}};
}
void CUIList::OnInput(const IUIInput* pInput) {
    const int* pDirections = sFocusDirections[mInputDirection];
    const u32* pButtons = sDirectionButtons[mInputDirection];
    int previousIndex = mSelectedIndex;
    int previousFirst = mFirstVisibleIndex;
    bool activated = false;
    u32 trigger = pInput->GetTrig();
    u32 buttons = trigger | pInput->GetRepeat();
    unsigned long pageSize = mLayouts.size();
    if (pButtons[2] & buttons) {
        GetSelectedIndex();
        if (pageSize <= static_cast<unsigned long>(mSelectedIndex)) mSelectedIndex -= pageSize;
        else if ((trigger & pButtons[2]) && mWrapSelection && !mItems.empty()) {
            mSelectedIndex += mItems.size()/pageSize*pageSize;
            mSelectedIndex = std::min(mSelectedIndex,mItems.size()-1);
        } else mSelectedIndex=0;
    } else if (pButtons[3] & buttons) {
        GetSelectedIndex();
        if (mSelectedIndex+pageSize < mItems.size()) mSelectedIndex += pageSize;
        else if (!mItems.empty()) {
            unsigned long page=mSelectedIndex/pageSize;
            if (page < (mItems.size()+pageSize-1)/pageSize-1) mSelectedIndex=mItems.size()-1;
            else if ((trigger&pButtons[3]) && mWrapSelection) mSelectedIndex-=page*pageSize;
            else mSelectedIndex=mItems.size()-1;
        } else mSelectedIndex=0;
    } else if (trigger & 0x20) {
        GetSelectedIndex();
        if (!mItems.empty()) {
            if (mAcceptCallbacks.get()) for (CallbackList::iterator i=mAcceptCallbacks.get()->begin();i!=mAcceptCallbacks.get()->end();++i) (**i)(this);
            activated=true;
        }
    } else {
        int direction=4;
        if (buttons&pButtons[0]) {
            direction=pDirections[0];
            GetSelectedIndex();
            if (!mItems.empty()) while (true) {
                if (mSelectedIndex==0) {
                    if (!mWrapSelection || !(trigger&pButtons[0])) { mSelectedIndex=previousIndex; break; }
                    mSelectedIndex=mItems.size();
                }
                --mSelectedIndex;
                if (mSelectedIndex==previousIndex || mItems[mSelectedIndex]->IsSelectable()) break;
            }
        } else if (buttons&pButtons[1]) {
            direction=pDirections[1];
            GetSelectedIndex();
            if (!mItems.empty()) while (true) {
                if (static_cast<unsigned long>(mSelectedIndex)==mItems.size()-1) {
                    if (!mWrapSelection || !(trigger&pButtons[1])) { mSelectedIndex=previousIndex; break; }
                    mSelectedIndex=-1;
                }
                ++mSelectedIndex;
                if (mSelectedIndex==previousIndex || mItems[mSelectedIndex]->IsSelectable()) break;
            }
        }
        if (mSelectedIndex!=previousIndex && mScrollMode==1) {
            UpdateScrollRange();
            if (mFirstVisibleIndex!=previousFirst) {
                BUI* pNeighbor=GetNeighbor(direction);
                if (pNeighbor) {
                    pNeighbor=NavigateTo(direction);
                    if (pNeighbor) {
                        mSelectedIndex=previousIndex;
                        mFirstVisibleIndex=previousFirst;
                        GetManager()->SetFocusedUI(pNeighbor);
                        return;
                    }
                }
                if (!mSelectionInputEnabled) {
                    mSelectedIndex=previousIndex;
                    mFirstVisibleIndex=previousFirst;
                    return;
                }
            }
        }
    }
    if (mSelectedIndex!=previousIndex) {
        if (mActivateOnHold && !activated && (pInput->GetHold()&0x20) && !mItems.empty() && mAcceptCallbacks.get()) {
            for (CallbackList::iterator i=mAcceptCallbacks.get()->begin();i!=mAcceptCallbacks.get()->end();++i) (**i)(this);
        }
        UpdateSelection();
        if (previousFirst!=mFirstVisibleIndex && mScrollCallbacks.get()) {
            for (CallbackList::iterator i=mScrollCallbacks.get()->begin();i!=mScrollCallbacks.get()->end();++i) (**i)(this);
        }
    } else BUI::OnInput(pInput);
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

