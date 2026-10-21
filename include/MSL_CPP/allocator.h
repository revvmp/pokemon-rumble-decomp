#ifndef MSL_CPP_ALLOCATOR_H
#define MSL_CPP_ALLOCATOR_H

#include <new>
#include <stddef.h>

namespace std {
template <class T> class allocator {
public:
    typedef T value_type;
    typedef T* pointer;
    typedef const T* const_pointer;
    typedef T& reference;
    typedef const T& const_reference;
    typedef size_t size_type;
    typedef ptrdiff_t difference_type;
    template <class U> struct rebind { typedef allocator<U> other; };
    allocator() {}
    template <class U> allocator(const allocator<U>&) {}
    pointer allocate(size_type count, const void* pHint = nullptr) {
        return static_cast<pointer>(::operator new(count * sizeof(T)));
    }
    size_type max_size() const { return size_type(-1) / sizeof(T); }
    void deallocate(pointer pValue, size_type) { ::operator delete(pValue); }
    void construct(pointer pValue, const_reference rValue) { new (pValue) T(rValue); }
    void destroy(pointer pValue) { pValue->~T(); }
};
}

#endif
