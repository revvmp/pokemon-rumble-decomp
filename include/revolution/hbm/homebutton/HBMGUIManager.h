#ifndef HBM_GUI_MANAGER_H
#define HBM_GUI_MANAGER_H

#include "nw4hbm/lyt/lyt_pane.h"
#include "nw4hbm/ut/ut_list.h"
#include "revolution/mem.h"
#include "revolution/mtx.h"
#include "revolution/kpad.h"

namespace nw4hbm {
namespace lyt {
class DrawInfo;
class Layout;
}
}

namespace homebutton {
namespace gui {

class Manager;
class PaneComponent;

enum Event {
    EVENT_TRIG = 0,
    EVENT_ON_POINT = 1,
    EVENT_OFF_POINT = 2,
    EVENT_MOVE = 3,
    EVENT_RELEASE = 5
};

class Interface {
public:
    Interface() {}

    virtual void create() {}
    virtual void init() {}
    virtual void calc() {}
    virtual void draw(Mtx&) {}
    virtual void draw() {}
    virtual ~Interface() {}
};

class EventHandler {
public:
    EventHandler() {}

    virtual void onEvent(u32, u32, void*) {}
    virtual void setManager(Manager* pManager) { mpManager = pManager; }

protected:
    Manager* mpManager;
};

class Component : public Interface {
public:
    Component(u32 id) : mbDragging(), muDraggingButton(), muID(id), mbTriggerTarger(), mpManager() {
        init();
    }

    virtual void init() {
        mbDragging = false;

        for (int i = 0; i < (int)(sizeof(mabPointed) / sizeof(mabPointed[0])); i++) {
            mabPointed[i] = false;
        }
    }

    virtual ~Component() {}
    virtual u32 getID() { return muID; }
    virtual int isPointed(int n) { return mabPointed[n]; }
    virtual void setPointed(int n, bool pointed) { mabPointed[n] = pointed; }
    virtual void onPoint() {}
    virtual void offPoint() {}
    virtual void onDrag(f32, f32) {}
    virtual void onMove(f32, f32) {}

    virtual void onTrig(u32 flag, Vec& rPos) {
        if (flag & muDraggingButton) {
            mDragStartPos = rPos;
            mbDragging = true;
        }
    }

    virtual void setDraggingButton(u32 button) { muDraggingButton = button; }
    virtual bool update(int, const KPADStatus*, f32, f32, void*) { return false; }
    virtual bool update(int i, f32 x, f32 y, u32 trigFlag, u32 holdFlag, u32 releaseFlag, void* pData);
    virtual bool isTriggerTarger() { return mbTriggerTarger; }
    virtual void setTriggerTarget(bool target) { mbTriggerTarger = target; }
    virtual void setManager(Manager* pManager) { mpManager = pManager; }
    virtual bool isVisible() { return true; }
    virtual bool contain(f32 x, f32 y) = 0;

protected:
    bool mabPointed[8];
    Vec mDragStartPos;
    bool mbDragging;
    u32 muDraggingButton;
    u32 muID;
    bool mbTriggerTarger;
    Manager* mpManager;
};

class Manager : public Interface {
protected:
    struct IDToComponent {
        IDToComponent(u32 id, Component* pComponent) : muID(id), mpComponent(pComponent) {}

        u32 muID;
        Component* mpComponent;
        nw4hbm::ut::Link mLink;
    };

public:
    Manager(EventHandler* pEventHandler, MEMAllocator* pAllocator)
        : mpEventHandler(pEventHandler), mpAllocator(pAllocator) {
        if (mpEventHandler != nullptr) {
            mpEventHandler->setManager(this);
        }

        nw4hbm::ut::List_Init(&mIDToComponent, 8);
    }

    virtual void init();
    virtual void calc();
    virtual void draw();
    virtual ~Manager();
    virtual void addComponent(Component* pComponent);
    virtual void delComponent(Component* pComponent);
    virtual Component* getComponent(u32 id);
    virtual bool update(int, const KPADStatus*, f32, f32, void*) { return false; }
    virtual bool update(int i, f32 x, f32 y, u32 trigFlag, u32 holdFlag, u32 releaseFlag, void* pData);

    virtual void onEvent(u32 id, u32 event, void* pData) {
        if (mpEventHandler != nullptr) {
            mpEventHandler->onEvent(id, event, pData);
        }
    }

    virtual void setAllComponentTriggerTarget(bool target);

    virtual void setEventHandler(EventHandler* pEventHandler) {
        mpEventHandler = pEventHandler;

        if (mpEventHandler != nullptr) {
            mpEventHandler->setManager(this);
        }
    }

protected:
    EventHandler* mpEventHandler;
    nw4hbm::ut::List mIDToComponent;
    MEMAllocator* mpAllocator;
};

class PaneManager : public Manager {
protected:
    struct PaneToComponent {
        PaneToComponent(nw4hbm::lyt::Pane* pPane, PaneComponent* pComponent)
            : mpPane(pPane), mpComponent(pComponent) {}

        nw4hbm::lyt::Pane* mpPane;
        PaneComponent* mpComponent;
        nw4hbm::ut::Link mLink;
    };

public:
    PaneManager(EventHandler* pEventHandler, const nw4hbm::lyt::DrawInfo* pDrawInfo, MEMAllocator* pAllocator)
        : Manager(pEventHandler, pAllocator), mpDrawInfo(pDrawInfo) {
        nw4hbm::ut::List_Init(&mPaneToComponent, 8);
    }

    virtual ~PaneManager();
    virtual void createLayoutScene(const nw4hbm::lyt::Layout& rLayout);
    virtual void addLayoutScene(const nw4hbm::lyt::Layout& rLayout);
    virtual void delLayoutScene(const nw4hbm::lyt::Layout& rLayout);
    virtual PaneComponent* getPaneComponentByPane(nw4hbm::lyt::Pane* pPane);
    virtual const nw4hbm::lyt::DrawInfo* getDrawInfo() { return mpDrawInfo; }
    virtual void setDrawInfo(const nw4hbm::lyt::DrawInfo* pDrawInfo) { mpDrawInfo = pDrawInfo; }
    virtual void setAllBoundingBoxComponentTriggerTarget(bool target);
    virtual void walkInChildren(nw4hbm::lyt::PaneList& rPaneList);
    virtual void walkInChildrenDel(nw4hbm::lyt::PaneList& rPaneList);

protected:
    nw4hbm::ut::List mPaneToComponent;
    const nw4hbm::lyt::DrawInfo* mpDrawInfo;
    u16 muNumPoint;
    u16 muPadding;

    static u32 suIDCounter;
};

class PaneComponent : public Component {
public:
    PaneComponent(u32 id) : Component(id), mpPane(nullptr) {}

    virtual void draw();
    virtual ~PaneComponent() {}
    virtual bool isVisible();
    virtual bool contain(f32 x, f32 y);
    virtual void setPane(nw4hbm::lyt::Pane* pPane) { mpPane = pPane; }
    virtual nw4hbm::lyt::Pane* getPane() { return mpPane; }

protected:
    nw4hbm::lyt::Pane* mpPane;
};

}
}

#endif
