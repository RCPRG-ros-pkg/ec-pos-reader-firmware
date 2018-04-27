#pragma once

#include <cstdint>

#include "tivaware/inc/hw_types.h"
#include "tivaware/driverlib/sysctl.h"

#define SYSCTL_PPBASE           0x400fe300
#define SYSCTL_SRBASE           0x400fe500
#define SYSCTL_RCGCBASE         0x400fe600
#define SYSCTL_SCGCBASE         0x400fe700
#define SYSCTL_DCGCBASE         0x400fe800
#define SYSCTL_PCBASE           0x400fe900
#define SYSCTL_PRBASE           0x400fea00

bool SysCtlIsPeripheralEnabled(uint32_t ui32Peripheral);

inline bool
SysCtlIsPeripheralEnabled(uint32_t ui32Peripheral)
{
	return HWREGBITW(SYSCTL_RCGCBASE + ((ui32Peripheral & 0xff00) >> 8),
              ui32Peripheral & 0xff) == 1;
}
