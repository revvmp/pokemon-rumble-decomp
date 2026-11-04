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
namespace std {
namespace detail {
template<typename T, typename Compare, typename Allocator>
Compare& tree_imp<T,Compare,Allocator>::comp() { return begin_.first(); }
template<typename Compare>
Compare& tree_begin_holder<Compare>::first() { return *this; }
template<typename T, typename Compare, typename Allocator>
tree_node_base*& tree_imp<T,Compare,Allocator>::root() { return *reinterpret_cast<tree_node_base**>(end_node()); }
}
template<class T,class Allocator>
template<bool B>
typename __list_deleter<T,Allocator>::node_base* __list_deleter<T,Allocator>::generic_iterator<B>::base() const { return node_; }
}
template std::less<int>& std::detail::tree_imp<int,std::less<int>,std::allocator<int> >::comp();
template std::less<int>& std::detail::tree_begin_holder<std::less<int> >::first();
template std::detail::tree_node_base*& std::detail::tree_imp<int,std::less<int>,std::allocator<int> >::root();
template std::__list_deleter<BWorldObj*,std::allocator<BWorldObj*> >::node_base* std::__list_deleter<BWorldObj*,std::allocator<BWorldObj*> >::generic_iterator<false>::base() const;
