#pragma once

#include "app/common/EventLoop.hpp"

#include "embxx/util/StaticFunction.h"

#include "app/ethercat/Status.hpp"
#include "app/encoders/Encoders.hpp"

#include "app/ethercat/abcc_appl/appl_abcc_handler.h"

void triggerAdiSyncInputCapture();

namespace app {
namespace ethercat {

//! Anybus CompactCom EtherCAT handler module
class EtherCAT
{
public:
	EtherCAT(common::EventLoop& eventLoop,
		encoders::Encoders& encoders);

	~EtherCAT();

	void start();

private:
	friend void ::triggerAdiSyncInputCapture();

	void setupABCCHardware();

	APPL_AbccHandlerStatusType _abccHandlerStatus;

	common::EventLoop& _eventLoop;
	encoders::Encoders& _encoders;

	static EtherCAT* _instance;
};

} // namespace ethercat
} // namespace app
