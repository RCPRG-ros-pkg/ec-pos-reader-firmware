#pragma once

#include <cassert>
#include <cstdint>

#include "util/driverlib/sysctl.hpp"

namespace device {

template<std::uint32_t TId>
class Peripheral
{
public:
	constexpr static std::uint32_t Id = TId;
	static_assert(Id > 0, "Invalid peripheral ID");

	Peripheral()
	{
		// This peripheral should be present in the device
		assert(SysCtlPeripheralPresent(Id));

		// Peripheral should be now disabled, so enable it
		assert(!SysCtlIsPeripheralEnabled(Id));
		SysCtlPeripheralEnable(Id);

		// Wait for Peripheral to be ready
		while(!SysCtlPeripheralReady(Id))
		{
			/* do nothing */
		}
	}

	~Peripheral()
	{
		// Peripheral should be now enabled, so disable it
		assert(SysCtlIsPeripheralEnabled(Id));
		SysCtlPeripheralDisable(Id);

		// Reset peripheral, that next time it will be in the reset state
		SysCtlPeripheralReset(Id);
	}
};

} // namespace device
