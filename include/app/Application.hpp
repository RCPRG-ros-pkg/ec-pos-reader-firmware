#pragma once

#include "app/common/EventLoop.hpp"
#include "app/common/SysTickDevice.hpp"
#include "app/common/SysTickDriver.hpp"

#include "device/GPIO.hpp"

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
	using ErrorStatus = embxx::error::ErrorStatus;
	using ErrorCode = ErrorStatus::ErrorCodeType;

	enum class State
	{
		Init,
		PreOperational,
		Operational,
		Error
	};

	//! Constructor
	Application();

	//! Destructor
	~Application();

	//! Does work for initialization state
	void runInit();

	//! Does work for pre-operational state
	void runPreOperational();

	//! Does work for operational state
	void runOperational();

	//! Does work for error state
	void runError();

	//! Executes eventLoop.run() until it is stopped by _eventLoop.stop()
	void handleEvents();

	// common
	common::EventLoop _eventLoop;

	// devices
	common::SysTickDevice _sysTickDevice;
	device::GPIOF _gpioFDevice;

	// drivers
	common::SysTickDriver _sysTickDriver;

	// modules
	blinker::Blinker _blinker;
	encoders::Encoders _encoders;
	ethercat::EtherCAT _etherCAT;

	State _state = State::Init;
};

} // namespace app
