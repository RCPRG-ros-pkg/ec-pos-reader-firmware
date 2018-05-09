#pragma once

#include "app/common/EventLoop.hpp"

#include "app/blinker/Blinker.hpp"
#include "app/encoders/Encoder0.hpp"
#include "app/encoders/Encoder1.hpp"
#include "app/ethercat/EtherCAT.hpp"

#include "embxx/error/ErrorStatus.h"
#include "embxx/error/ErrorCode.h"

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
	// common stuff
	using ErrorStatus = embxx::error::ErrorStatus;
	using ErrorCode = embxx::error::ErrorCode;

	//! Starts modules
	void start();

	// commons
	common::EventLoop _eventLoop;

	// modules
	blinker::Blinker _blinker;
	encoders::Encoder0 _encoder0;
	encoders::Encoder1 _encoder1;
	ethercat::EtherCAT _etherCAT;
};

} // namespace app
