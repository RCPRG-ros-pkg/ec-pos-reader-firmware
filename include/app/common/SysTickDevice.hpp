#pragma once

#include "device/SysTick.hpp"

namespace app {
namespace common {

// constexpr static auto SysTickFrequency = 1000; // Hz
using SysTickDevice = device::SysTick;

} // namespace common
} // namespace app
