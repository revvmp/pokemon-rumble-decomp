#ifndef GAME_CUILIST_H
#define GAME_CUILIST_H

#include <game/BUILayout.h>
#include <game/IUIListItem.h>
#include <functional>
#include <vector>

class CUIList : public BUILayout {
public:
    typedef std::tr1::shared_ptr<std::tr1::function<void(BUI*)> > Callback;
    typedef std::list<Callback> CallbackList;
    CUIList();
    virtual ~CUIList();
    virtual void Calc();
    virtual void Draw();
    virtual void OnInput(const IUIInput* pInput);
    virtual void Load(const char*, const char*, const std::tr1::shared_ptr<CLayout::FontMap>&, int);
    virtual void Load(const char* const*, const char*, const std::tr1::shared_ptr<CLayout::FontMap>&, int);
    virtual void UpdateSelection();
    unsigned long GetLayoutCount() const;
    void SetSelectedIndex(int index);
    int GetSelectedIndex();
    std::tr1::shared_ptr<IUIListItem> GetSelectedItem();
#if defined(CUIMENU_EMISSION_LAYOUT)
    __declspec(section ".text.menu_prefix")
#endif
    std::tr1::shared_ptr<IUIListItem> GetItem(int index) { return GetItem(static_cast<unsigned long>(index)); }
#if defined(CUIMENU_EMISSION_LAYOUT)
    __declspec(section ".text.menu_prefix")
#endif
    std::tr1::shared_ptr<IUIListItem> GetItem(unsigned long index) { return index < mItems.size() ? mItems[index] : std::tr1::shared_ptr<IUIListItem>(); }
    std::vector<std::tr1::shared_ptr<IUIListItem> >& GetItems() { return mItems; }
    CLayout* GetSelectedLayout();
    void SetScrollRange(unsigned long begin,unsigned long end);
    void SetSelectionInputEnabled(bool enabled);
    void SetActivateOnHold(bool enabled);
    int GetFirstVisibleIndex();
    void SetScrollMode(int mode);
    void UpdateScrollRange();
    void OnFocus(BUI* pUI);
private:
    typedef BUI::CCallbackListOwner CCallbackListOwner;
protected:
    int mInputDirection;
    int mScrollMode;
    std::vector<std::tr1::shared_ptr<CLayout> > mLayouts;
    std::vector<std::tr1::shared_ptr<IUIListItem> > mItems;
    unsigned long mSelectedIndex;
    unsigned long mFirstVisibleIndex;
    int mScrollRangeBegin;
    int mScrollRangeEnd;
    bool mSelectionInputEnabled;
    bool mRequireFocusForHighlight;
    bool mActivateOnHold;
    bool mWrapSelection;
    std::tr1::weak_ptr<IUIListItem> mLastSelection;
    IUIListItem* mpLastSelection;
    CCallbackListOwner mSelectionCallbacks;
    CCallbackListOwner mAcceptCallbacks;
    CCallbackListOwner mScrollCallbacks;

public:
    CCallbackListOwner& GetSelectionCallbacks() { return mSelectionCallbacks; }
    CCallbackListOwner& GetAcceptCallbacks() { return mAcceptCallbacks; }
    CCallbackListOwner& GetScrollCallbacks() { return mScrollCallbacks; }
};


#endif
