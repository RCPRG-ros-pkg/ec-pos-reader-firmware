#include "app/blinker/Blinker.hpp"

#include <cstdint>
#include "tivaware/utils/uartstdio.h"

namespace app {
namespace blinker {

Blinker::Blinker(common::SysTickDriver& sysTickDriver,
	device::GPIOF& gpioFDevice)
	:	_ledPin(gpioFDevice),
		_timer(sysTickDriver.allocTimer()),
		_led(_ledPin)
{
	assert(_timer.isValid());

	_ledPin.setAsDigitalOutput();

	UARTprintf("[Blinker] ready\n");

	assert(_led.isTurnedOff());
	assert(_moduleState == ModuleState::Ready);
}

void
Blinker::start()
{
	if(_moduleState == ModuleState::Active)
	{
		return; // nothing to do
	}

	UARTprintf("[Blinker] starting...\n");

	_moduleState = ModuleState::Active;
	_running = true;
	execWait();

	UARTprintf("[Blinker] started\n");
	assert(_running);
	assert(_moduleState == ModuleState::Active);
}

void
Blinker::stop()
{
	if(_moduleState != ModuleState::Active)
	{
		return; // nothing to do
	}

	UARTprintf("[Blinker] stopping...\n");

	_running = false; // signal stop request

	assert(!_running);
}

void
Blinker::execWait()
{
	assert(_moduleState == ModuleState::Active);

	_timer.asyncWait(std::chrono::milliseconds(1000),
		[this](const auto& errorStatus)
		{
			assert(!errorStatus); // no errors will occur
			static_cast<void>(errorStatus);

			if(_running)
			{
				// keep blinking
				toggleLED();
				execWait();
			}
			else
			{
				// stop request signalled
				_led.turnOff();

				_moduleState = ModuleState::Ready;
				UARTprintf("[Blinker] stopped\n");
			}
		});
}

void
Blinker::toggleLED()
{
	if(_led.isTurnedOn())
	{
		_led.turnOff();
	}
	else
	{
		_led.turnOn();
	}
}

} // namespace blinker
} // namespace app
