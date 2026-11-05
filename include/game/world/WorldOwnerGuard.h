#ifndef GAME_WORLDOWNERGUARD_H
#define GAME_WORLDOWNERGUARD_H
#include <MSL_CPP/compressed_pair.h>
#include <MSL_CPP/allocator.h>
namespace std {
namespace tr1 {
namespace detail {
template <typename T, typename D> class world_owner_guard {
    struct nat { int dummy_; };
public:
    template <typename Y>
    explicit world_owner_guard(Y* p, typename Metrowerks::restrict_to<Metrowerks::is_convertible<Y*, T*>::value, nat>::type = nat()) : ptr_(p) {}
    template <typename E> world_owner_guard(T* p, E& d) : ptr_(p, d) {}
    ~world_owner_guard();
    T* release() {
        T* p = ptr_.first();
        ptr_.first() = nullptr;
        return p;
    }
private:
    Metrowerks::compressed_pair<T*, D> ptr_;
};

template <typename T, typename D> world_owner_guard<T, D>::~world_owner_guard() {
    if (ptr_.first()) {
        ptr_.second()(ptr_.first());
    }
}
}
}
}
#endif
