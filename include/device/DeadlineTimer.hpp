#pragma once

#include <cstdint>
#include <chrono>
#include <cassert>

#include "tivaware/inc/hw_ints.h"

#include "util/driverlib/timer.hpp"
#include "util/driverlib/interrupt.hpp"

#include "init.hpp"

#include "embxx/util/StaticFunction.h"
#include "embxx/device/context.h"

namespace device {

namespace {

} // namespace

template<std::uint32_t TBaseAddress, std::uint32_t TIntNumber>
class DeadlineTimer
{
public:
	constexpr static std::uint32_t BaseAddress = TBaseAddress;
	static_assert(BaseAddress != 0);

	constexpr static std::uint32_t IntNumber = TIntNumber;
	static_assert(IntNumber < NUM_INTERRUPTS);

	constexpr static int Frequency = ClockHz;

	using WaitRep = std::uint32_t;
	using WaitRatio = std::ratio<1, Frequency>;
	using WaitDuration = std::chrono::duration<WaitRep, WaitRatio>;

	using EventLoopCtx = embxx::device::context::EventLoop;
	using InterruptCtx = embxx::device::context::Interrupt;

	DeadlineTimer()
	{
		// Configure timer to work as full-width, one shot
		MAP_TimerConfigure(BaseAddress, TIMER_CFG_ONE_SHOT);

		// Register interrupt handler and enable it
		IntRegister(IntNumber, timerISR);
		IntUserDataSet(IntNumber, static_cast<void*>(this));
		IntEnable(IntNumber);

		// After construction, timer should be idle
		assert(!checkWaiting(EventLoopCtx()));
	}

	~DeadlineTimer()
	{
		// Lock guard
		TimerIntDisable(BaseAddress, TIMER_TIMA_TIMEOUT);

		// During destruction, timer should be idle
		assert(!checkWaiting(EventLoopCtx()));

		// Disable interrupt handler and unregister it
		IntDisable(IntNumber);
		IntUserDataUnset(IntNumber);
		IntUnregister(IntNumber);
	}

	template<typename TFunc>
	void setTimeoutCallback(TFunc&& func)
	{
		// During setting the timeout callback, timer should be idle
		assert(!checkWaiting(EventLoopCtx()));

		_timeoutCallback = std::forward<TFunc>(func);
	}

	template<typename TRep, typename TPeriod>
	void startWait(const std::chrono::duration<TRep, TPeriod>& duration,
		EventLoopCtx)
	{
		// Lock guard
		TimerIntDisable(BaseAddress, TIMER_TIMA_TIMEOUT);

		// Start wait as in the interrupt context
		startWait(duration, InterruptCtx());
	}

	bool isWaiting(EventLoopCtx)
	{
		// Lock guard
		TimerIntDisable(BaseAddress, TIMER_TIMA_TIMEOUT);

		// Check status as in the interrupt context
		const auto waiting = checkWaiting(InterruptCtx());

		// If it is waiting, reenable interrupts
		if(waiting)
		{
			TimerIntEnable(BaseAddress, TIMER_TIMA_TIMEOUT);
		}

		return waiting;
	}

	bool cancelWait(EventLoopCtx)
	{
		// Lock guard
		TimerIntDisable(BaseAddress, TIMER_TIMA_TIMEOUT);

		// Cancel wait as in the interrupt context
		const auto waitCancelled = cancelWait(InterruptCtx());
		return waitCancelled;
	}

private:
	template<typename T> using Function = embxx::util::StaticFunction<T, 2 * sizeof(void*)>;

	using TimeoutCallback = Function<void()>;

	template<typename TRep, typename TPeriod>
	void startWait(const std::chrono::duration<TRep, TPeriod>& duration,
		InterruptCtx)
	{
		// No multiple waits possible
		assert(!checkWaiting(InterruptCtx()));

		// Calculate load for the timer
		const auto waitDuration =
			std::chrono::duration_cast<WaitDuration>(duration);
		TimerLoadSet(BaseAddress, TIMER_A, waitDuration.count());

		// Assume, that there are no pending interrupts
		assert(TimerRawIntStatus(BaseAddress) == 0);

		// Enable timer interrupts, and then launch timer
		TimerIntEnable(BaseAddress, TIMER_TIMA_TIMEOUT);
		TimerEnable(BaseAddress, TIMER_BOTH);
	}

	bool cancelWait(InterruptCtx)
	{
		if(checkWaiting(InterruptCtx()))
		{
			// Timer was waiting, so disable it, and just to be sure clear interrupts,
			//  because timeout could occur right after call to `TimerIsEnabled`
			TimerDisable(BaseAddress, TIMER_A);
			TimerIntClear(BaseAddress, TIMER_TIMA_TIMEOUT);

			// We have successfully cancelled wait
			return true;
		}

		// Neither timer was not enabled nor there was not any pending interrupts,
		//  so callback was invoked earlier, and callback was arleady called.

		// We have not cancelled the wait
		return false;
	}

	bool checkWaiting(EventLoopCtx)
	{
		// Lock guard
		TimerIntDisable(BaseAddress, TIMER_TIMA_TIMEOUT);

		// Check, if it was waiting, like in the interrupt context
		const auto waiting = checkWaiting(InterruptCtx());

		return waiting;
	}

	bool checkWaiting(InterruptCtx)
	{
		if(TimerIsEnabled(BaseAddress, TIMER_A))
		{
			// Right after disabling interrupts, timer was enabled,
			//  so callback was not called yet => timer is waiting
			return true;
		}
		else if(const auto rawIntStatus = TimerRawIntStatus(BaseAddress); rawIntStatus)
		{
			// Maybe timer was not enabled right after disabling interrupts,
			//  but there is non-zero raw int status,
			//  so callback was not called yet => timer is waiting
			assert(rawIntStatus == TIMER_TIMA_TIMEOUT);
			return true;
		}

		// Neither timer was not enabled, nor there was not any pending interrupts,
		//  so callback was invoked earlier => timer is not waiting
		return false;
	}

	void handleISR(InterruptCtx)
	{
		// Check, that valid interrupt occured, and clear it
		assert(TimerMaskedIntStatus(BaseAddress) == TIMER_TIMA_TIMEOUT);
		TimerIntClear(BaseAddress, TIMER_TIMA_TIMEOUT);

		// Wait complete, invoke the callback
		assert(_timeoutCallback);
		_timeoutCallback();

		// Since this timer was in one-shot mode, it should be now disabled
		assert(!TimerIsEnabled(BaseAddress, TIMER_A));
	}

	static void timerISR()
	{
		using ThisType = DeadlineTimer<BaseAddress, IntNumber>;

		auto userData = IntUserDataGet(IntNumber);
		auto instance = static_cast<ThisType*>(userData);

		assert(instance != nullptr);
		instance->handleISR(InterruptCtx());
	}

	TimeoutCallback _timeoutCallback;
};

} // namespace device
