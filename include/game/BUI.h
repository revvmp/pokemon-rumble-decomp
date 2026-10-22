#ifndef GAME_BUI_H
#define GAME_BUI_H

#include <functional>
#include <list>
#include <memory>
#include <game/ICalcDraw.h>
#include <game/IUIInput.h>

class CUIManager;
class MenuCallback;

class BUI : public ICalcDraw, public std::tr1::enable_shared_from_this<BUI> {
public:
    typedef std::tr1::shared_ptr<BUI> Child;
    typedef std::tr1::shared_ptr<std::tr1::function<void(BUI*)> > Callback;
    typedef std::list<Callback> CallbackList;

    BUI();
    virtual ~BUI();
    virtual void Calc();
    virtual void CalcMtx();
    virtual void CalcView();
    virtual void Draw();
    virtual CUIManager* GetManager();
    virtual void OnAttached();
    virtual void OnDetached(BUI* pParent);
    virtual void OnInput(const IUIInput* pInput);

    bool AddChild(Child child);
    bool IsFocused();
    bool IsFocusable() const { return (mFlags & 4) != 0; }
    BUI* GetNeighbor(int direction) { return mNeighbors[direction].lock().get(); }
    BUI* NavigateTo(int direction);
    BUI* FindFocusableFrom(int direction);
    bool RemoveChild(Child child);
    void SetVisible(bool visible);

    class CCallbackListOwner {
    public:
        explicit CCallbackListOwner(CallbackList* pList = nullptr) : mpList(pList) {}
        ~CCallbackListOwner() { delete mpList; }
        CallbackList* get() const { return mpList; }
        Callback Add(const std::tr1::function<void(BUI*)>& rCallback);

    private:
        CCallbackListOwner(const CCallbackListOwner&);
        CCallbackListOwner& operator=(const CCallbackListOwner&);

        CallbackList* mpList;
    };

private:
    friend class MenuCallback;
    friend class CUIManager;
    friend class BUILayout;

    u32 mFlags;
    BUI* mpParent;
    std::list<Child> mChildren;
    std::tr1::weak_ptr<BUI> mNeighbors[4];
    CCallbackListOwner mFocusCallbacks;
    CCallbackListOwner mVisibilityCallbacks;
    CCallbackListOwner mEventCallbacks;

public:
    CCallbackListOwner& GetFocusCallbacks() { return mFocusCallbacks; }
};

#endif
