#ifndef GAME_CPPUREGISTER_H
#define GAME_CPPUREGISTER_H

#include <memory>
namespace std {
template <class T> class allocator;
template <class T, class Allocator> class vector;
}

namespace NPpu {
class Slice;
typedef Slice CValue;
}

class CPpuRegister {
public:
    CPpuRegister();
    ~CPpuRegister();
    NPpu::CValue GetValue(unsigned int index) const;
    void Set(u32 index, const NPpu::CValue& rValue);
    void Swap(CPpuRegister* pOther);
private:
    std::tr1::shared_ptr<std::vector<NPpu::Slice, std::allocator<NPpu::Slice> > > mValues;
};

#endif
