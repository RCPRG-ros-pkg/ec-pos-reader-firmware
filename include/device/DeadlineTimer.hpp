#pragma once

#include <cstdint>
#include <chrono>
#include <cassert>

#include "tivaware/inc/hw_ints.h"

#include "embxx/util/StaticFunction.h"
#include "embxx/device/context.h"

#include "util/driverlib/timer.hpp"
#include "util/driverlib/interrupt.hpp"

#include "init.hpp"

#include "device/Peripheral.hpp"

namespace device {

namespace {

} // namespace

template<std::uint32_t TBaseAddress, std::uint32_t TId, std::uint32_t TIntNumber>
class DeadlineTimer
	:	public Peripheral<TId>
{
public:
	constexpr static std::uint32_t BaseAddress = TBaseAddress;
	static_assert(BaseAddress != 0,
		"Specified BaseAddress is invalid");

	constexpr static std::uint32_t IntNumber = TIntNumber;
	static_assert(IntNumber < NUM_INTERRUPTS,
		"Specified IntNumber is invalid");

	constexpr static int Frequency = ClockHz;

	using WaitRep = std::uint32_t;
	using WaitRatio = std::ratio<1, Frequency>;
	using WaitDuration = std::chrono::duration<WaitRep, WaitRatio>;

	using EventLoopCtx = embxx::device::context::EventLoop;
	using InterruptCtx = embxx::device::context::Interrupt;

	/**
	 * @brief Constructor
	 * @details [long description]
	 */
	DeadlineTimer()
	{
		// Interrupts should be locked
		assert(IntGeneralEnabledGet(IntNumber) == false);

		// Be sure, that during construction Timer is disabled
		assert(!TimerIsEnabled(BaseAddress, TIMER_BOTH));

		// Be sure, that during construction Timer interrupts are disabled
		assert(TimerIntEnabledGet(BaseAddress) == 0);

		// Configure timer to work as full-width, one shot
		MAP_TimerConfigure(BaseAddress, TIMER_CFG_ONE_SHOT);

		// Register interrupt handler and enable it
		IntRegister(IntNumber, timerISR);
		IntUserDataSet(IntNumber, static_cast<void*>(this));

		// After construction, timer should be idle
		assert(!isBusy(InterruptCtx()));

		// Unlock interrupts
		IntGeneralEnable(IntNumber);
	}

	/**
	 * @brief Destructor
	 * @details [long description]
	 */
	~DeadlineTimer()
	{
		// Lock interrupts
		IntGeneralDisable(IntNumber);

		// During destruction, device should be idle
		assert(!isBusy(InterruptCtx()));

		// Disable interrupt handler and unregister it
		IntUserDataUnset(IntNumber);
		IntUnregister(IntNumber);
	}

	/**
	 * @brief Sets timeout handler used in asynchronous operations
	 * @details [long description]
	 *
	 * @param handler [description]
	 */
	template<typename THandler>
	void setTimeoutHandler(THandler&& handler)
	{
		// Lock interrupts and check, that device should not be busy
		assert(!isBusy(EventLoopCtx()));

		// Store provided handler
		_timeoutHandler = std::forward<THandler>(handler);
	}

	/**
	 * @brief Starts asynchronous wait from event loop context
	 * @details [long description]
	 *
	 * @param duration [description]
	 * @param x [description]
	 * @tparam TRep [description]
	 * @tparam TPeriod [description]
	 */
	template<typename TRep, typename TPeriod>
	void startWait(const std::chrono::duration<TRep, TPeriod>& duration,
		EventLoopCtx)
	{
		// Lock interrupts and check, that device should not be busy
		assert(!isBusy(EventLoopCtx()));

		// Start wait as in the interrupt context
		startWait(duration, InterruptCtx());
	}

	/**
	 * @brief Checks, whether device is busy or not, from event loop context
	 * @details [long description]
	 *
	 * @param  [description]
	 * @return [description]
	 */
	bool isBusy(EventLoopCtx)
	{
		// Lock interrupts
		IntGeneralDisable(IntNumber);

		// Check status as in the interrupt context
		const auto busy = isBusy(InterruptCtx());

		// Unlock interrupts
		IntGeneralEnable(IntNumber);

		return busy;
	}

	/**
	 * @brief Cancels asynchronous wait, from event loop context
	 * @details [long description]
	 *
	 * @param  [description]
	 * @return [description]
	 */
	bool cancelWait(EventLoopCtx)
	{
		// Lock interrupts
		IntGeneralDisable(IntNumber);

		// Cancel wait as in the interrupt context
		const auto waitCancelled = cancelWait(InterruptCtx());

		// Unlock interrupts
		IntGeneralEnable(IntNumber);

		return waitCancelled;
	}

private:
	template<typename T> using Function = embxx::util::StaticFunction<T, 2 * sizeof(void*)>;

	using TimeoutHandler = Function<void()>;

	/**
	 * @brief Starts asynchronous wait, from interrupt context
	 * @details [long description]
	 *
	 * @param duration [description]
	 * @param x [description]
	 * @tparam TRep [description]
	 * @tparam TPeriod [description]
	 */
	template<typename TRep, typename TPeriod>
	void startWait(const std::chrono::duration<TRep, TPeriod>& duration,
		InterruptCtx)
	{
		// Lock interrupts and check, that device should not be busy
		assert(!isBusy(InterruptCtx()));

		// Calculate load for the timer
		const auto waitDuration =
			std::chrono::duration_cast<WaitDuration>(duration);
		TimerLoadSet(BaseAddress, TIMER_A, waitDuration.count());

		// Assume, that there are no pending interrupts and enable them
		assert(TimerRawIntStatus(BaseAddress) == 0);
		assert(TimerIntEnabledGet(BaseAddress) == 0);
		TimerIntEnable(BaseAddress, TIMER_TIMA_TIMEOUT);

		// Timer should be now disabled, so enable it
		assert(!TimerIsEnabled(BaseAddress, TIMER_BOTH));
		TimerEnable(BaseAddress, TIMER_BOTH);
	}

	bool cancelWait(InterruptCtx)
	{
		if(isBusy(InterruptCtx()))
		{
			// Timer was waiting, so disable it, and just to be sure clear interrupts,
			//  because timeout could occur right after call to `TimerIsEnabled`
			TimerDisable(BaseAddress, TIMER_A);
			TimerIntDisable(BaseAddress, TIMER_TIMA_TIMEOUT);
			TimerIntClear(BaseAddress, TIMER_TIMA_TIMEOUT);

			// We have successfully cancelled wait
			return true;
		}

		// Neither timer was not enabled nor there was not any pending interrupts,
		//  so callback was invoked earlier, and callback was arleady called.

		// We have not cancelled the wait
		return false;
	}

	bool isBusy(InterruptCtx)
	{
		if(TimerIsEnabled(BaseAddress, TIMER_A))
		{
			// Right after disabling interrupts, timer was enabled
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
		// Device should be busy since interrupt was fired
		assert(isBusy(InterruptCtx()));

		// Check, that valid interrupt occured, and clear it
		assert(TimerMaskedIntStatus(BaseAddress) == TIMER_TIMA_TIMEOUT);
		TimerIntClear(BaseAddress, TIMER_TIMA_TIMEOUT);

		// Since this timer was in one-shot mode, it should be now disabled,
		//  so disable only interrupts
		assert(!TimerIsEnabled(BaseAddress, TIMER_A));
		TimerIntDisable(BaseAddress, TIMER_TIMA_TIMEOUT);

		// Wait complete, invoke the callback
		assert(_timeoutHandler);
		_timeoutHandler();
	}

	static void timerISR()
	{
		// Retrieve stored UserData pointer (pointing to this object),
		// 	and cast it to this object type
		using ThisType = DeadlineTimer<BaseAddress, TId, IntNumber>;
		auto userData = IntUserDataGet(IntNumber);
		auto instance = static_cast<ThisType*>(userData);

		// Obtained pointer should be non-null, so invoke ISR handler
		assert(instance != nullptr);
		instance->handleISR(InterruptCtx());
	}

	// Private members
	TimeoutHandler _timeoutHandler; //< Timeout handler for async operations
};

} // namespace device
