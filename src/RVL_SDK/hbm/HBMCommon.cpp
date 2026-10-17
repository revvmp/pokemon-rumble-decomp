#include "nw4hbm/lyt/lyt_layout.h"

#include "revolution/hbm/homebutton/HBMCommon.h"
#include "revolution/hbm/homebutton/HBMAxSound.h"
#include "revolution/hbm/homebutton/HBMBase.h"
#include "revolution/os.h"

enum HBMAllocatorType {
    HBM_ALLOCATOR_APPLI,
    HBM_ALLOCATOR_LOCAL,
    HBM_ALLOCATOR_NW4R
};

static HBMAllocatorType getAllocatorType(const HBMDataInfo* pHBInfo);

const char* __HBMVersion = "<< RVL_SDK - HBM \trelease build: Mar  3 2009 13:57:36 (0x4201_142) >>";

static MEMAllocator sAllocator;

MEMAllocator* spAllocator = &sAllocator;

void* HBMAllocMem(u32 size) {
    void* pBlock = MEMAllocFromAllocator(spAllocator, size);
    return pBlock;
}

void HBMFreeMem(void* pBlock) {
    MEMFreeToAllocator(spAllocator, pBlock);
}

static HBMAllocatorType getAllocatorType(const HBMDataInfo* pHBInfo) {
    if (pHBInfo->pAllocator) {
        return HBM_ALLOCATOR_APPLI;
    } else if (pHBInfo->mem) {
        return HBM_ALLOCATOR_LOCAL;
    } else {
        return HBM_ALLOCATOR_NW4R;
    }
}

void HBMCreate(const HBMDataInfo* pHBInfo) {
    MEMHeapHandle hExpHeap;

    if (getAllocatorType(pHBInfo) == HBM_ALLOCATOR_LOCAL) {
        hExpHeap = MEMCreateExpHeap(pHBInfo->mem, pHBInfo->memSize);
        MEMInitAllocatorForExpHeap(&sAllocator, hExpHeap, 32);
        spAllocator = &sAllocator;
    }

    switch (getAllocatorType(pHBInfo)) {
    case HBM_ALLOCATOR_APPLI:
        nw4hbm::lyt::Layout::SetAllocator(pHBInfo->pAllocator);
        spAllocator = pHBInfo->pAllocator;
        break;
    case HBM_ALLOCATOR_LOCAL:
        nw4hbm::lyt::Layout::SetAllocator(spAllocator);
        break;
    case HBM_ALLOCATOR_NW4R:
        spAllocator = nw4hbm::lyt::Layout::GetAllocator();
        break;
    }

    homebutton::HomeButton::createInstance(pHBInfo);
    homebutton::HomeButton::getInstance()->create();
}

void HBMDelete() {
    const HBMDataInfo* pHBInfo = homebutton::HomeButton::getInstance()->getHBMDataInfo();

    homebutton::HomeButton::deleteInstance();

    if (getAllocatorType(pHBInfo) == HBM_ALLOCATOR_LOCAL) {
        MEMDestroyExpHeap(static_cast<MEMHeapHandle>(spAllocator->pHeap));
    }
}

void HBMInit() {
    OSRegisterVersion(__HBMVersion);
    homebutton::HomeButton::getInstance()->init();
}

HBMSelectBtnNum HBMCalc(const HBMControllerData* pController) {
    homebutton::HomeButton::getInstance()->calc(pController);
    return homebutton::HomeButton::getInstance()->getSelectBtnNum();
}

void HBMDraw() {
    homebutton::HomeButton::getInstance()->draw();
}

HBMSelectBtnNum HBMGetSelectBtnNum() {
    return homebutton::HomeButton::getInstance()->getSelectBtnNum();
}

void HBMSetAdjustFlag(BOOL flag) {
    homebutton::HomeButton::getInstance()->setAdjustFlag(flag);
}

void HBMStartBlackOut() {
    homebutton::HomeButton::getInstance()->startBlackOut();
}

BOOL HBMIsReassignedControllers() {
    return homebutton::HomeButton::getInstance()->getReassignedFlag();
}

void HBMCreateSound(void* soundData, void* memBuf, u32 memSize) {
    homebutton::HomeButton::getInstance()->getHBMDataInfo();
    homebutton::InitAxSound(soundData, memBuf, memSize);
}

void HBMDeleteSound() {
    homebutton::ShutdownAxSound();
}

void HBMUpdateSound() {
    homebutton::HomeButton::getInstance()->update_sound();
}

void HBMCreateInfoEx(const HBMDataInfoEx* pHBInfoEx) {
    homebutton::HomeButton::getInstance()->createInfoEx(pHBInfoEx);
}
