#include "dev/RTCDevice.hpp"

#include "tivaware/driverlib/sysctl.h"
#include "tivaware/driverlib/rom.h"
#include "tivaware/driverlib/rom_map.h"

extern int clockHz;

namespace dev {

RTCDevice::RTCDevice()
{
	MAP_SysCtlPeripheralEnable(SYSCTL_PERIPH_HIBERNATE);
	MAP_HibernateEnableExpClk(clockHz);

	MAP_HibernateClockConfig(HIBERNATE_OSC_LOWDRIVE);
	MAP_HibernateRTCEnable();
	MAP_HibernateRTCSet(0);
}

RTCDevice::~RTCDevice()
{
	MAP_HibernateRTCDisable();
	MAP_SysCtlPeripheralDisable(SYSCTL_PERIPH_HIBERNATE);
}

} // namespace dev
