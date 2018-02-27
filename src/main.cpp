/**
 * main.cpp
 * Contains implementation of main function
 * Author: akowalew
 */

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

#include "binary.h" // ETLCPP

int main()
{
	MAP_SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOA);
	MAP_GPIOPinConfigure(GPIO_PA2_SSI0CLK);
	MAP_GPIOPinConfigure(GPIO_PA3_SSI0FSS);
	MAP_GPIOPinConfigure(GPIO_PA4_SSI0RX);
	MAP_GPIOPinConfigure(GPIO_PA5_SSI0TX);
	MAP_GPIOPinTypeSSI(GPIO_PORTA_BASE,
		GPIO_PIN_2 | GPIO_PIN_3 | GPIO_PIN_4 | GPIO_PIN_5);

	MAP_SysCtlPeripheralEnable(SYSCTL_PERIPH_SSI0);
	MAP_SSIConfigSetExpClk(SSI0_BASE, MAP_SysCtlClockGet(), SSI_FRF_TI,
		SSI_MODE_MASTER, 1500000, 13);

	SSIEnable(SSI0_BASE);

	while(1)
	{
		SSIDataPut(SSI0_BASE, 0);
		std::uint32_t data;
		SSIDataGet(SSI0_BASE, &data);
		data = (data & 8191);
		data = etl::gray_to_binary(data);
		UARTprintf("Received data: %d\n", (int)data);
	}

    return 0;
}
