#pragma once

#include <chrono>
#include <utility>

namespace driver {

template<typename TTimerDevice, typename TEventLoop, typename TTimeoutHandler>
class DeadlineTimer
{
public:
	using TimerDevice = TTimerDevice;
	using EventLoop = TEventLoop;
	using TimeoutHandler = TTimeoutHandler;

	DeadlineTimer(EventLoop& eventLoop, TimerDevice& timerDevice)
		:	_timerDevice(timerDevice),
			_eventLoop(eventLoop)
	{

	}

	template<typename TRep, typename TPeriod, typename TFunc>
	void asyncWait(std::chrono::duration<TRep, TPeriod> waitDuration,
		TFunc&& func)
	{
		_timerDevice.setCallback(
			[this, func = std::forward<TFunc>(func)]()
			{
				_eventLoop.post(func);
			});
		_timerDevice.startWait(waitDuration);
	}

private:
	TimerDevice& _timerDevice;
	EventLoop& _eventLoop;
};

} // namespace driver
