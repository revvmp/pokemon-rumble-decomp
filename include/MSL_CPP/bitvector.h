#ifndef MSL_CPP_BITVECTOR_H
#define MSL_CPP_BITVECTOR_H

#include <algorithm>
#include <MSL_CPP/allocator.h>
#include <MSL_CPP/compressed_pair.h>

namespace std {

template <typename T, typename Allocator> class vector;

class __bit_reference {
public:
    typedef unsigned int __storage_type;

    __bit_reference(__storage_type* pWord, __storage_type mask) : p_(pWord), m_(mask) {}

    operator bool() const { return (*p_ & m_) != 0; }
    __bit_reference& operator=(bool x) {
        if (x) {
            *p_ |= m_;
        } else {
            *p_ &= ~m_;
        }
        return *this;
    }

private:
    __storage_type* p_;
    __storage_type m_;
};

class __bit_iterator {
public:
    typedef unsigned int __storage_type;
    typedef unsigned long size_type;

    __bit_iterator(__storage_type* pWord, __storage_type mask) : p_(pWord), m_(mask) {}

    __bit_reference operator*() const { return __bit_reference(p_, m_); }

    __storage_type* p_;
    __storage_type m_;
};

inline void __bit_fill_n(__bit_iterator::__storage_type* p, __bit_iterator::__storage_type mask,
                         __bit_iterator::size_type n, const bool& x) {
    typedef __bit_iterator::__storage_type __storage_type;
    typedef __bit_iterator::size_type size_type;

    if (n == 0) {
        return;
    }

    size_type clz = __cntlzw(mask);
    if (x) {
        for (;;) {
            size_type dn = min(clz + 1, n);
            __storage_type m = (~__storage_type(0) << (31 - clz)) & (~__storage_type(0) >> (clz + 1 - dn));
            *p |= m;
            n -= dn;
            if (n == 0) {
                break;
            }
            clz = 31;
            ++p;
        }
    } else {
        for (;;) {
            size_type dn = min(clz + 1, n);
            __storage_type m = (~__storage_type(0) << (31 - clz)) & (~__storage_type(0) >> (clz + 1 - dn));
            *p &= ~m;
            n -= dn;
            if (n == 0) {
                break;
            }
            clz = 31;
            ++p;
        }
    }
}

template <typename Allocator> class __bitvec_deleter {
public:
    typedef unsigned int __storage_type;
    typedef unsigned long size_type;
    typedef typename Allocator::template rebind<__storage_type>::other storage_allocator;

    __bitvec_deleter() : data_(nullptr), size_(0), capacity_(0) {}
    ~__bitvec_deleter() {
        if (data_) {
            capacity_.second().deallocate(data_, capacity_.first());
        }
    }

    __storage_type* data_;
    size_type size_;
    Metrowerks::compressed_pair<size_type, storage_allocator> capacity_;

private:
    __bitvec_deleter(const __bitvec_deleter&);
    __bitvec_deleter& operator=(const __bitvec_deleter&);
};

template <typename Allocator> class __bitvec_base : protected __bitvec_deleter<Allocator> {
public:
    ~__bitvec_base() {}
};

template <typename Allocator> class vector<bool, Allocator> : private __bitvec_base<Allocator> {
    typedef __bitvec_deleter<Allocator> base;

public:
    typedef bool value_type;
    typedef unsigned long size_type;
    typedef __bit_reference reference;
    typedef __bit_iterator iterator;
    typedef typename base::__storage_type __storage_type;

    explicit vector(size_type n, bool x = false) {
        if (n != 0) {
            allocate(n);
            this->size_ = n;
            __bit_fill_n(this->data_, 1, n, x);
        }
    }
    ~vector() {}

    iterator begin() { return iterator(this->data_, 1); }
    size_type size() const { return this->size_; }

    reference operator[](size_type n) { return *iterator(this->data_ + n / 32, __storage_type(1) << n % 32); }

private:
    void allocate(size_type n) {
        size_type words = (n - 1) / 32 + 1;
        this->data_ = this->capacity_.second().allocate(words);
        this->capacity_.first() = words;
    }
};

}

#endif
