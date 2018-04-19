#pragma once

#include "embxx/driver/TimerMgr.h"

namespace driver {

template<typename TSysTickDevice, typename TEventLoop, std::size_t TMaxTimers>
using SysTick =
	embxx::driver::TimerMgr<
		TSysTickDevice,
		TEventLoop,
		TMaxTimers
	>;

} // namespace driver
