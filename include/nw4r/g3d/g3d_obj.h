#ifndef NW4R_G3D_OBJ_H
#define NW4R_G3D_OBJ_H
#include <nw4r/types_nw4r.h>

#include <nw4r/g3d/g3d_rtti.h>
#include <nw4r/g3d/platform/g3d_allocator.h>

namespace nw4r {
namespace g3d {
namespace {

inline u32 align4(u32 x) {
    return (x + 3) & ~3;
}
inline u32 align32(u32 x) {
    return (x + 31) & ~31;
}

}

class G3dObj {
public:
    template <u32 N>
    struct ResNameDataT {
        u32 len;

        char str[(N + 4) & ~3];
    };

    class TypeObj {
    public:
        struct ResNameDataPT {
            u32 len;
            char str[4];
        };

    public:
        template <u32 N>
        explicit TypeObj(const ResNameDataT<N> &rName) : mName(reinterpret_cast<const ResNameDataPT *>(&rName)) {}

        u32 GetTypeID() const {

            return reinterpret_cast<u32>(mName);
        }

        const char *GetTypeName() const {
            return mName->str;
        }

        bool operator==(const TypeObj &rOther) const {
            return GetTypeID() == rOther.GetTypeID();
        }

    private:
        const ResNameDataPT *mName;
    };

    enum G3dProcTask {
        G3DPROC_NONE = 0x00000,

        G3DPROC_CALC_WORLD = 0x00001,
        G3DPROC_CALC_MAT = 0x00002,
        G3DPROC_CALC_VTX = 0x00003,
        G3DPROC_CALC_VIEW = 0x00004,
        G3DPROC_GATHER_SCNOBJ = 0x00005,
        G3DPROC_DRAW_OPA = 0x00006,
        G3DPROC_DRAW_XLU = 0x00007,
        G3DPROC_UPDATEFRAME = 0x00008,

        __G3DPROC_OPTIONAL_END,

        G3DPROC_CHILD_DETACHED = 0x10001,
        G3DPROC_ATTACH_PARENT = 0x10002,
        G3DPROC_DETACH_PARENT = 0x10003,
        G3DPROC_ZSORT = 0x10004
    };

public:
    virtual bool IsDerivedFrom(TypeObj other) const {
        return other == GetTypeObjStatic();
    }

    virtual void G3dProc(u32 task, u32 param, void *pInfo) = 0;
    virtual ~G3dObj();

    virtual const TypeObj GetTypeObj() const {
        return TypeObj(TYPE_NAME);
    }
    static const G3dObj::TypeObj GetTypeObjStatic() {
        return TypeObj(TYPE_NAME);
    }
    virtual const char *GetTypeName() const {
        return GetTypeObj().GetTypeName();
    }

    G3dObj(MEMAllocator *pAllocator, G3dObj *pParent) : mpHeap(pAllocator), mpParent(pParent) {}

    G3dObj *GetParent() const {
        return mpParent;
    }
    void SetParent(G3dObj *pParent) {
        mpParent = pParent;
    }

    void Destroy();
    G3dObj *DetachFromParent();

    static void *Alloc(MEMAllocator *pAllocator, u32 size) {
        return detail::AllocFromAllocator(pAllocator, size);
    }
    static void Dealloc(MEMAllocator *pAllocator, void *pBlock) {
        detail::FreeToAllocator(pAllocator, pBlock);
    }

    static inline void *operator new(size_t , void *pBlock) {
        return pBlock;
    }
    static inline void operator delete(void * ) {}

    template <typename TTo>
    static TTo *DynamicCast(G3dObj *pObj) {
        if (pObj != NULL && pObj->IsDerivedFrom(TTo::GetTypeObjStatic())) {
            return static_cast<TTo *>(pObj);
        }
        return NULL;
    }

private:
    G3dObj *mpParent;
    MEMAllocator *mpHeap;

    __NW4R_G3D_TYPEOBJ_DECL(G3dObj);
};

}
}

#endif
