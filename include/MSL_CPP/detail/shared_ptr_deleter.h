#ifndef MSL_CPP_DETAIL_SHARED_PTR_DELETER_H
#define MSL_CPP_DETAIL_SHARED_PTR_DELETER_H

#include <MSL_CPP/compressed_pair.h>
#include <MSL_CPP/detail/shared_ptr.h>
#include <typeinfo>
#include <revolution/types.h>

namespace std {
namespace tr1 {
namespace detail {

template <typename T, typename D> class shared_ptr_deleter : public shared_ptr_deleter_common {
public:
    explicit shared_ptr_deleter(T* pObject) : mStorage(pObject) {}
    shared_ptr_deleter(T* pObject, const D& rDeleter) : mStorage(pObject, rDeleter) {}
#if defined(CMODEL_HELPER_EMISSION_LAYOUT)
    virtual ~shared_ptr_deleter();
    virtual const void* get_deleter(const type_info& rType) const;
    virtual void dispose();
#else
#if defined(CMODEL_EMISSION_LAYOUT)
    __declspec(section ".text.model_owners")
#endif
    virtual const void* get_deleter(const type_info& rType) const {
#if defined(SAVEDATA_EMISSION_LAYOUT)
        return rType == SaveDataEmission::DeleterTypeInfo<D>::Get() ? &mStorage.second() : nullptr;
#else
        return rType == typeid(D) ? &mStorage.second() : nullptr;
#endif
    }
#if defined(CMODEL_EMISSION_LAYOUT)
    __declspec(section ".text.model_owners")
#endif
    virtual void dispose() { mStorage.second()(mStorage.first()); }
#endif

private:
    Metrowerks::compressed_pair<T*, D> mStorage;
};

}
}
}

#endif
