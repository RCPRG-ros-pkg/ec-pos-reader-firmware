#pragma once

#include "device/SSIMasterDevice.hpp"
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
	device::SSIMasterDevice _ssiMasterDevice;
	hohner::SMRS59 _smrs59;
};

} // namespace app
