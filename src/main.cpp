/**
 * main.cpp
 * Contains implementation of main function
 * Author: akowalew
 */

#include <cassert>
#include <cstdint>

#include "tivaware/utils/uartstdio.h"
#include "tivaware/driverlib/sysctl.h"
#include "tivaware/driverlib/rom.h"
#include "tivaware/driverlib/rom_map.h"

#include "abcc_td.h"
#include "abcc.h"
#include "abcc_sys_adapt.h"
#include "ad_obj.h"
#include "appl_abcc_handler.h"

int clock;

constexpr auto APPL_TIMER_MS = 1;
static void DelayMs( UINT32 lDelayMs )
{
	SysCtlDelay((clock / 1000) * lDelayMs);
}

static void Reset( void )
{
	UARTprintf("Reset!\n");
	while(1);
}

int main()
{
	clock = MAP_SysCtlClockGet();

	UARTprintf("Main program started.\n");

	APPL_AbccHandlerStatusType eAbccHandlerStatus = APPL_MODULE_NO_ERROR;

	if( ABCC_HwInit() != ABCC_EC_NO_ERROR )
	{
		UARTprintf("Error during ABCC_HwInit.\n");
		while(1);
	}

	UARTprintf("ABCC hardware initialized. Starting main loop.\n");
	while( eAbccHandlerStatus == APPL_MODULE_NO_ERROR )
	{
		eAbccHandlerStatus = APPL_HandleAbcc();

		ABCC_RunTimerSystem( APPL_TIMER_MS );
		DelayMs( APPL_TIMER_MS );

		switch( eAbccHandlerStatus )
		{
		case APPL_MODULE_RESET:
			Reset();
			break;
		default:
			break;
		}
	}

	while(1);
	assert(false && "Should not get here!");
    return 0;
}
