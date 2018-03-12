#include "app/Application.hpp"

#include "tivaware/inc/hw_memmap.h"
#include "tivaware/utils/uartstdio.h"

constexpr auto SSIMaster0BitRate = 1250000;
constexpr auto SSIMaster0DataWidth = 13;

namespace app {

/**
 * @brief Constructor
 * @details Initializes ABCC module
 */
Application::Application()
	:	_sysTickMgr(_sysTickDevice, _eventLoop),
		_timer(_sysTickMgr.allocTimer()),
		_ssiMasterDevice(SSI0_BASE, SSIMaster0BitRate, SSIMaster0DataWidth),
		_smrs59(_ssiMasterDevice)
{
	assert(_timer.isValid());

	readEncoder();

	UARTprintf("Application initialized\n");
}

/**
 * @brief Destructor
 * @details Shutdowns ABCC module
 */
Application::~Application()
{

}

/**
 * @brief Starts main loop of application
 * @details
 */
void
Application::run()
{
	_eventLoop.run();
}

void
Application::readEncoder()
{
	const auto position = _smrs59.readPosition();
	UARTprintf("Encoder position: %d\n", position.value());

	_timer.asyncWait(std::chrono::milliseconds(100),
		[this](const auto errorStatus)
		{
			if(errorStatus)
			{
				UARTprintf("Error occured during wait. code=%d\n",
					static_cast<int>(errorStatus.code()));
			}
			else
			{
				readEncoder();
			}
		});
}

} // namespace app


// #include <cassert>
// #include <cstdint>

// #include "tivaware/utils/uartstdio.h"
// #include "tivaware/driverlib/interrupt.h"
// #include "tivaware/driverlib/systick.h"
// #include "tivaware/driverlib/rom.h"
// #include "tivaware/driverlib/rom_map.h"

// #include "abcc_td.h"
// #include "abcc.h"
// #include "abcc_sys_adapt.h"
// #include "ad_obj.h"
// #include "appl_abcc_handler.h"

// #include <chrono>

// #include "common.hpp"

// constexpr auto TimerDelayMs = std::chrono::milliseconds(1);

// static void sysTickISR()
// {
// 	ABCC_RunTimerSystem(TimerDelayMs.count());
// }

// static void setupSysTick()
// {
// 	UARTprintf("Initializing timer...\n");

// 	using Period = decltype(TimerDelayMs)::period;
// 	constexpr auto SysTickPeriod = ClockHz * TimerDelayMs.count() *
// 		Period::num / Period::den;

// 	MAP_SysTickPeriodSet(SysTickPeriod);
// 	SysTickIntRegister(sysTickISR);
// 	MAP_SysTickIntEnable();
// 	MAP_SysTickEnable();
// }

// static void resetModule()
// {
// 	UARTprintf("resetModule!\n");
// 	while(1);
// }

// int main()
// {
// 	UARTprintf("Initializing ABCC hardware...\n");
// 	if( ABCC_HwInit() != ABCC_EC_NO_ERROR )
// 	{
// 		UARTprintf("Error during ABCC_HwInit.\n");
// 		while(1);
// 	}

// 	setupSysTick();

// 	IntMasterEnable();

// 	APPL_AbccHandlerStatusType eAbccHandlerStatus = APPL_MODULE_NO_ERROR;
// 	while( eAbccHandlerStatus == APPL_MODULE_NO_ERROR )
// 	{
// 		eAbccHandlerStatus = APPL_HandleAbcc();

// 		switch( eAbccHandlerStatus )
// 		{
// 		case APPL_MODULE_RESET:
// 			resetModule();
// 			break;
// 		default:
// 			break;
// 		}
// 	}

// 	while(1);
// 	assert(false && "Should not get here!");
//     return 0;
// }

