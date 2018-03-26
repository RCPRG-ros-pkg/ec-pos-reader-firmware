#pragma once

#include "driver/SysTick.hpp"

#include "app/common/EventLoop.hpp"
#include "app/common/SysTickDevice.hpp"

namespace app {
namespace common {

constexpr static auto SysTickTimersMax = 3;

using SysTickDriver =
	driver::SysTick<SysTickDevice, EventLoop, SysTickTimersMax>;

} // namespace common
} // namespace app
