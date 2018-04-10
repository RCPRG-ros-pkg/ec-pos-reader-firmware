#pragma once

#include "device/GPIO.hpp"
#include "component/LED.hpp"

#include "app/common/SysTickDriver.hpp"
#include "app/ModuleState.hpp"

namespace app {
namespace blinker {

class Blinker
{
public:
	//! Constructor
	Blinker(common::SysTickDriver& sysTickDriver,
		device::GPIOF& gpioFDevice);

	//! Starts module
	void start();

	//! Stops module
	void stop();

	//! Checks, if module is in active state
	bool isActive() const
	{
		return _moduleState == ModuleState::Active;
	}

	//! Checks, if module is in failed state
	bool isFailed() const
	{
		return _moduleState == ModuleState::Error;
	}

	//! Checks, if module is in ready state
	bool isReady() const
	{
		return _moduleState == ModuleState::Ready;
	}

	ModuleState getState() const
	{
		return _moduleState;
	}

private:
	// devices
	using LEDPin = device::GPIOF::Pin<2>;

	// drivers
	using Timer = common::SysTickDriver::Timer;

	// components
	using LED = component::LED<LEDPin, component::LogicDesign::ActiveHigh>;

	void execWait();

	void toggleLED();

	// devices
	LEDPin _ledPin;

	// drivers
	Timer _timer;

	// components
	LED _led;

	// others
	ModuleState _moduleState = ModuleState::Ready;
	bool _running = false;
};

} // namespace blinker
} // namespace app
