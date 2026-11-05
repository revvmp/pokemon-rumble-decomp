#define GAME_SCRIPTWORLDOBJ_SEPARATE_TEMPLATES
#define GAME_SCRIPTWORLDOBJ_CONTAINER_TAIL
#include <game/script/detail/ScriptWorldObjContainers.h>
namespace std { namespace detail { template<> WorldIdTree::iterator::__generic_iterator(const WorldIdTree::iterator& rOther) : node_(rOther.node_) {} } } 
namespace std { namespace detail { template<> unsigned long WorldIdTree::size() const {return sz();} } } 
namespace std { namespace detail { template<> unsigned long WorldIdTree::max_size() const {return numeric_limits<unsigned long>::max();} } } 
namespace std { namespace detail { template<> WorldNodeAllocator& WorldIdTree::node_alloc() {return root_.first();} } } 
namespace Metrowerks { namespace detail { template<> WorldNodeAllocator& CompressedPairStorage<WorldNodeAllocator,std::detail::tree_root_ptr,1>::first() {return *this;} } } 
namespace std { namespace detail { template<> std::allocator<int>& WorldIdTree::alloc() {return size_.second();} } } 
namespace Metrowerks { namespace detail { template<> std::allocator<int>& CompressedPairStorage<unsigned long,std::allocator<int>,2>::second() {return *this;} } } 
namespace std { namespace detail { template<> unsigned long& WorldIdTree::sz() {return size_.first();} } } 
namespace Metrowerks { namespace detail { template<> unsigned long& CompressedPairStorage<unsigned long,std::allocator<int>,2>::first() {return mFirst;} } } 
