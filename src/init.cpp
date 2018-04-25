#include <cstdint>
#include "tivaware/inc/hw_memmap.h"
#include "tivaware/driverlib/sysctl.h"
#include "tivaware/driverlib/interrupt.h"
#include "tivaware/driverlib/gpio.h"
#include "tivaware/driverlib/pin_map.h"
#include "tivaware/driverlib/ssi.h"
#include "tivaware/driverlib/rom.h"
#include "tivaware/driverlib/rom_map.h"
#include "tivaware/utils/uartstdio.h"

#include "util/driverlib/ssi.hpp"

#include "init.hpp"

extern "C" {

//! Initializes early hardware
//! Sets up system clock to the 80MHz
void preinitHardware()
{
    MAP_SysCtlClockSet(SYSCTL_SYSDIV_2_5
        | SYSCTL_USE_PLL
        | SYSCTL_XTAL_16MHZ
        | SYSCTL_OSC_MAIN);
}

//! Initializes additional hardware used by software
//! Postcondition: clockHz will be set to non-zero value
void initHardware()
{
	MAP_SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOA);
	MAP_SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOB);
	MAP_SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOF);
	MAP_SysCtlPeripheralEnable(SYSCTL_PERIPH_SSI0);
	MAP_SysCtlPeripheralEnable(SYSCTL_PERIPH_SSI1);
	MAP_SysCtlPeripheralEnable(SYSCTL_PERIPH_SSI2);
	MAP_SysCtlPeripheralEnable(SYSCTL_PERIPH_TIMER0);

	// Configure GPIO of pins of SSI0 module. Pull-up SSI0CLK pin
	MAP_GPIOPinConfigure(GPIO_PA2_SSI0CLK);
	MAP_GPIOPinConfigure(GPIO_PA4_SSI0RX);
	MAP_GPIOPinTypeSSI(GPIO_PORTA_BASE,
		GPIO_PIN_2 | GPIO_PIN_4);
}

//! Initializes IO UART channel
//! Configures UART0 to work with UARTstdio
void initIO()
{
	// enable GPIO for UART0 module
	MAP_GPIOPinTypeUART(GPIO_PORTA_BASE, GPIO_PIN_0 | GPIO_PIN_1);

	constexpr auto IOBaudRate = 115200;
	UARTStdioConfig(0, IOBaudRate, ClockHz);

	UARTprintf("[Init] IO initialized\n");
}

}
