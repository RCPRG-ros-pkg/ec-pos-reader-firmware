#include <cstdint>
#include "tivaware/inc/hw_memmap.h"
#include "tivaware/driverlib/sysctl.h"
#include "tivaware/driverlib/interrupt.h"
#include "tivaware/driverlib/gpio.h"
#include "tivaware/driverlib/fpu.h"
#include "tivaware/driverlib/pin_map.h"
#include "tivaware/driverlib/ssi.h"
#include "tivaware/driverlib/rom.h"
#include "tivaware/driverlib/rom_map.h"
#include "tivaware/utils/uartstdio.h"

#include "util/driverlib/ssi.hpp"

extern "C" {

//! Global variable which contains speed of system clock
int clockHz;

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
	clockHz = MAP_SysCtlClockGet();
	assert(clockHz != 0);

	// enable GPIO for UART0 module
	MAP_SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOA);
	MAP_GPIOPinTypeUART(GPIO_PORTA_BASE, GPIO_PIN_0 | GPIO_PIN_1);

	// Configure GPIO of pins of SSI0 module. Pull-up SSI0CLK pin
	MAP_SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOA);
	MAP_GPIOPinConfigure(GPIO_PA2_SSI0CLK);
	MAP_GPIOPinConfigure(GPIO_PA3_SSI0FSS);
	MAP_GPIOPinConfigure(GPIO_PA4_SSI0RX);
	MAP_GPIOPinConfigure(GPIO_PA5_SSI0TX);
	MAP_GPIOPinTypeSSI(GPIO_PORTA_BASE,
		GPIO_PIN_2 | GPIO_PIN_3 | GPIO_PIN_4 | GPIO_PIN_5);

	// configure SSI0: TI SSI mode, master, 1.25MHz and 13bits frame width
	MAP_SysCtlPeripheralEnable(SYSCTL_PERIPH_SSI0);

	// enable FPU
	MAP_FPUEnable();
}

//! Initializes IO UART channel
//! Configures UART0 to work with UARTstdio
void initIO()
{
	constexpr auto IOBaudRate = 115200;
	UARTStdioConfig(0, IOBaudRate, clockHz);

	UARTprintf("Init: IO initialized\n");
}

}
