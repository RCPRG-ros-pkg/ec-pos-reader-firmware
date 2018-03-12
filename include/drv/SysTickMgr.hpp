#pragma once

#include "embxx/driver/TimerMgr.h"

#include "util/EventLoop.hpp"
#include "dev/SysTickDevice.hpp"

namespace drv {

constexpr auto SysTickTimersMax = 1;

using SysTickMgr =
	embxx::driver::TimerMgr<
		dev::SysTickDevice,
		util::EventLoop,
		SysTickTimersMax
	>;

} // namespace drv
