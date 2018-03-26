#include "app/blinker/Blinker.hpp"

#include <cstdint>
#include "tivaware/utils/uartstdio.h"

namespace app {
namespace blinker {

Blinker::Blinker(common::SysTickDriver& sysTickDriver,
	device::GPIOF& gpioFDevice)
	:	_redLEDPin(gpioFDevice),
		_blueLEDPin(gpioFDevice),
		_greenLEDPin(gpioFDevice),
		_timer(sysTickDriver.allocTimer()),
		_redLED(_redLEDPin),
		_blueLED(_blueLEDPin),
		_greenLED(_greenLEDPin)
{
	assert(_timer.isValid());

	_redLEDPin.setAsDigitalOutput();
	_blueLEDPin.setAsDigitalOutput();
	_greenLEDPin.setAsDigitalOutput();

	UARTprintf("[Blinker] initialized\n");
}

void Blinker::signalInit()
{
	UARTprintf("[Blinker] signalling init\n");

	_blueLED.turnOn();
}

void Blinker::signalOperational()
{
	UARTprintf("[Blinker] signalling operational\n");

	assert(_blueLED.isTurnedOn());
	_blueLED.turnOff();

	toggleGreenLED();
}

void Blinker::signalError()
{
	UARTprintf("[Blinker] signalling error\n");

	_blueLED.turnOff();
	_greenLED.turnOff();
	_redLED.turnOn();
}

void Blinker::toggleGreenLED()
{
	if(_turnedOn)
	{
		_greenLED.turnOff();
		_turnedOn = false;
	}
	else
	{
		_greenLED.turnOn();
		_turnedOn = true;
	}

	_timer.asyncWait(std::chrono::milliseconds(500),
		[this](const auto& errorStatus)
		{
			const auto code = errorStatus.code();
			if(code != ErrorCode::Success)
			{
				if(code != ErrorCode::Aborted)
				{
					UARTprintf("[Blinker] error during wait, code: %d\n",
						static_cast<int>(code));
				}

				return;
			}

			toggleGreenLED();
		});
}

} // namespace blinker
} // namespace app
