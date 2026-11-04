#define GAME_SCRIPTWORLDOBJ_SEPARATE_TEMPLATES
#define GAME_SCRIPTWORLDOBJ_CONTAINER_TAIL
#include <game/detail/ScriptWorldObjContainers.h>
namespace std { template<> pair<WorldIdTree::iterator,bool>::pair(const WorldIdTree::iterator& rFirst,const bool& rSecond) : first(rFirst),second(rSecond) {} } 
namespace Metrowerks { template<> NodeOwner::alloc_ptr(WorldIdTree::node* p,WorldNodeAllocator& a,OneNode size) : ptr_(a,NodeCapacity(size,p)) {} } 
namespace Metrowerks { template<> NodeOwner::~alloc_ptr() { if(get()!=nullptr) allocator().deallocate(get(),capacity()); } } 
namespace Metrowerks { template<> WorldIdTree::node* NodeOwner::operator->() const {return get();} } 
namespace Metrowerks { template<> WorldIdTree::node* NodeOwner::get() const {return ptr_.second().second();} } 
namespace Metrowerks { template<> WorldIdTree::node* NodeOwner::release() {WorldIdTree::node* p=get();ptr_.second().second()=nullptr;return p;} } 
namespace Metrowerks { template<> WorldNodeAllocator& NodeOwner::allocator() {return ptr_.first();} } 
namespace Metrowerks { template<> size_t NodeOwner::capacity() const {return ptr_.second().first();} } 
namespace Metrowerks { template<> NodeStorage::compressed_pair(WorldNodeAllocator& rFirst,const NodeCapacity rSecond) : Storage(rFirst,rSecond) {} } 
namespace Metrowerks { template<> NodeCapacity::compressed_pair(const OneNode rFirst,WorldIdTree::node* rSecond) : Storage(rFirst,rSecond) {} } 
namespace Metrowerks { namespace detail { template<> CompressedPairStorage<WorldNodeAllocator&,NodeCapacity,0>::CompressedPairStorage(WorldNodeAllocator& rFirst,const NodeCapacity rSecond) : mFirst(rFirst),mSecond(rSecond) {} } } 
namespace Metrowerks { namespace detail { template<> WorldNodeAllocator& CompressedPairStorage<WorldNodeAllocator&,NodeCapacity,0>::first() {return mFirst;} } } 
namespace Metrowerks { namespace detail { template<> NodeCapacity& CompressedPairStorage<WorldNodeAllocator&,NodeCapacity,0>::second() {return mSecond;} } } 
namespace Metrowerks { namespace detail { template<> const NodeCapacity& CompressedPairStorage<WorldNodeAllocator&,NodeCapacity,0>::second() const {return mSecond;} } } 
namespace Metrowerks { namespace detail { template<> CompressedPairStorage<OneNode,WorldIdTree::node*,1>::CompressedPairStorage(const OneNode rFirst,WorldIdTree::node* rSecond) : OneNode(rFirst),mSecond(rSecond) {} } } 
namespace Metrowerks { namespace detail { template<> const OneNode& CompressedPairStorage<OneNode,WorldIdTree::node*,1>::first() const {return *this;} } } 
namespace Metrowerks { namespace detail { template<> WorldIdTree::node*& CompressedPairStorage<OneNode,WorldIdTree::node*,1>::second() {return mSecond;} } } 
namespace Metrowerks { namespace detail { template<> WorldIdTree::node* const& CompressedPairStorage<OneNode,WorldIdTree::node*,1>::second() const {return mSecond;} } } 
namespace Metrowerks { namespace detail { template<> std::allocator<WorldObjectList::node>& CompressedPairStorage<std::allocator<WorldObjectList::node>,WorldObjectList::node_base,1>::first() {return *this;} } } 
namespace Metrowerks { namespace detail { template<> std::allocator<BWorldObj*>& CompressedPairStorage<std::allocator<BWorldObj*>,unsigned long,1>::first() {return *this;} } } 
namespace Metrowerks { namespace detail { template<> unsigned long& CompressedPairStorage<std::allocator<BWorldObj*>,unsigned long,1>::second() {return mSecond;} } } 
namespace std { namespace detail { template<> template<> tree_node_base* WorldIdTree::find_leaf<int>(const int& rKey,node*& rpPred,bool& rLeft,bool& rLeftmost) {
    rpPred = nullptr;
    node* pLeaf = static_cast<node*>(end_node());
    node* pNode = static_cast<node*>(root());
    rLeft = true;
    rLeftmost = true;
    while (pNode != nullptr) {
        pLeaf = pNode;
        if (comp()(rKey, pNode->value_)) {
            pNode = static_cast<node*>(pNode->left_);
            rLeft = true;
        } else {
            rpPred = pLeaf;
            pNode = static_cast<node*>(pNode->right_);
            rLeft = false;
            rLeftmost = false;
        }
    }
    return pLeaf;
} } } 
