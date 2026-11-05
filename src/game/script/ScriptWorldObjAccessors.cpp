#include <MSL_CPP/functional_base.h>

#pragma push
#pragma dont_inline on
namespace std {
template <> struct less<int> : binary_function<int, int, bool> {
    less() {}
    bool operator()(const int& a, const int& b) const { return a < b; }
};
}
#pragma pop

#define GAME_SCRIPTWORLDOBJ_SEPARATE_TEMPLATES
#include <set>
#include <list>
class BWorldObj;
#pragma dont_inline on

namespace std { namespace detail {
template<> less<int>& tree_begin_holder<less<int> >::first();
template<> less<int>& tree_imp<int,less<int>,allocator<int> >::comp() {return begin_.first();}
template<> less<int>& tree_begin_holder<less<int> >::first() {return *this;}
template<> tree_node_base*& tree_imp<int,less<int>,allocator<int> >::root() {return *reinterpret_cast<tree_node_base**>(end_node());}
}
template<> template<> __list_deleter<BWorldObj*,allocator<BWorldObj*> >::node_base* __list_deleter<BWorldObj*,allocator<BWorldObj*> >::generic_iterator<false>::base() const {return node_;}
}
