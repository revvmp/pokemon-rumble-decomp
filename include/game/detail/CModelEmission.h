#ifndef GAME_DETAIL_CMODEL_EMISSION_H
#define GAME_DETAIL_CMODEL_EMISSION_H

#include <MSL_CPP/compressed_pair.h>

namespace nw4r { namespace g3d { class AnmObjChrRes; } }
namespace ModelEmission {
template <class T, class D> class ResourceGuard {
public:
    ResourceGuard(T* pObject, D& rDeleter) : mStorage(pObject, rDeleter) {}
    __declspec(section ".text.model_holder") ~ResourceGuard();
    T* release() {
        T* pObject = mStorage.first();
        mStorage.first() = nullptr;
        return pObject;
    }

private:
    Metrowerks::compressed_pair<T*, D&> mStorage;
};
template <class T, class D> ResourceGuard<T, D>::~ResourceGuard() {
    if (mStorage.first()) {
        mStorage.second()(mStorage.first());
    }
}
}

#endif
