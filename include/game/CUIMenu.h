#ifndef GAME_CUIMENU_H
#define GAME_CUIMENU_H

#include <game/BUI.h>
#include <game/CLayout.h>
#include <game/IUIInput.h>
#include <MSL_CPP/msl_utility.h>
#include <list>
#include <memory>
#include <revolution/types.h>

class CUIManager;

struct UIMenuItem {
    const wchar_t* mText;
    int mKind;
};

namespace Metrowerks {
template <> struct is_POD<UIMenuItem> {
    enum { value = true };
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

class CUICallbackLists {
private:
    BUI::CCallbackListOwner mFocusCallbacks;
    BUI::CCallbackListOwner mVisibilityCallbacks;
    BUI::CCallbackListOwner mEventCallbacks;
};

class CUIMenuCallbacks : public CUICallbackLists {
public:
    explicit CUIMenuCallbacks(int param);

private:
    int mParam;
    bool mEnabled;
};

int RunUIMenu(const UIMenuParam& rParam, CUIMenuCallbacks& rCallbacks, IUIInput* pInput, std::tr1::shared_ptr<CUIManager> manager, std::tr1::shared_ptr<CLayout::FontMap> fonts, bool flag);

#endif
