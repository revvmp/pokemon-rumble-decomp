#ifndef GAME_CUIMENUITEM_H
#define GAME_CUIMENUITEM_H

#include <game/CUIList.h>
#include <string>

class CUIMenuItem : public IUIListItem {
public:
    CUIMenuItem(const std::wstring& rTitle, int kind, float width);
    virtual ~CUIMenuItem();
    virtual bool IsSelectable() const;
    virtual void Update(CUIList* pList, CLayout* pLayout);
    float GetWidth() const { return mWidth; }
#if defined(CUIMENU_EMISSION_LAYOUT)
    __declspec(section ".text.menu_prefix")
#endif
    int GetKind() const { return mKind; }

private:
    float mWidth;
    std::wstring mTitle;
    int mKind;
};

#endif
