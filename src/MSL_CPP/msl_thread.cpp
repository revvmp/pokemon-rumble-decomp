#include <MSL_CPP/detail/shared_ptr.h>

namespace std {
namespace tr1 {
namespace detail {

void shared_ptr_deleter_common::release() {
    if (--use_count_ == 0) {
        dispose();
        release_weak();
    }
}

void shared_ptr_deleter_common::release_weak() {
    if (--weak_count_ == 0) {
        delete this;
    }
}
}
}
}
