#pragma once

#include <cassert>
#include <cstdint>
#include "tivaware/inc/hw_memmap.h"
#include "tivaware/inc/hw_timer.h"
#include "tivaware/inc/hw_types.h"
#include "tivaware/driverlib/timer.h"
#include "tivaware/driverlib/rom.h"
#include "tivaware/driverlib/rom_map.h"

std::uint32_t TimerIntEnabledGet(std::uint32_t baseAddress);
bool TimerIsEnabled(std::uint32_t baseAddress, std::uint32_t timer);
std::uint32_t TimerMaskedIntStatus(std::uint32_t baseAddress);
std::uint32_t TimerRawIntStatus(std::uint32_t baseAddress);

inline std::uint32_t
TimerIntEnabledGet(std::uint32_t baseAddress)
{
	return HWREG(baseAddress + TIMER_O_IMR);
}

inline bool
TimerIsEnabled(std::uint32_t baseAddress, std::uint32_t timer)
{
	assert((timer == TIMER_A)
		|| (timer == TIMER_B)
		|| (timer == TIMER_BOTH));

	return (HWREG(baseAddress + TIMER_O_CTL) & timer);
}

inline std::uint32_t
TimerMaskedIntStatus(std::uint32_t baseAddress)
{
	return HWREG(baseAddress + TIMER_O_MIS);
}

inline std::uint32_t
TimerRawIntStatus(std::uint32_t baseAddress)
{
	return HWREG(baseAddress + TIMER_O_RIS);
}
