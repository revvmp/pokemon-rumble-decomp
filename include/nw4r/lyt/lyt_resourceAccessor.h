#ifndef NW4R_LYT_RESOURCE_ACCESSOR_H
#define NW4R_LYT_RESOURCE_ACCESSOR_H

#include <nw4r/types_nw4r.h>

namespace nw4r {
namespace ut {
class Font;
}
namespace lyt {

class ResourceAccessor {
public:
    ResourceAccessor();
    virtual ~ResourceAccessor();
    virtual void* GetResource(u32 type, const char* pName, u32* pSize) = 0;
    virtual ut::Font* GetFont(const char* pName);
};

}
}

#endif
