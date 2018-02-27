#include <cstdint>
#include "tivaware/inc/hw_memmap.h"
#include "tivaware/driverlib/sysctl.h"
#include "tivaware/driverlib/gpio.h"
#include "tivaware/driverlib/pin_map.h"
#include "tivaware/driverlib/rom.h"
#include "tivaware/driverlib/uart.h"
#include "tivaware/utils/uartstdio.h"

extern "C" {

void preinitHardware()
{
    ROM_SysCtlClockSet(SYSCTL_SYSDIV_4
        | SYSCTL_USE_PLL
        | SYSCTL_XTAL_16MHZ
        | SYSCTL_OSC_MAIN);
}

void initIO()
{
	ROM_SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOA);
	ROM_GPIOPinTypeUART(GPIO_PORTA_BASE, GPIO_PIN_0 | GPIO_PIN_1);

	constexpr auto baudRate = 115200;
	const auto systemClock = ROM_SysCtlClockGet();
	UARTStdioConfig(0, baudRate, systemClock);
}

void initHardware()
{

}

}
