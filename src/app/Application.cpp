#include "app/Application.hpp"

#include "tivaware/driverlib/interrupt.h"
#include "tivaware/utils/uartstdio.h"

namespace app {

/**
 * @brief Constructor
 * @details
 */
Application::Application()
	:	_sysTickDriver(_sysTickDevice, _eventLoop),
		_blinker(_sysTickDriver, _gpioFDevice),
		_encoders(_sysTickDriver),
		_etherCAT(_eventLoop, _sysTickDriver)
{
	UARTprintf("[Application] preinitialized\n");

	assert(_state == State::Init);
}

/**
 * @brief Destructor
 * @details Shutdowns ABCC module
 */
Application::~Application()
{

}

/**
 * @brief Starts main loop of application
 * @details
 */
void
Application::run()
{
	assert(_state == State::Init);
	UARTprintf("[Application] starting...\n");

	while(1)
	{
		switch(_state)
		{
			case State::Init:
				runInit();
				break;

			case State::Operational:
				runOperational();
				break;

			default:
				assert(_state == State::Error);
				runError();
				break;
		}
	}

	UARTprintf("[Application] exits\n");
}

void
Application::runInit()
{
	assert(_state == State::Init);

	UARTprintf("[Application] entering INIT state...\n");

	_blinker.signalInit();
	_etherCAT.initialize(
		[this](auto status)
		{
			if(status != ethercat::Status::Success)
			{
				UARTprintf("[Application] INIT error : %d\n",
					static_cast<int>(status));
				_state = State::Error;
			}
			else
			{
				UARTprintf("[Application] INIT success\n");
				_state = State::Operational;
			}

			_eventLoop.stop();
		});

	handleEvents();
	assert(_state == State::Operational || _state == State::Error);

	UARTprintf("[Application] INIT state ends\n");
}

void
Application::runOperational()
{
	assert(_state == State::Operational);

	UARTprintf("[Application] entering OPERATIONAL state...\n");

	_blinker.signalOperational();
	_encoders.startReading();
	_etherCAT.start(
		[this](auto status)
		{
			UARTprintf("[Application] EtherCAT stopped, code: %d\n",
				static_cast<int>(status));
		});

	handleEvents();
	assert(!"Should not get here!");

	UARTprintf("[Application] OPERATIONAL state ends\n");
}

void
Application::runError()
{
	assert(_state == State::Error);

	UARTprintf("[Application] entering ERROR state\n");

	_blinker.signalError();

	while(1);
	assert(!"Should not get here!");

	UARTprintf("[Application] exiting ERROR state\n");
}

void Application::handleEvents()
{
	// enter event handling loop. It exits after call to EventLoop::stop()
	IntMasterEnable();
	_eventLoop.run();
	IntMasterDisable();

	// Restore state of EventLoop, because stop() was called
	_eventLoop.reset();
}

// void
// Application::handleABCC()
// {
// 	const auto abccHandlerStatus = APPL_HandleAbcc();
// 	switch(abccHandlerStatus)
// 	{
// 	case APPL_MODULE_RESET:
// 		resetABCC();
// 		break;
// 	default:
// 		break;
// 	}
// }

// void
// Application::resetABCC()
// {
// 	UARTprintf("resetABCC!\n");
// 	while(1);
// }

// void
// Application::runABCCTimer()
// {
// 	assert(_abccTimer.isValid());
// 	_abccTimer.asyncWait(ABCCTimerDelay,
// 		[this](const auto errorStatus)
// 		{
// 			if(errorStatus)
// 			{
// 				UARTprintf("Error occured during wait, code=%d\n",
// 					static_cast<int>(errorStatus.code()));
// 				return;
// 			}

// 			runABCCTimer();
// 			ABCC_RunTimerSystem(ABCCTimerDelay.count());
// 		});
// }

} // namespace app


// namespace {

// constexpr auto ABCCTimerDelay = std::chrono::milliseconds(1);

// hohner::SMRS59* smrs59 = nullptr;
// _ssiMasterDevice(SSI0_BASE, SSIMaster0BitRate, SSIMaster0DataWidth),

		// _abccTimer(_sysTickDriver.allocTimer()),
		// _encoderTimer(_sysTickDriver.allocTimer()),
		// _smrs59(_ssiMasterDevice)
	// assert(_abccTimer.isValid());
	// assert(_encoderTimer.isValid());
	// initializeABCC();

	// smrs59 = &_smrs59;
		// runEncoderTimer();
	// runABCCTimer();
		// handleABCC();

// } // namespace
