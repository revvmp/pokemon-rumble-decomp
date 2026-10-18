#ifndef GAME_RESETCALLBACK_H
#define GAME_RESETCALLBACK_H

#include <functional>

void RegisterResetCallback(const std::tr1::function<void()>& rCallback, int priority);

#endif
