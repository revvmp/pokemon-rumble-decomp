#ifndef NW4R_LYT_UTILS_H
#define NW4R_LYT_UTILS_H

namespace nw4r {
namespace lyt {

class Group;
class AnimTransform;

void BindAnimation(Group* pGroup, AnimTransform* pTransform, bool recursive, bool disable);

}
}

#endif
