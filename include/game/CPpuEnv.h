#ifndef GAME_CPPUENV_H
#define GAME_CPPUENV_H

#include <game/NPpuVM.h>
#include <memory>

class CPpuEnv : public NPpu::CEnvironment, public std::tr1::enable_shared_from_this<NPpu::CEnvironment> {
public:
    CPpuEnv(int globalCount, const NPpu::SNativeFunction* pNatives, u32 nativeCount);
};

#endif
