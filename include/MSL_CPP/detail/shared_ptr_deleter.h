#ifndef MSL_CPP_DETAIL_SHARED_PTR_DELETER_H
#define MSL_CPP_DETAIL_SHARED_PTR_DELETER_H

#include <MSL_CPP/detail/shared_ptr.h>
#include <typeinfo>
#include <revolution/types.h>

namespace std {
namespace detail {
template <typename T> struct default_delete {
    void operator()(T* pObject) const { delete pObject; }
};
}

namespace tr1 {
namespace detail {

template <typename T, typename D> struct shared_ptr_storage : private D {
    explicit shared_ptr_storage(T* pObject) : mpObject(pObject) {}
    shared_ptr_storage(T* pObject, const D& rDeleter) : D(rDeleter), mpObject(pObject) {}
    const D* GetDeleter() const { return this; }
    T* mpObject;
};

template <typename T, typename U> struct shared_ptr_storage<T, void (*)(U*)> {
    shared_ptr_storage(T* pObject, void (*deleter)(U*)) : mpObject(pObject), mDeleter(deleter) {}
    void (*const* GetDeleter() const)(U*) { return &mDeleter; }
    T* mpObject;
    void (*mDeleter)(U*);
};

template <typename T, typename D> class shared_ptr_deleter : public shared_ptr_deleter_common {
public:
    explicit shared_ptr_deleter(T* pObject) : mStorage(pObject) {}
    shared_ptr_deleter(T* pObject, const D& rDeleter) : mStorage(pObject, rDeleter) {}
    virtual const void* get_deleter(const type_info& rType) const {
        return rType == typeid(D) ? mStorage.GetDeleter() : nullptr;
    }
    virtual void dispose() { (*mStorage.GetDeleter())(mStorage.mpObject); }

private:
    shared_ptr_storage<T, D> mStorage;
};

}
}
}

#endif
