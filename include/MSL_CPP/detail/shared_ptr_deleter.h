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

template <typename T, typename D> class shared_ptr_deleter : public shared_ptr_deleter_common {
public:
    explicit shared_ptr_deleter(T* pObject) : mStorage(pObject) {}
    virtual const void* get_deleter(const type_info& rType) const {
        return rType == typeid(D) ? mStorage.GetDeleter() : nullptr;
    }
    virtual void dispose() { (*mStorage.GetDeleter())(mStorage.mpObject); }

private:
    struct Storage : private D {
        explicit Storage(T* pObject) : mpObject(pObject) {}
        const D* GetDeleter() const { return this; }
        T* mpObject;
    };

    Storage mStorage;
};

}
}
}

#endif
