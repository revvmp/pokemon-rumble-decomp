#include "revolution/hbm/homebutton/HBMGUIManager.h"
#include "nw4hbm/lyt/lyt_bounding.h"
#include "nw4hbm/lyt/lyt_layout.h"
#include "nw4hbm/lyt/lyt_picture.h"
#include "nw4hbm/lyt/lyt_window.h"
#include "revolution/gx.h"
#include "revolution/gx/GXTransform.h"
#include "revolution/gx/GXVert.h"

inline void* operator new(size_t, void* p) { return p; }

namespace homebutton {
namespace gui {

u32 PaneManager::suIDCounter;

static void drawLine_(f32 x0, f32 y0, f32 x1, f32 y1, f32 z, u8 width, GXColor& rColor);
static bool is_visible(nw4hbm::lyt::Pane* pPane);

static void drawLine_(f32 x0, f32 y0, f32 x1, f32 y1, f32 z, u8 width, GXColor& rColor) {
    static const f32 cubeScale = 1.0f;

    GXClearVtxDesc();
    GXSetVtxDesc(GX_VA_POS, GX_DIRECT);
    GXSetVtxDesc(GX_VA_CLR0, GX_DIRECT);
    GXSetVtxAttrFmt(GX_VTXFMT0, GX_VA_POS, GX_CLR_RGBA, GX_F32, 0);
    GXSetVtxAttrFmt(GX_VTXFMT0, GX_VA_CLR0, GX_CLR_RGBA, GX_RGBA8, 0);

    GXSetCullMode(GX_CULL_NONE);

    GXSetNumChans(1);
    GXSetChanCtrl(GX_COLOR0A0, GX_FALSE, GX_SRC_VTX, GX_SRC_VTX, GX_LIGHT_NULL, GX_DF_NONE, GX_AF_NONE);

    GXSetNumTexGens(0);
    GXSetNumTevStages(1);
    GXSetTevOp(GX_TEVSTAGE0, GX_PASSCLR);
    GXSetTevOrder(GX_TEVSTAGE0, GX_TEXCOORD_NULL, GX_TEXMAP_NULL, GX_COLOR0A0);
    GXSetBlendMode(GX_BM_NONE, GX_BL_ZERO, GX_BL_ZERO, GX_LO_NOOP);

    Mtx modelMtx;
    PSMTXTrans(modelMtx, 0.0f, 0.0f, 0.0f);
    GXLoadPosMtxImm(modelMtx, GX_PNMTX0);

    GXSetLineWidth(width, GX_TO_ZERO);

    GXBegin(GX_LINES, GX_VTXFMT0, 2);
    GXPosition3f32(x0, y0, z);
    GXColor1u32(*reinterpret_cast<u32*>(&rColor));
    GXPosition3f32(x1, y1, z);
    GXColor1u32(*reinterpret_cast<u32*>(&rColor));
    GXEnd();
}

bool Component::update(int i, f32 x, f32 y, u32, u32, u32, void* pData) {
    bool touched = false;

    if (isVisible()) {
        if (contain(x, y)) {
            if (isPointed(i)) {
                onMove(x, y);
                mpManager->onEvent(getID(), EVENT_MOVE, pData);
            } else {
                setPointed(i, true);
                onPoint();
                mpManager->onEvent(getID(), EVENT_ON_POINT, pData);
            }

            touched = true;
        } else if (isPointed(i)) {
            setPointed(i, false);
            offPoint();
            mpManager->onEvent(getID(), EVENT_OFF_POINT, pData);
        }
    }

    return touched;
}

Manager::~Manager() {
    void* p = nw4hbm::ut::List_GetFirst(&mIDToComponent);

    for (; p != nullptr; p = nw4hbm::ut::List_GetFirst(&mIDToComponent)) {
        nw4hbm::ut::List_Remove(&mIDToComponent, p);

        if (mpAllocator != nullptr) {
            MEMFreeToAllocator(mpAllocator, p);
        } else {
            delete static_cast<IDToComponent*>(p);
        }
    }
}

void Manager::init() {
    for (u32 i = 0; i < nw4hbm::ut::List_GetSize(&mIDToComponent); i++) {
        const IDToComponent* p = static_cast<const IDToComponent*>(nw4hbm::ut::List_GetNth(&mIDToComponent, i));
        p->mpComponent->init();
    }
}

void Manager::addComponent(Component* pComponent) {
    u32 id = pComponent->getID();
    pComponent->setManager(this);

    if (mpAllocator != nullptr) {
        void* p = MEMAllocFromAllocator(mpAllocator, sizeof(IDToComponent));
        nw4hbm::ut::List_Append(&mIDToComponent, new (p) IDToComponent(id, pComponent));
    } else {
        nw4hbm::ut::List_Append(&mIDToComponent, new IDToComponent(id, pComponent));
    }
}

void Manager::delComponent(Component* pComponent) {
    IDToComponent* p = static_cast<IDToComponent*>(nw4hbm::ut::List_GetNext(&mIDToComponent, nullptr));

    while (p != nullptr) {
        if (p->mpComponent == pComponent) {
            break;
        }

        p = static_cast<IDToComponent*>(nw4hbm::ut::List_GetNext(&mIDToComponent, p));
    }

    nw4hbm::ut::List_Remove(&mIDToComponent, p);

    if (mpAllocator != nullptr) {
        MEMFreeToAllocator(mpAllocator, p);
    } else {
        delete p;
    }
}

Component* Manager::getComponent(u32 id) {
    const IDToComponent* p = static_cast<const IDToComponent*>(nw4hbm::ut::List_GetNth(&mIDToComponent, id));
    return p->mpComponent;
}

bool Manager::update(int i, f32 x, f32 y, u32 trigFlag, u32 holdFlag, u32 releaseFlag, void* pData) {
    bool touched = false;
    Component* pLastContained = nullptr;

    for (u32 n = 0; n < nw4hbm::ut::List_GetSize(&mIDToComponent); n++) {
        const IDToComponent* p = static_cast<const IDToComponent*>(nw4hbm::ut::List_GetNth(&mIDToComponent, n));

        if (p->mpComponent->update(i, x, y, trigFlag, holdFlag, releaseFlag, pData)) {
            if (p->mpComponent->isTriggerTarger()) {
                pLastContained = p->mpComponent;
            }

            touched = true;
        }
    }

    if (pLastContained != nullptr) {
        if (trigFlag) {
            Vec pos;
            pLastContained->onTrig(trigFlag, pos);
            onEvent(pLastContained->getID(), EVENT_TRIG, pData);
        }

        if (releaseFlag) {
            Vec pos;
            pLastContained->onTrig(releaseFlag, pos);
            onEvent(pLastContained->getID(), EVENT_RELEASE, pData);
        }
    }

    return touched;
}

void Manager::calc() {
    for (u32 i = 0; i < nw4hbm::ut::List_GetSize(&mIDToComponent); i++) {
        const IDToComponent* p = static_cast<const IDToComponent*>(nw4hbm::ut::List_GetNth(&mIDToComponent, i));
        p->mpComponent->calc();
    }
}

void Manager::draw() {
    for (u32 i = 0; i < nw4hbm::ut::List_GetSize(&mIDToComponent); i++) {
        const IDToComponent* p = static_cast<const IDToComponent*>(nw4hbm::ut::List_GetNth(&mIDToComponent, i));
        p->mpComponent->draw();
    }
}

void Manager::setAllComponentTriggerTarget(bool target) {
    for (u32 i = 0; i < nw4hbm::ut::List_GetSize(&mIDToComponent); i++) {
        const IDToComponent* p = static_cast<const IDToComponent*>(nw4hbm::ut::List_GetNth(&mIDToComponent, i));
        p->mpComponent->setTriggerTarget(target);
    }
}

PaneManager::~PaneManager() {
    PaneToComponent* p = static_cast<PaneToComponent*>(nw4hbm::ut::List_GetFirst(&mPaneToComponent));

    for (; p != nullptr; p = static_cast<PaneToComponent*>(nw4hbm::ut::List_GetFirst(&mPaneToComponent))) {
        nw4hbm::ut::List_Remove(&mPaneToComponent, p);

        if (mpAllocator != nullptr) {
            MEMFreeToAllocator(mpAllocator, p->mpComponent);
            MEMFreeToAllocator(mpAllocator, p);
        } else {
            delete p->mpComponent;
            delete p;
        }
    }
}

void PaneManager::createLayoutScene(const nw4hbm::lyt::Layout& rLayout) {
    suIDCounter = 0;
    walkInChildren(rLayout.GetRootPane()->GetChildList());
}

void PaneManager::addLayoutScene(const nw4hbm::lyt::Layout& rLayout) {
    walkInChildren(rLayout.GetRootPane()->GetChildList());
}

void PaneManager::walkInChildren(nw4hbm::lyt::PaneList& rPaneList) {
    for (nw4hbm::lyt::PaneList::Iterator it = rPaneList.GetBeginIter(); it != rPaneList.GetEndIter(); it++) {
        PaneComponent* pComponent = nullptr;
        PaneToComponent* pLink = nullptr;

        if (mpAllocator != nullptr) {
            void* pComponentBuf = MEMAllocFromAllocator(mpAllocator, sizeof(PaneComponent));
            void* pLinkBuf = MEMAllocFromAllocator(mpAllocator, sizeof(PaneToComponent));

            pComponent = new (pComponentBuf) PaneComponent(suIDCounter);
            pLink = new (pLinkBuf) PaneToComponent(&*it, pComponent);
        } else {
            pComponent = new PaneComponent(suIDCounter);
            pLink = new PaneToComponent(&*it, pComponent);
        }

        nw4hbm::ut::List_Append(&mPaneToComponent, pLink);
        suIDCounter++;

        pComponent->setPane(&*it);

        if (nw4hbm::ut::DynamicCast<nw4hbm::lyt::Picture*>(&*it) != nullptr) {
            pComponent->setTriggerTarget(true);
        }

        if (nw4hbm::ut::DynamicCast<nw4hbm::lyt::Window*>(&*it) != nullptr) {
            pComponent->setTriggerTarget(true);
        }

        addComponent(pComponent);
        walkInChildren(it->GetChildList());
    }
}

void PaneManager::delLayoutScene(const nw4hbm::lyt::Layout& rLayout) {
    walkInChildrenDel(rLayout.GetRootPane()->GetChildList());
}

void PaneManager::walkInChildrenDel(nw4hbm::lyt::PaneList& rPaneList) {
    for (nw4hbm::lyt::PaneList::Iterator it = rPaneList.GetBeginIter(); it != rPaneList.GetEndIter(); it++) {
        PaneToComponent* p = static_cast<PaneToComponent*>(nw4hbm::ut::List_GetNext(&mPaneToComponent, nullptr));

        while (p != nullptr) {
            if (p->mpPane == &*it) {
                break;
            }

            p = static_cast<PaneToComponent*>(nw4hbm::ut::List_GetNext(&mPaneToComponent, p));
        }

        delComponent(p->mpComponent);
        nw4hbm::ut::List_Remove(&mPaneToComponent, p);
        suIDCounter--;

        if (mpAllocator != nullptr) {
            MEMFreeToAllocator(mpAllocator, p->mpComponent);
            MEMFreeToAllocator(mpAllocator, p);
        } else {
            delete p->mpComponent;
            delete p;
        }

        walkInChildrenDel(it->GetChildList());
    }
}

PaneComponent* PaneManager::getPaneComponentByPane(nw4hbm::lyt::Pane* pPane) {
    for (u32 i = 0; i < nw4hbm::ut::List_GetSize(&mIDToComponent); i++) {
        PaneToComponent* p = static_cast<PaneToComponent*>(nw4hbm::ut::List_GetNth(&mPaneToComponent, i));

        if (p->mpPane == pPane) {
            return p->mpComponent;
        }
    }

    return nullptr;
}

#pragma push
#pragma opt_propagation off

void PaneManager::setAllBoundingBoxComponentTriggerTarget(bool target) {
    for (u32 i = 0; i < nw4hbm::ut::List_GetSize(&mIDToComponent); i++) {
        PaneToComponent* p = static_cast<PaneToComponent*>(nw4hbm::ut::List_GetNth(&mPaneToComponent, i));

        if (nw4hbm::ut::DynamicCast<nw4hbm::lyt::Bounding*>(p->mpPane) != nullptr) {
            p->mpComponent->setTriggerTarget(target);
        }
    }
}

#pragma pop

bool PaneComponent::contain(f32 x, f32 y) {
    if (mpManager == nullptr) {
        return false;
    }

    const nw4hbm::lyt::DrawInfo* pDrawInfo = static_cast<PaneManager*>(mpManager)->getDrawInfo();

    if (pDrawInfo == nullptr) {
        return false;
    }

    nw4hbm::math::MTX34 invGlbMtx;
    PSMTXInverse(mpPane->GetGlobalMtx(), invGlbMtx);

    nw4hbm::math::VEC3 lclPos;
    PSMTXMultVec(invGlbMtx, nw4hbm::math::VEC3(x, y, 0.0f), lclPos);

    nw4hbm::ut::Rect rect = mpPane->GetPaneRect(*pDrawInfo);

    if (rect.left <= lclPos.x && lclPos.x <= rect.right && rect.bottom <= lclPos.y && lclPos.y <= rect.top) {
        return true;
    }

    return false;
}

void PaneComponent::draw() {
    const nw4hbm::lyt::DrawInfo* pDrawInfo = static_cast<PaneManager*>(mpManager)->getDrawInfo();

    if (pDrawInfo == nullptr) {
        return;
    }

    mpPane->GetTranslate();
    nw4hbm::lyt::Size size = mpPane->GetSize();
    const nw4hbm::math::MTX34& rGlbMtx = mpPane->GetGlobalMtx();

    f32 x = rGlbMtx._03;
    f32 y = rGlbMtx._13;

    GXColor color = {255, 0, 0, 255};

    if (mabPointed[0]) {
        color.r = 0;
        color.b = 255;
    }

    drawLine_(x - size.width / 2.0f, y - size.height / 2.0f, x + size.width / 2.0f, y - size.height / 2.0f, 0.0f, 8, color);
    drawLine_(x + size.width / 2.0f, y - size.height / 2.0f, x + size.width / 2.0f, y + size.height / 2.0f, 0.0f, 8, color);
    drawLine_(x + size.width / 2.0f, y + size.height / 2.0f, x - size.width / 2.0f, y + size.height / 2.0f, 0.0f, 8, color);
    drawLine_(x - size.width / 2.0f, y + size.height / 2.0f, x - size.width / 2.0f, y - size.height / 2.0f, 0.0f, 8, color);
}

static bool is_visible(nw4hbm::lyt::Pane* pPane) {
    if (!pPane->IsVisible()) {
        return false;
    }

    if (pPane->GetParent() == nullptr) {
        return true;
    }

    return is_visible(pPane->GetParent());
}

bool PaneComponent::isVisible() {
    return is_visible(mpPane);
}

}
}
