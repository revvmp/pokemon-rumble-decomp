#ifndef NW4R_LYT_LAYOUT_H
#define NW4R_LYT_LAYOUT_H
#include <nw4r/lyt/lyt_types.h>
#include <new>
#include <nw4r/ut/ut_LinkList.h>
#include <nw4r/ut/ut_Rect.h>
#include <revolution/mem/allocator.h>

namespace nw4r {
namespace ut {
template <typename T> class TagProcessorBase;
}
namespace lyt {

class AnimResource;
class AnimTransform;
class DrawInfo;
class GroupContainer;
class Pane;
class ResourceAccessor;
struct ResBlockSet;

typedef ut::LinkList<AnimTransform, 4> AnimTransformList;

class Layout {
public:
    Layout();
    virtual ~Layout();
    virtual bool Build(const void* pBinary, ResourceAccessor* pAccessor);
    virtual AnimTransform* CreateAnimTransform();
    virtual AnimTransform* CreateAnimTransform(const void* pBinary, ResourceAccessor* pAccessor);
    virtual AnimTransform* CreateAnimTransform(const AnimResource& rResource, ResourceAccessor* pAccessor);
    virtual void BindAnimation(AnimTransform* pTransform);
    virtual void UnbindAnimation(AnimTransform* pTransform);
    virtual void UnbindAllAnimation();
    virtual bool BindAnimationAuto(const AnimResource& rResource, ResourceAccessor* pAccessor);
    virtual void SetAnimationEnable(AnimTransform* pTransform, bool enable);
    virtual void CalculateMtx(const DrawInfo& rInfo);
    virtual void Draw(const DrawInfo& rInfo);
    virtual void Animate(u32 option = 0);
    virtual void SetTagProcessor(ut::TagProcessorBase<wchar_t>* pProcessor);

    ut::Rect GetLayoutRect() const;

    static MEMAllocator* GetAllocator() {
        return mspAllocator;
    }

    static void SetAllocator(MEMAllocator* pAllocator) {
        mspAllocator = pAllocator;
    }

    static void* AllocMemory(u32 size) {
        return MEMAllocFromAllocator(mspAllocator, size);
    }

    static void FreeMemory(void* pMemory) {
        MEMFreeToAllocator(mspAllocator, pMemory);
    }

    template <typename T> static T* NewObj() {
        void* pMemory = AllocMemory(sizeof(T));
        return pMemory != nullptr ? new (pMemory) T() : nullptr;
    }

    template <typename T, typename A, typename B>
    static T* NewObj(A parameter1, B parameter2) {
        void* pMemory = AllocMemory(sizeof(T));
        return pMemory != nullptr ? new (pMemory) T(parameter1, parameter2) : nullptr;
    }

    template <typename T> static T* NewArray(size_t count) {
        void* pMemory = AllocMemory(count * sizeof(T));
        if (pMemory == nullptr) {
            return nullptr;
        }
        T* const pArray = static_cast<T*>(pMemory);
        for (size_t i = 0; i < count; i++) {
            new (&pArray[i]) T();
        }
        return pArray;
    }

    template <typename T> static void DeleteArray(T* pArray, size_t count) {
        if (pArray != nullptr) {
            for (size_t i = 0; i < count; i++) {
                pArray[i].~T();
            }
            FreeMemory(pArray);
        }
    }

    template <typename T> static void DeletePrimArray(T* pArray) {
        if (pArray != nullptr) {
            FreeMemory(pArray);
        }
    }

    template <typename T> static void DeleteObj(T* pObject) {
        if (pObject != nullptr) {
            pObject->~T();
            FreeMemory(pObject);
        }
    }

protected:
    static Pane* BuildPaneObj(s32 kind, const void* pBinary, const ResBlockSet& rBlocks);

    AnimTransformList mAnimTransList;
    Pane* mpRootPane;
    GroupContainer* mpGroupContainer;
    Size mLayoutSize;

    static MEMAllocator* mspAllocator;
};

}
}

#endif
