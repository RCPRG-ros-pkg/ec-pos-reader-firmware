#pragma once

#include "util/EventLoop.hpp"
#include "dev/SysTickDevice.hpp"
#include "drv/SysTickMgr.hpp"
#include "dev/SSIMasterDevice.hpp"
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
	void readEncoder();

	util::EventLoop _eventLoop;
	dev::SysTickDevice _sysTickDevice;
	drv::SysTickMgr _sysTickMgr;
	drv::SysTickMgr::Timer _timer;
	dev::SSIMasterDevice _ssiMasterDevice;
	hohner::SMRS59 _smrs59;
};

} // namespace app
