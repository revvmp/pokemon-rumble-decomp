#define GAME_SCRIPTWORLDOBJ_SEPARATE_TEMPLATES
#define GAME_SCRIPTWORLDOBJ_CONTAINER_TAIL
#define GAME_SCRIPTWORLDOBJ_COPY_CTORS
#include <game/detail/ScriptWorldObjContainers.h>
namespace Metrowerks {
template<> NodeCapacity::compressed_pair(const NodeCapacity& rOther) : Storage(rOther) {}
namespace detail {
template<> CompressedPairStorage<OneNode,WorldIdTree::node*,1>::CompressedPairStorage(const CompressedPairStorage<OneNode,WorldIdTree::node*,1>& rOther) : mSecond(rOther.mSecond) {}
}
}
