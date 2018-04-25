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
		// _encoders(),
		// _etherCAT(_eventLoop, _encoders)
{
	UARTprintf("[Application] initialized\n");
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
	startModules();

	IntMasterEnable();
	_eventLoop.run();
	assert(!"Should not get here");
}

void
Application::startModules()
{
	UARTprintf("[Application] starting modules...\n");

	// start modules
	_blinker.start();
	// _etherCAT.start();

	UARTprintf("[Application] modules started\n");

}

} // namespace app
