#pragma once

#include <chrono>
#include <utility>
#include <type_traits>
#include <exception>

#include "embxx/device/context.h"
#include "embxx/error/ErrorCode.h"

namespace driver {

template<typename TEventLoop, typename TTimerDevice, typename TCallback>
class DeadlineTimer
{
public:
	using EventLoop = TEventLoop;
	using TimerDevice = TTimerDevice;
	using Callback = TCallback;

	using ErrorCode = embxx::error::ErrorCode;

	static_assert(std::is_same_v<typename Callback::result_type, void>,
		"Result type of callback must be 'void'");
	// static_assert(std::is_same_v<Callback::argument_type, ErrorCode>,
		// "Argument type of callback must be 'ErrorCode'");

	DeadlineTimer(EventLoop& eventLoop, TimerDevice& timerDevice)
		:	_eventLoop(eventLoop),
			_timerDevice(timerDevice)
	{
		_timerDevice.setTimeoutCallback(
			[this]() { timerTimeout(InterruptCtx()); });
	}

	template<typename TRep, typename TPeriod, typename TFunc>
	void asyncWait(std::chrono::duration<TRep, TPeriod> waitDuration,
		TFunc&& func)
	{
		// No multiple waits possible
		assert(!_timerDevice.isWaiting(EventLoopCtx()));

		_callback = std::forward<TFunc>(func);
		_timerDevice.startWait(waitDuration, EventLoopCtx());
	}

	bool cancel()
	{
		if(const auto cancelled = _timerDevice.cancelWait(EventLoopCtx()); cancelled)
		{
			// Wait has been cancelled. Post callback with `Aborted` status
			const auto postSuccess = _eventLoop.post(
				[this]()
				{
					assert(_callback);
					_callback(ErrorCode::Aborted);
				});
			assert(postSuccess);
			static_cast<void>(postSuccess);

			return true;
		}

		// Wait has been not cancelled
		return false;
	}

	bool isWaiting()
	{
		return _timerDevice.isWaiting(EventLoopCtx());
	}

private:
	using EventLoopCtx = embxx::device::context::EventLoop;
	using InterruptCtx = embxx::device::context::Interrupt;

	void timerTimeout(InterruptCtx)
	{
		// Timer timeout ocurred. Post callback with 'Success' status
		const auto postSuccess = _eventLoop.postInterruptCtx(
			[this]()
			{
				assert(_callback);
				_callback(ErrorCode::Success);
			});
		assert(postSuccess);
		static_cast<void>(postSuccess);
	}

	Callback _callback;
	EventLoop& _eventLoop;
	TimerDevice& _timerDevice;
};

} // namespace driver
