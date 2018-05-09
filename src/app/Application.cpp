#include "app/Application.hpp"

#include "tivaware/driverlib/interrupt.h"
#include "tivaware/utils/uartstdio.h"

namespace app {

/**
 * @brief Constructor
 * @details
 */
Application::Application()
	:	_blinker(_eventLoop)
		,_encoder0(_eventLoop)
		,_encoder1(_eventLoop)
		,_etherCAT(_eventLoop, _encoder0, _encoder1)
{
	UARTprintf("[Application] initialized\n");

	IntMasterDisable();
}

/**
 * @brief Destructor
 * @details Shutdowns ABCC module
 */
Application::~Application()
{
	UARTprintf("[Application] cleaned up\n");
}

/**
 * @brief Starts main loop of application
 * @details
 */
void
Application::run()
{
	start();

	IntMasterEnable();
	_eventLoop.run();
	assert(!"Should not get here");
}

void
Application::start()
{
	UARTprintf("[Application] starting...\n");

	// start modules
	_blinker.start();
	_etherCAT.start();

	UARTprintf("[Application] started\n");
}

} // namespace app
