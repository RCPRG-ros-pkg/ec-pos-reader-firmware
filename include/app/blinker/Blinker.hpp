#pragma once

#include "device/OutputPin.hpp"
#include "device/DeadlineTimer.hpp"

#include "driver/DeadlineTimer.hpp"

#include "component/LED.hpp"

#include "app/common/EventLoop.hpp"

namespace app {
namespace blinker {

class Blinker
{
public:
	using EventLoop = common::EventLoop;

	using LEDPin = device::OutputPin<GPIO_PORTF_BASE, 2>;

	//! Constructor
	Blinker(EventLoop& eventLoop);

	//! Destructor
	~Blinker();

	//! Starts module
	void start();

	//! Stops module
	void stop();

	//! Checks, if module is in active state
	bool isActive() const;

	//! Checks, if module is in failed state
	bool isFailed() const;

	//! Checks, if module is in ready state
	bool isReady() const;

private:
	using ErrorCode = embxx::error::ErrorCode;

	template<typename T, std::size_t TSize>
	using Function = embxx::util::StaticFunction<T, TSize>;

	using TimerDevice = device::DeadlineTimer<TIMER0_BASE>;
	using TimerCallback = Function<void(ErrorCode), 1 * sizeof(void*)>;
	using DeadlineTimer =
		driver::DeadlineTimer<EventLoop, TimerDevice, TimerCallback>;

	using LED = component::LED<LEDPin>;

	//! Time duration between LED on/off switches
	constexpr static auto WaitDuration = std::chrono::seconds(1);

	//! Represents internal state of the module
	enum class State
	{
		Idle, //< Module is not blinking
		Active, //< Module is blinking
		Stopping //< Module is going to stop blinking
	};

	//! Executes asynchronous wait and then toggles the LED
	void doWait();

	//! Toggles the LED
	void toggleLED();

	TimerDevice _timerDevice;
	DeadlineTimer _deadlineTimer;

	LEDPin _ledPin;
	LED _led;

	EventLoop& _eventLoop;
	State _state = State::Idle;
};

} // namespace blinker
} // namespace app
