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

	// devices
	using RedLEDPin = device::GPIOF::Pin<1>;
	using GreenLEDPin = device::GPIOF::Pin<3>;

	// components
	using GreenLED = component::LED<GreenLEDPin, component::LogicDesign::ActiveHigh>;
	using RedLED = component::LED<RedLEDPin, component::LogicDesign::ActiveHigh>;

	//! Constructor
	Application();

	//! Destructor
	~Application();

	//! Starts modules
	void start();

	//! Stops modules
	void stop();

	//! Halts whole application
	void halt();

	// commons
	common::EventLoop _eventLoop;

	// devices
	common::SysTickDevice _sysTickDevice;
	device::GPIOF _gpiofDevice;
	RedLEDPin _redLEDPin;
	GreenLEDPin _greenLEDPin;

	// drivers
	common::SysTickDriver _sysTickDriver;

	// components
	RedLED _redLED;
	GreenLED _greenLED;

	// modules
	blinker::Blinker _blinker;
	encoders::Encoders _encoders;
	ethercat::EtherCAT _etherCAT;
};

} // namespace app
