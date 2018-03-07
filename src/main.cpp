/**
 * main.cpp
 * Contains implementation of main function
 * Author: akowalew
 */

#include <cassert>
#include <cstdint>

#include "tivaware/utils/uartstdio.h"
#include "tivaware/driverlib/interrupt.h"
#include "tivaware/driverlib/systick.h"
#include "tivaware/driverlib/rom.h"
#include "tivaware/driverlib/rom_map.h"

#include "abcc_td.h"
#include "abcc.h"
#include "abcc_sys_adapt.h"
#include "ad_obj.h"
#include "appl_abcc_handler.h"

#include <chrono>

#include "common.hpp"

constexpr auto TimerDelayMs = std::chrono::milliseconds(1);

static void sysTickISR()
{
	ABCC_RunTimerSystem(TimerDelayMs.count());
}

static void setupSysTick()
{
	UARTprintf("Initializing timer...\n");

	using Period = decltype(TimerDelayMs)::period;
	constexpr auto SysTickPeriod = ClockHz * TimerDelayMs.count() *
		Period::num / Period::den;

	MAP_SysTickPeriodSet(SysTickPeriod);
	SysTickIntRegister(sysTickISR);
	MAP_SysTickIntEnable();
	MAP_SysTickEnable();
}

static void resetModule()
{
	UARTprintf("resetModule!\n");
	while(1);
}

int main()
{
	UARTprintf("Initializing ABCC hardware...\n");
	if( ABCC_HwInit() != ABCC_EC_NO_ERROR )
	{
		UARTprintf("Error during ABCC_HwInit.\n");
		while(1);
	}

	setupSysTick();

	IntMasterEnable();

	APPL_AbccHandlerStatusType eAbccHandlerStatus = APPL_MODULE_NO_ERROR;
	while( eAbccHandlerStatus == APPL_MODULE_NO_ERROR )
	{
		eAbccHandlerStatus = APPL_HandleAbcc();

		switch( eAbccHandlerStatus )
		{
		case APPL_MODULE_RESET:
			resetModule();
			break;
		default:
			break;
		}
	}

	while(1);
	assert(false && "Should not get here!");
    return 0;
}
