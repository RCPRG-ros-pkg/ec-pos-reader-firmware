#pragma once

#include "device/GPIO.hpp"
#include "device/GPTM.hpp"

#include "driver/DeadlineTimer.hpp"

#include "component/LED.hpp"

#include "app/common/EventLoop.hpp"
#include "app/ModuleState.hpp"

namespace app {
namespace blinker {

class Blinker
{
public:
	using EventLoop = common::EventLoop;
	using LEDPinGPIO = device::GPIOF;

	//! Constructor
	Blinker(EventLoop& eventLoop, LEDPinGPIO& ledPinGPIO);

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
	using DeadlineTimerDevice = device::GPTM0;

	// drivers
	using DeadlineTimer = driver::DeadlineTimer<DeadlineTimerDevice,
		EventLoop, embxx::util::StaticFunction<void()>>;

	// components
	using LED = component::LED<LEDPin>;

	void execWait();

	void toggleLED();

	// devices
	LEDPin _ledPin;
	DeadlineTimerDevice _deadlineTimerDevice;

	// drivers
	DeadlineTimer _deadlineTimer;

	// components
	LED _led;

	// others
	ModuleState _moduleState = ModuleState::Ready;
	bool _running = false;
};

} // namespace blinker
} // namespace app
