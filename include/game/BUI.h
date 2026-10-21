#ifndef GAME_BUI_H
#define GAME_BUI_H

#include <game/ICalcDraw.h>
#include <game/IUIInput.h>
#include <functional>
#include <list>
#include <memory>

class CUIManager;

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

    bool AddChild(const Child& rChild);
    bool RemoveChild(const Child& rChild);

private:
    class CCallbackListOwner {
    public:
        explicit CCallbackListOwner(CallbackList* pList = nullptr) : mpList(pList) {}
        ~CCallbackListOwner() { delete mpList; }
        CallbackList* get() const { return mpList; }

    private:
        CCallbackListOwner(const CCallbackListOwner&);
        CCallbackListOwner& operator=(const CCallbackListOwner&);

        CallbackList* mpList;
    };

    u32 mFlags;
    BUI* mpParent;
    std::list<Child> mChildren;
    std::tr1::weak_ptr<BUI> mNeighbors[4];
    CCallbackListOwner mFocusCallbacks;
    CCallbackListOwner mVisibilityCallbacks;
    CCallbackListOwner mEventCallbacks;
};

#endif
