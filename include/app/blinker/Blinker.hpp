#pragma once

#include "device/GPIO.hpp"
#include "component/LED.hpp"

#include "app/common/SysTickDriver.hpp"

namespace app {
namespace blinker {

class Blinker
{
public:
	Blinker(common::SysTickDriver& sysTickDriver,
		device::GPIOF& gpioFDevice);

	void signalInit();

	void signalOperational();

	void signalError();

private:
	using ErrorStatus = embxx::error::ErrorStatus;
	using ErrorCode = ErrorStatus::ErrorCodeType;

	// devices
	using RedLEDPin = device::GPIOF::Pin<1>;
	using BlueLEDPin = device::GPIOF::Pin<2>;
	using GreenLEDPin = device::GPIOF::Pin<3>;

	// drivers
	using Timer = common::SysTickDriver::Timer;

	// components
	using RedLED = component::LED<RedLEDPin, component::LogicDesign::ActiveHigh>;
	using BlueLED = component::LED<BlueLEDPin, component::LogicDesign::ActiveHigh>;
	using GreenLED = component::LED<GreenLEDPin, component::LogicDesign::ActiveHigh>;

	void toggleGreenLED();

	// devices
	RedLEDPin _redLEDPin;
	BlueLEDPin _blueLEDPin;
	GreenLEDPin _greenLEDPin;

	// drivers
	Timer _timer;

	// components
	RedLED _redLED;
	BlueLED _blueLED;
	GreenLED _greenLED;

	bool _turnedOn = false;
};

} // namespace blinker
} // namespace app
