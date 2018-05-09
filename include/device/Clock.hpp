#pragma once

#include <cstdint>
#include <chrono>
#include <limits>

#include "util/driverlib/timer.hpp"

#include "init.hpp"

#include "device/Peripheral.hpp"

namespace device {

template<std::uint32_t TBaseAddress, std::uint32_t TId>
class Clock
	:	public Peripheral<TId>
{
public:
	constexpr static std::uint32_t BaseAddress = TBaseAddress;
	static_assert(BaseAddress != 0,
		"Specified BaseAddess is invalid");

	constexpr static int Frequency = ClockHz;

	using rep = std::uint32_t;
	using period = std::ratio<1, Frequency>;
	using duration = std::chrono::duration<rep, period>;

	using time_point = std::chrono::time_point<Clock<BaseAddress, TId>>;

	constexpr static bool is_steady = true;

	/**
	 * @brief Constructor
	 * @details [long description]
	 */
	Clock()
	{
		TimerConfigure(BaseAddress, TIMER_CFG_PERIODIC_UP);
		TimerLoadSet(BaseAddress, TIMER_A, std::numeric_limits<rep>::max());
		TimerEnable(BaseAddress, TIMER_BOTH);
	}

	~Clock()
	{
		TimerDisable(BaseAddress, TIMER_BOTH);
	}

	time_point now()
	{
		const auto value = TimerValueGet(BaseAddress, TIMER_A);
		return time_point(duration(value));
	}
};

} // namespace device
