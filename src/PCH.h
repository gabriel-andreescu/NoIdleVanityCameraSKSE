#pragma once
#define WIN32_LEAN_AND_MEAN
#define NOMINMAX
#define NOGDI

#include <RE/Skyrim.h>
#include <SKSE/SKSE.h>

#include <string_view>

#define DLLEXPORT __declspec(dllexport)

namespace logger = SKSE::log;

using namespace std::literals;

namespace stl {
using namespace SKSE::stl;
}
