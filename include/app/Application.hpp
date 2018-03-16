#pragma once

#include "util/EventLoop.hpp"
#include "dev/RTCDevice.hpp"
#include "dev/SysTickDevice.hpp"
#include "dev/SSIMasterDevice.hpp"
#include "drv/SysTickMgr.hpp"
#include "hohner/SMRS59.hpp"

namespace app {

//! Main application class
class Application
{
public:
	//! Constructor
	Application();

	//! Destructor
	~Application();

	//! Starts main loop of application
	void run();

private:
	void initializeABCC();

	void handleABCC();

	void resetABCC();

	void runABCCTimer();

	void runEncoderTimer();

	void readEncoder();

	util::EventLoop _eventLoop;
	dev::RTCDevice _rtcDevice;
	dev::SysTickDevice _sysTickDevice;
	drv::SysTickMgr _sysTickMgr;
	drv::SysTickMgr::Timer _abccTimer;
	drv::SysTickMgr::Timer _encoderTimer;
	dev::SSIMasterDevice _ssiMasterDevice;
	hohner::SMRS59 _smrs59;
};

} // namespace app
