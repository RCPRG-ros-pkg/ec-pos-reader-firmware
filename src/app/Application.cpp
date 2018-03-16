#include "app/Application.hpp"

#include "tivaware/inc/hw_memmap.h"
#include "tivaware/utils/uartstdio.h"

#include "abcc_td.h"
#include "abcc.h"
#include "abcc_sys_adapt.h"
#include "ad_obj.h"
#include "appl_abcc_handler.h"

namespace {

constexpr auto SSIMaster0BitRate = 1250000;
constexpr auto SSIMaster0DataWidth = 13;
constexpr auto ABCCTimerDelay = std::chrono::milliseconds(1);

} // namespace

namespace app {

/**
 * @brief Constructor
 * @details Initializes ABCC module
 */
Application::Application()
	:	_rtcDevice(),
		_sysTickMgr(_sysTickDevice, _eventLoop),
		_abccTimer(_sysTickMgr.allocTimer()),
		_encoderTimer(_sysTickMgr.allocTimer()),
		_ssiMasterDevice(SSI0_BASE, SSIMaster0BitRate, SSIMaster0DataWidth),
		_smrs59(_ssiMasterDevice)
{
	assert(_abccTimer.isValid());
	assert(_encoderTimer.isValid());

	// initializeABCC();

	UARTprintf("Application initialized\n");
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
	// runEncoderTimer();
	// runABCCTimer();

	IntMasterEnable();
	while(1)
	{
		_eventLoop.runOnce();

		using seconds = std::chrono::seconds;
		using subseconds = std::chrono::duration<float, std::ratio<1, 32768>>;
		using nanoseconds = std::chrono::nanoseconds;

		const auto secondsNow = seconds(_rtcDevice.getSeconds());
		const auto subsecondsNow = subseconds(_rtcDevice.getSubSeconds());
		const auto nanosecondsNow = std::chrono::duration_cast<nanoseconds>(subsecondsNow);
		// const auto total = secondsNow + subsecondsNow;
		// const float subSecondsScaled = subSecondsInv * 1000000000;

		UARTprintf("nanoseconds: %d\n", nanosecondsNow.count());

		// handleABCC();
	}
}

void
Application::initializeABCC()
{
	UARTprintf("Initializing ABCC hardware...\n");
	if(ABCC_HwInit() != ABCC_EC_NO_ERROR)
	{
		UARTprintf("Error during ABCC_HwInit.\n");
		while(1);
	}

	UARTprintf("ABCC initialized\n");
}

void
Application::handleABCC()
{
	const auto abccHandlerStatus = APPL_HandleAbcc();
	switch(abccHandlerStatus)
	{
	case APPL_MODULE_RESET:
		resetABCC();
		break;
	default:
		break;
	}
}

void
Application::resetABCC()
{
	UARTprintf("resetABCC!\n");
	while(1);
}

void
Application::runABCCTimer()
{
	assert(_abccTimer.isValid());
	_abccTimer.asyncWait(ABCCTimerDelay,
		[this](const auto errorStatus)
		{
			if(errorStatus)
			{
				UARTprintf("Error occured during wait, code=%d\n",
					static_cast<int>(errorStatus.code()));
				return;
			}

			runABCCTimer();
			ABCC_RunTimerSystem(ABCCTimerDelay.count());
		});
}

void
Application::runEncoderTimer()
{
	assert(_encoderTimer.isValid());
	_encoderTimer.asyncWait(std::chrono::milliseconds(100),
		[this](const auto errorStatus)
		{
			if(errorStatus)
			{
				UARTprintf("Error occured during wait. code=%d\n",
					static_cast<int>(errorStatus.code()));
				return;
			}

			runEncoderTimer();
			readEncoder();
		});
}

void
Application::readEncoder()
{
	const auto position = _smrs59.readPosition();
	UARTprintf("Encoder position: %d\n", position.value());
}

} // namespace app
