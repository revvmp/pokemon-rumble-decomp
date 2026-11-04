#ifndef GAME_DETAIL_SCRIPTWORLDOBJCONTAINERS_H
#define GAME_DETAIL_SCRIPTWORLDOBJCONTAINERS_H
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


#include <set>
#include <list>
class BWorldObj;
#pragma dont_inline on
typedef std::detail::tree_imp<int,std::less<int>,std::allocator<int> > WorldIdTree;
typedef std::__list_deleter<BWorldObj*,std::allocator<BWorldObj*> > WorldObjectList;
typedef std::allocator<WorldIdTree::node> WorldNodeAllocator;
typedef Metrowerks::number<unsigned long,1> OneNode;
typedef Metrowerks::compressed_pair<OneNode,WorldIdTree::node*> NodeCapacity;
typedef Metrowerks::compressed_pair<WorldNodeAllocator&,NodeCapacity> NodeStorage;
typedef Metrowerks::alloc_ptr<WorldIdTree::node,WorldNodeAllocator&> NodeOwner;
namespace std { template<> pair<WorldIdTree::iterator,bool>::pair(const WorldIdTree::iterator& rFirst,const bool& rSecond); } 
namespace Metrowerks { template<> NodeOwner::alloc_ptr(WorldIdTree::node* p,WorldNodeAllocator& a,OneNode size); } 
namespace Metrowerks { template<> NodeOwner::~alloc_ptr(); } 
namespace Metrowerks { template<> WorldIdTree::node* NodeOwner::operator->() const; } 
namespace Metrowerks { template<> WorldIdTree::node* NodeOwner::get() const; } 
namespace Metrowerks { template<> WorldIdTree::node* NodeOwner::release(); } 
namespace Metrowerks { template<> WorldNodeAllocator& NodeOwner::allocator(); } 
namespace Metrowerks { template<> size_t NodeOwner::capacity() const; } 
namespace Metrowerks { template<> NodeStorage::compressed_pair(WorldNodeAllocator& rFirst,const NodeCapacity rSecond); } 
namespace Metrowerks { template<> NodeCapacity::compressed_pair(const OneNode rFirst,WorldIdTree::node* rSecond); } 
namespace Metrowerks { namespace detail { template<> CompressedPairStorage<WorldNodeAllocator&,NodeCapacity,0>::CompressedPairStorage(WorldNodeAllocator& rFirst,const NodeCapacity rSecond); } } 
namespace Metrowerks { namespace detail { template<> WorldNodeAllocator& CompressedPairStorage<WorldNodeAllocator&,NodeCapacity,0>::first(); } } 
namespace Metrowerks { namespace detail { template<> NodeCapacity& CompressedPairStorage<WorldNodeAllocator&,NodeCapacity,0>::second(); } } 
namespace Metrowerks { namespace detail { template<> const NodeCapacity& CompressedPairStorage<WorldNodeAllocator&,NodeCapacity,0>::second() const; } } 
namespace Metrowerks { namespace detail { template<> CompressedPairStorage<OneNode,WorldIdTree::node*,1>::CompressedPairStorage(const OneNode rFirst,WorldIdTree::node* rSecond); } } 
namespace Metrowerks { namespace detail { template<> const OneNode& CompressedPairStorage<OneNode,WorldIdTree::node*,1>::first() const; } } 
namespace Metrowerks { namespace detail { template<> WorldIdTree::node*& CompressedPairStorage<OneNode,WorldIdTree::node*,1>::second(); } } 
namespace Metrowerks { namespace detail { template<> WorldIdTree::node* const& CompressedPairStorage<OneNode,WorldIdTree::node*,1>::second() const; } } 
namespace Metrowerks { namespace detail { template<> std::allocator<WorldObjectList::node>& CompressedPairStorage<std::allocator<WorldObjectList::node>,WorldObjectList::node_base,1>::first(); } } 
namespace Metrowerks { namespace detail { template<> std::allocator<BWorldObj*>& CompressedPairStorage<std::allocator<BWorldObj*>,unsigned long,1>::first(); } } 
namespace Metrowerks { namespace detail { template<> unsigned long& CompressedPairStorage<std::allocator<BWorldObj*>,unsigned long,1>::second(); } } 
namespace std { namespace detail { template<> template<> tree_node_base* WorldIdTree::find_leaf<int>(const int& rKey,node*& rpPred,bool& rLeft,bool& rLeftmost); } } 
namespace std { namespace detail { template<> WorldIdTree::iterator::__generic_iterator(const WorldIdTree::iterator& rOther); } } 
namespace std { namespace detail { template<> unsigned long WorldIdTree::size() const; } } 
namespace std { namespace detail { template<> unsigned long WorldIdTree::max_size() const; } } 
namespace std { namespace detail { template<> WorldNodeAllocator& WorldIdTree::node_alloc(); } } 
namespace Metrowerks { namespace detail { template<> WorldNodeAllocator& CompressedPairStorage<WorldNodeAllocator,std::detail::tree_root_ptr,1>::first(); } } 
namespace std { namespace detail { template<> std::allocator<int>& WorldIdTree::alloc(); } } 
namespace Metrowerks { namespace detail { template<> std::allocator<int>& CompressedPairStorage<unsigned long,std::allocator<int>,2>::second(); } } 
namespace std { namespace detail { template<> unsigned long& WorldIdTree::sz(); } } 
namespace Metrowerks { namespace detail { template<> unsigned long& CompressedPairStorage<unsigned long,std::allocator<int>,2>::first(); } } 

#endif
