#ifndef GAME_IUILISTITEM_H
#define GAME_IUILISTITEM_H

class CUIList;
class CLayout;

class IUIListItem {
public:
    virtual ~IUIListItem() = 0;
    virtual bool IsSelectable() const;
    virtual void Update(CUIList* pList, CLayout* pLayout);
};

#endif
