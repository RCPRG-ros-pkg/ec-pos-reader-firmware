#pragma once

#include <chrono>
#include <utility>
#include <type_traits>

#include "embxx/device/context.h"
#include "embxx/error/ErrorCode.h"

namespace driver {

template<typename TEventLoop, typename TTimerDevice, typename TTimeoutHandler>
class DeadlineTimer
{
public:
	using EventLoop = TEventLoop;
	using TimerDevice = TTimerDevice;
	using TimeoutHandler = TTimeoutHandler;

	using ErrorCode = embxx::error::ErrorCode;

	static_assert(std::is_same_v<typename TimeoutHandler::result_type, void>,
		"Result type of callback must be 'void'");
	// static_assert(std::is_same_v<TimeoutHandler::argument_type, ErrorCode>,
		// "Argument type of callback must be 'ErrorCode'");

	/**
	 * @brief Constructor
	 * @details [long description]
	 *
	 * @param eventLoop [description]
	 * @param timerDevice [description]
	 * @param e [description]
	 */
	DeadlineTimer(EventLoop& eventLoop, TimerDevice& timerDevice)
		:	_eventLoop(eventLoop),
			_timerDevice(timerDevice)
	{
		_timerDevice.setTimeoutHandler(
			[this]()
			{
				timerTimeout(InterruptCtx());
			});

		// Postcondition: driver should not be busy
		assert(!isBusy());
	}

	/**
	 * @brief Starts asynchronous wait
	 * @details [long description]
	 *
	 * @param waitDuration [description]
	 * @param handler [description]
	 * @tparam TRep [description]
	 * @tparam TPeriod [description]
	 */
	template<typename TRep, typename TPeriod, typename THandler>
	void asyncWait(std::chrono::duration<TRep, TPeriod> waitDuration,
		THandler&& handler)
	{
		// No multiple waits possible
		assert(!_timerDevice.isBusy(EventLoopCtx()));

		// Store provided handler
		_timeoutHandler = std::forward<THandler>(handler);

		// Start wait from event loop context
		_timerDevice.startWait(waitDuration, EventLoopCtx());
	}

	/**
	 * @brief Cancels asynchronous wait
	 * @details [long description]
	 *
	 * @param cancelled [description]
	 * @return [description]
	 */
	bool cancel()
	{
		if(const auto cancelled = _timerDevice.cancelWait(EventLoopCtx()); cancelled)
		{
			// Wait has been cancelled. Post callback with `Aborted` status
			const auto postSuccess = _eventLoop.post(
				[this]()
				{
					// Timeout handler should be non-null, so invoke it
					assert(_timeoutHandler);
					_timeoutHandler(ErrorCode::Aborted);
				});
			assert(postSuccess);
			static_cast<void>(postSuccess);

			return true;
		}

		// Wait has been not cancelled
		return false;
	}

	/**
	 * @brief Checks, if driver is busy or not
	 * @details [long description]
	 *
	 * @param _timerDevice [description]
	 * @return [description]
	 */
	bool isBusy()
	{
		return _timerDevice.isBusy(EventLoopCtx());
	}

private:
	using EventLoopCtx = embxx::device::context::EventLoop;
	using InterruptCtx = embxx::device::context::Interrupt;

	/**
	 * @brief Timer device timeout handler. Should be invoked from interrupt
	 * @details [long description]
	 *
	 * @param  [description]
	 */
	void timerTimeout(InterruptCtx)
	{
		// Timer timeout ocurred. Post callback with 'Success' status
		const auto postSuccess = _eventLoop.postInterruptCtx(
			[this]()
			{
				// Timeout handler should be non-null, so invoke it
				assert(_timeoutHandler);
				_timeoutHandler(ErrorCode::Success);
			});
		assert(postSuccess);
		static_cast<void>(postSuccess);
	}

	TimeoutHandler _timeoutHandler;
	EventLoop& _eventLoop;
	TimerDevice& _timerDevice;
};

} // namespace driver
