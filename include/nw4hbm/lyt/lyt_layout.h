#ifndef NW4HBM_LYT_LAYOUT_H
#define NW4HBM_LYT_LAYOUT_H
#include "nw4hbm/types_nw4hbm.h"

#include "nw4hbm/lyt/lyt_animation.h"
#include "nw4hbm/lyt/lyt_types.h"

#include "nw4hbm/ut.h"

#include "revolution/mem.h"

namespace nw4hbm {
namespace ut {

template <typename T>
class TagProcessorBase;

typedef TagProcessorBase<wchar_t> WideTagProcessor;

}

namespace lyt {

class ArcResourceAccessor;
class DrawInfo;
class GroupContainer;
class Pane;

enum OriginType {
    ORIGINTYPE_TOPLEFT,
    ORIGINTYPE_CENTER,

    ORIGINTYPE_MAX
};

namespace res {

struct Layout {
    static const u32 SIGNATURE = FOURCC('l', 'y', 't', '1');

    DataBlockHeader blockHeader;
    u8 originType;
    u8 PADDING_0x9[0xC - 0x9];
    Size layoutSize;
};

}

class Layout {
public:
    static const u32 SIGNATURE = FOURCC('R', 'L', 'Y', 'T');
    static const u32 SIGNATURE_ANIMATION = FOURCC('R', 'L', 'A', 'N');

public:
    Layout();
    virtual ~Layout();

    virtual bool Build(const void* pLytBinary,
                       ResourceAccessor* pAccessor);

    virtual AnimTransform* CreateAnimTransform(const void* pAnmBinary,
                                               ResourceAccessor* pAccessor);

    virtual void BindAnimation(AnimTransform* pAnimTrans);
    virtual void UnbindAnimation(AnimTransform* pAnimTrans);
    virtual void UnbindAllAnimation();
    virtual void SetAnimationEnable(AnimTransform* pAnimTrans,
                                    bool enable);

    virtual void CalculateMtx(const DrawInfo& rInfo);
    virtual void Draw(const DrawInfo& rInfo);
    virtual void Animate(u32 option = 0);

    virtual void SetTagProcessor(ut::WideTagProcessor* pProcessor);

    ut::Rect GetLayoutRect() const;

    Pane* GetRootPane() const {
        return mpRootPane;
    }

    GroupContainer* GetGroupContainer() const {
        return mpGroupContainer;
    }

    static MEMAllocator* GetAllocator() {
        return mspAllocator;
    }
    static void SetAllocator(MEMAllocator* pAllocator) {
        mspAllocator = pAllocator;
    }

    static void* AllocMemory(u32 size) {
        return MEMAllocFromAllocator(mspAllocator, size);
    }
    static void FreeMemory(void* pBlock) {
        MEMFreeToAllocator(mspAllocator, pBlock);
    }

    template <typename TObj> static TObj* NewObj() {
        void* pMem = AllocMemory(sizeof(TObj));

        if (pMem != NULL) {
            return new (pMem) TObj();
        }

        return NULL;
    }

    template <typename TObj, typename TParam> static TObj* NewObj(TParam param1) {
        void* pMem = AllocMemory(sizeof(TObj));

        if (pMem != NULL) {
            return new (pMem) TObj(param1);
        }

        return NULL;
    }

    template <typename TObj, typename TParam1, typename TParam2>
    static TObj* NewObj(TParam1 param1, TParam2 param2) {
        void* pMem = AllocMemory(sizeof(TObj));

        if (pMem != NULL) {
            return new (pMem) TObj(param1, param2);
        }

        return NULL;
    }

    template <typename TObj> static TObj* NewArray(size_t num) {
        void* pMem = AllocMemory(num * sizeof(TObj));

        if(pMem == NULL){
            return NULL;
        }else{
            TObj* const objAry = static_cast<TObj*>(pMem);

            for (size_t i = 0; i < num; i++) {
               new (&objAry[i]) TObj();
            }

            return objAry;
        }
    }

    template <typename TObj> static void DeleteObj(TObj* pObj) {
        if(pObj != NULL){
            pObj->~TObj();
            FreeMemory(pObj);
        }
    }

    template <typename TObj> static void DeleteArray(TObj* objAry, size_t num) {
        if(objAry != NULL){
            for (size_t i = 0; i < num; i++) {
                objAry[i].~TObj();
            }

            FreeMemory(objAry);
            objAry = NULL;
        }
    }

protected:
    static const u32 SIGNATURE_TEXTURELIST = FOURCC('t', 'x', 'l', '1');
    static const u32 SIGNATURE_FONTLIST = FOURCC('f', 'n', 'l', '1');
    static const u32 SIGNATURE_MATERIALLIST = FOURCC('m', 'a', 't', '1');

    static const u32 SIGNATURE_ANIMATIONINFO = FOURCC('p', 'a', 'i', '1');

    static const u32 SIGNATURE_PANESTART = FOURCC('p', 'a', 's', '1');
    static const u32 SIGNATURE_PANEEND = FOURCC('p', 'a', 'e', '1');

    static const u32 SIGNATURE_GROUPSTART = FOURCC('g', 'r', 's', '1');
    static const u32 SIGNATURE_GROUPEND = FOURCC('g', 'r', 'e', '1');

protected:
    static Pane* BuildPaneObj(s32 kind, const void* pBinary,
                              const ResBlockSet& rBlockSet);

protected:
    AnimTransformList mAnimTransList;
    Pane* mpRootPane;
    GroupContainer* mpGroupContainer;
    Size mLayoutSize;
    u8 mOriginType;

    static MEMAllocator* mspAllocator;
};

}
}

#endif
