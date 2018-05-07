#include "app/blinker/Blinker.hpp"

#include <cstdint>
#include "tivaware/utils/uartstdio.h"

namespace app {
namespace blinker {

Blinker::Blinker(EventLoop& eventLoop)
	:	_timerDevice(),
		_deadlineTimer(eventLoop, _timerDevice),
		_ledPin(),
		_led(_ledPin),
		_eventLoop(eventLoop)
{
	UARTprintf("[Blinker] ready\n");

	assert(!_deadlineTimer.isBusy());
	assert(_led.isTurnedOff());
	assert(_state == State::Idle);
}

Blinker::~Blinker()
{
	assert(_state == State::Idle);
	assert(_led.isTurnedOff());
	assert(!_deadlineTimer.isBusy());
}

void
Blinker::start()
{
	assert(_state == State::Idle);
	UARTprintf("[Blinker] starting...\n");

	_state = State::Active;
	doWait();

	UARTprintf("[Blinker] started\n");
}

void
Blinker::stop()
{
	assert(_state == State::Active);
	UARTprintf("[Blinker] stopping...\n");

	_deadlineTimer.cancel();
	_state = State::Stopping; // signal stop request
}

void
Blinker::doWait()
{
	assert(_state == State::Active);
	_deadlineTimer.asyncWait(WaitDuration,
		[this](ErrorCode errorCode)
		{
			static_cast<void>(errorCode);

			if(_state == State::Stopping)
			{
				// ErrorCode=Success may occur, when wait was not cancelled
				//  (cancel occur after posting to EventLoop).
				// ErrorCode=Aborted may occur, when wait was cancelled.
				assert(errorCode == ErrorCode::Success
					|| errorCode == ErrorCode::Aborted);

				// We are going to stop now.
				// Turn off LED, stop blinking and go to IDLE state.
				_led.turnOff();
				_state = State::Idle;
				UARTprintf("[Blinker] stopped\n");

				return;
			}

			// Module is active => keep blinking
			assert(_state == State::Active);
			assert(errorCode == ErrorCode::Success);

			toggleLED();
			doWait();
		});
}

void
Blinker::toggleLED()
{
	assert(_state == State::Active);

	if(_led.isTurnedOn())
	{
		_led.turnOff();
	}
	else
	{
		_led.turnOn();
	}
}

//! Checks, if module is in active state
// bool
// Blinker::isActive() const
// {
// 	return _moduleState == ModuleState::Active;
// }

// //! Checks, if module is in failed state
// bool
// Blinker::isFailed() const
// {
// 	return _moduleState == ModuleState::Error;
// }

// //! Checks, if module is in ready state
// bool
// Blinker::isReady() const
// {
// 	return _moduleState == ModuleState::Ready;
// }

// ModuleState
// Blinker::getState() const
// {
// 	return _moduleState;
// }

} // namespace blinker
} // namespace app
