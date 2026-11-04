#ifndef MSL_CPP_ALLOC_PTR_H
#define MSL_CPP_ALLOC_PTR_H

#include <MSL_CPP/compressed_pair.h>
#include <stddef.h>
#include <revolution/types.h>

namespace Metrowerks {
template <typename T, T Value> struct number {
    operator T() const { return Value; }
};

template <typename T, typename Allocator, typename Size = number<size_t, 1> > class alloc_ptr {
public:
    alloc_ptr(T* p, Allocator a, Size size = Size()) : ptr_(a, compressed_pair<Size, T*>(size, p)) {}
#if defined(CFILEDATA_EMISSION_LAYOUT)
    ~alloc_ptr();
#else
    ~alloc_ptr() {
        if (get() != nullptr) {
            allocator().deallocate(get(), capacity());
        }
    }
#endif
    T* operator->() const { return get(); }
    T* get() const { return ptr_.second().second(); }
    T* release() {
        T* p = get();
        ptr_.second().second() = nullptr;
        return p;
    }
    typename call_traits<Allocator>::reference allocator() { return ptr_.first(); }
    size_t capacity() const { return ptr_.second().first(); }
private:
    alloc_ptr(const alloc_ptr&);
    alloc_ptr& operator=(const alloc_ptr&);

    compressed_pair<Allocator, compressed_pair<Size, T*> > ptr_;
};
#if defined(CFILEDATA_EMISSION_LAYOUT)
template <typename T, typename Allocator, typename Size>
    alloc_ptr<T, Allocator, Size>::~alloc_ptr() {
        if (get() != nullptr) {
            allocator().deallocate(get(), capacity());
        }
    }
#endif
}

#endif
