#pragma once
#include <cstdio>
#include <fmt/format.h>
#define ERROR_LOG_FMT(category, ...) do { fmt::print(stderr, __VA_ARGS__); fmt::print(stderr, "\n"); } while (false)
