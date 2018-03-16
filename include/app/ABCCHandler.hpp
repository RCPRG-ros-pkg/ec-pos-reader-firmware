#pragma once

#include <cstdint>

#include "embxx/device/context.h"

namespace app {

class ABCCHandler
{
public:
	//! Constructor
	ABCCHandler();

	//! Destructor
	~ABCCHandler();

	std::uint16_t doADIMapping();

	void doUserInit();

	void updateWriteProcessData();

	void handleReadProcessData();

	void messageReceived();

	void watchdogTimeout();

	void watchdogTimeoutRecovered();

	void remapDone();

	void anybusStateChanged();

	void handleEvent();

	void handleSyncISR();

private:
	static ABCCHandler* _instance;
};

} // namespace app
