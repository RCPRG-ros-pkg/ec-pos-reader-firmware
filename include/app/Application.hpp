#pragma once

#include "app/common/EventLoop.hpp"
#include "app/common/SysTickDevice.hpp"
#include "app/common/SysTickDriver.hpp"

#include "device/GPIO.hpp"

#include "component/LED.hpp"

#include "app/blinker/Blinker.hpp"
#include "app/encoders/Encoders.hpp"
#include "app/ethercat/EtherCAT.hpp"

#include "embxx/error/ErrorStatus.h"

namespace app {

//! Main application class
class Application
{
public:
	static Application& instance()
	{
		static Application instance;
		return instance;
	}

	//! Starts main loop of application
	void run();

private:
	// common stuff
	using ErrorStatus = embxx::error::ErrorStatus;
	using ErrorCode = ErrorStatus::ErrorCodeType;

	//! Constructor
	Application();

	//! Destructor
	~Application();

	//! Starts modules
	void startModules();

	// commons
	common::EventLoop _eventLoop;

	// devices
	device::GPIOF _gpiofDevice;

	// drivers

	// modules
	blinker::Blinker _blinker;
	encoders::Encoders _encoders;
	ethercat::EtherCAT _etherCAT;
};

} // namespace app
