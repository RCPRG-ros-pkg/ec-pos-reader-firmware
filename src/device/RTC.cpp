#include "device/RTC.hpp"

#include "tivaware/driverlib/sysctl.h"
#include "tivaware/driverlib/rom.h"
#include "tivaware/driverlib/rom_map.h"

extern int clockHz;

namespace device {

RTC::RTC()
{
	MAP_SysCtlPeripheralEnable(SYSCTL_PERIPH_HIBERNATE);
	MAP_HibernateEnableExpClk(clockHz);

	MAP_HibernateClockConfig(HIBERNATE_OSC_LOWDRIVE);
	MAP_HibernateRTCEnable();
	MAP_HibernateRTCSet(0);
}

RTC::~RTC()
{
	MAP_HibernateRTCDisable();
	MAP_SysCtlPeripheralDisable(SYSCTL_PERIPH_HIBERNATE);
}

} // namespace device
