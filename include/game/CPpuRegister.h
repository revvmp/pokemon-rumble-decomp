#ifndef GAME_CPPUREGISTER_H
#define GAME_CPPUREGISTER_H

#include <memory>
#include <vector>

namespace NPpu {
class Slice;
class CValue;
}

class CPpuRegister {
public:
    CPpuRegister();
    ~CPpuRegister();
    NPpu::CValue GetValue(unsigned int index) const;

private:
    std::tr1::shared_ptr<std::vector<NPpu::Slice> > mValues;
};

#endif
