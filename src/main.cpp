/**
 * main.cpp
 * Contains implementation of main function
 * Author: akowalew
 */

#include <cassert>
#include <cstdint>

#include "tivaware/utils/uartstdio.h"
#include "tivaware/inc/hw_memmap.h"

#include "device/SSIMasterDevice.hpp"
#include "hohner/SMRS59.hpp"

int main()
{
	device::SSIMasterDevice ssiMasterDevice(SSI0_BASE);
	hohner::SMRS59 _smrs59(ssiMasterDevice);

	while(1)
	{
		const auto position = _smrs59.readPosition();
		UARTprintf("Encoder position: %d\n", position.value());
	}

	assert(false && "Should not get here!");
    return 0;
}
