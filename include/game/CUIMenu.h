#ifndef GAME_CUIMENU_H
#define GAME_CUIMENU_H

#include <game/BUI.h>
#include <game/BUICallbackList.h>
#include <game/CUIList.h>
#include <game/PaneRectangle.h>
#include <game/CLayout.h>
#include <game/IUIInput.h>
#include <MSL_CPP/msl_utility.h>
#include <list>
#include <memory>
#include <vector>
#include <revolution/types.h>

class CUIManager;

struct UIMenuItem {
    operator const wchar_t* const&() const { return mText; }

    const wchar_t* mText;
    int mKind;
};

namespace Metrowerks {
template <> struct is_POD<UIMenuItem> {
    enum { value = true };
};
}

namespace std {
template <> struct __vec_bulk_move<UIMenuItem> {
    enum { value = false };
};
}

struct UIMenuParam {
    const wchar_t* mTitle;
    UIMenuItem* mpItems;
    u32 mItemCount;
    int mColumnCount;
    u32 mRowCount;
    int mDefaultIndex;
    const char* const* mpArchives;
    const char* mpLayoutName;
};

class MenuCallback {
public:
    BUI::CCallbackListOwner mSelectionCallbacks;
    BUI::CCallbackListOwner mAcceptCallbacks;
    BUI::CCallbackListOwner mScrollCallbacks;
};

class CUIMenuSoundCallback : public MenuCallback {
public:
    explicit CUIMenuSoundCallback(int mode);

    void OnAccept(BUI* pSender);
    void OnSelectionChange(BUI* pSender);
    void OnScroll(BUI* pSender);

private:
    enum { MODE_NO_SCROLL_SOUND = 1 };

    int mMode;
    bool mMuteSelection;
};

int RunUIMenu(const UIMenuParam& rParam, CUIMenuSoundCallback& rCallbacks, IUIInput* pInput, std::tr1::shared_ptr<CUIManager> manager, std::tr1::shared_ptr<CLayout::FontMap> fonts, bool flag);

class CUIMenu : public CUIList {
public:
    CUIMenu();
    virtual ~CUIMenu();
    virtual void OnInput(const IUIInput* pInput);
    virtual void UpdateSelection();
    virtual void Load(int columns, int rows, const char* pArchive, const char* pName, const std::tr1::shared_ptr<CLayout::FontMap>& rFonts, bool background);
    virtual void Load(int columns, int rows, const char* const* pArchives, const char* pName, const std::tr1::shared_ptr<CLayout::FontMap>& rFonts, bool background);
    void SetTitle(const wchar_t* pTitle);

private:
    bool mLayoutReady;
    NErrMsg::PaneRectangle mBounds;
    float mCellHeight;
    nw4r::math::VEC2 mItemCenter;
    nw4r::math::VEC2 mTitlePosition;
    float mMinimumWidth;
    int mColumnCount;
    int mRowCount;
    float mColumnSpacing;
    float mRowSpacing;
    int mCursorRow;
};

#endif
