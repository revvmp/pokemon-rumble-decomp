#ifndef MSL_CPP_DETAIL_SHARED_PTR_HOLDER_H
#define MSL_CPP_DETAIL_SHARED_PTR_HOLDER_H

#include <MSL_CPP/compressed_pair.h>

namespace std {
namespace tr1 {

namespace detail {

template <typename T> struct shared_ptr_reference { typedef T& type; };
template <> struct shared_ptr_reference<void> { typedef void type; };

template <typename T, typename D> class shared_ptr_holder {
public:
    shared_ptr_holder(T* pObject, D& rDeleter) : ptr_(pObject, rDeleter) {}
#if defined(CMODEL_EMISSION_LAYOUT)
    __declspec(section ".text.model_shared")
#endif
    ~shared_ptr_holder();
    T* release() {
        T* p = ptr_.first();
        ptr_.first() = nullptr;
        return p;
    }

private:
    Metrowerks::compressed_pair<T*, D&> ptr_;
};

template <typename T, typename D> shared_ptr_holder<T, D>::~shared_ptr_holder() {
    if (ptr_.first()) {
        ptr_.second()(ptr_.first());
    }
}

}

}
}

#endif
