#ifndef MSL_CPP_DETAIL_SHARED_PTR_H
#define MSL_CPP_DETAIL_SHARED_PTR_H

#include <stddef.h>

namespace std {
class type_info;
namespace tr1 {
namespace detail {
class shared_ptr_deleter_common {
public:
    shared_ptr_deleter_common();
    virtual ~shared_ptr_deleter_common();
    void release();
    void release_weak();
    virtual const void* get_deleter(const type_info&) const = 0;
    virtual void dispose() = 0;
private:
    ptrdiff_t use_count_;
    ptrdiff_t weak_count_;
};

}
}
}

#endif
