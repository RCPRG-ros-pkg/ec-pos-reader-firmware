#include "app/Application.hpp"

#include "tivaware/driverlib/interrupt.h"
#include "tivaware/utils/uartstdio.h"

namespace app {

/**
 * @brief Constructor
 * @details
 */
Application::Application()
	:	_redLEDPin(_gpiofDevice),
		_greenLEDPin(_gpiofDevice),
		_sysTickDriver(_sysTickDevice, _eventLoop),
		_redLED(_redLEDPin),
		_greenLED(_greenLEDPin),
		_blinker(_sysTickDriver, _gpiofDevice),
		_encoders(_sysTickDriver),
		_etherCAT(_eventLoop, _sysTickDriver)
{
	UARTprintf("[Application] initialized\n");

	_redLEDPin.setAsDigitalOutput();
	_greenLEDPin.setAsDigitalOutput();

	assert(_blinker.isReady());
	assert(_encoders.isReady());
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
	assert(!_blinker.isActive());

	start();

	IntMasterEnable();
	_eventLoop.run();
	assert(!"Should not get here");
}

void
Application::start()
{
	UARTprintf("[Application] starting modules...\n");

	// turn leds to signal start procedure - yellow color
	_redLED.turnOn();
	_greenLED.turnOn();

	// start modules
	_blinker.start();
	_encoders.start();

	// wait for modules to be started
	_eventLoop.busyWait(
		[this]()
		{
			const auto endWait = (!_blinker.isReady()
				&& !_encoders.isReady());
			return endWait;
		},
		[this]()
		{
			if(!_blinker.isActive() || !_encoders.isActive())
			{
				UARTprintf("[Application] modules not started\n");
				halt();
				return;
			}

			UARTprintf("[Application] modules started\n");
			_redLED.turnOff();
			_greenLED.turnOff();

			_eventLoop.busyWait(
				[this]()
				{
					const auto endWait = (_blinker.isFailed()
						|| _encoders.isFailed());
					return endWait;
				},
				[this]()
				{
					UARTprintf("[Application] Blinker module failed\n");
					halt();
				});
		});
}

void
Application::halt()
{
	UARTprintf("[Application] halted\n");

	// leave red led to signal error
	_redLED.turnOn();
	_greenLED.turnOff();

	// stop modules
	_blinker.stop();
	_encoders.stop();

	// wait for modules to be stopped and execute forever loop
	_eventLoop.busyWait(
		[this]()
		{
			const auto endWait = (!_blinker.isActive()
				&& !_blinker.isActive());
			return endWait;
		},
		[]()
		{
			UARTprintf("[Application] halting...\n");
			IntMasterDisable();
			while(1);
		});
}

} // namespace app
