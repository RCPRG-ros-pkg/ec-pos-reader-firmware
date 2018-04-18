#pragma once

#include "app/common/EventLoop.hpp"

#include "embxx/util/StaticFunction.h"
#include "embxx/error/ErrorCode.h"

#include "app/ethercat/Status.hpp"
#include "app/encoders/Encoders.hpp"

#include "app/ethercat/abcc_appl/appl_abcc_handler.h"

extern "C" void APPL_SyncIsr();

namespace app {
namespace ethercat {

//! Anybus CompactCom EtherCAT handler module
class EtherCAT
{
public:
	using ErrorCode = embxx::error::ErrorCode;

	EtherCAT(common::EventLoop& eventLoop,
		encoders::Encoders& encoders);

	~EtherCAT();

	void start();

private:
	friend void ::APPL_SyncIsr();

	using Position = encoders::Encoders::Position;

	void setupABCCHardware();

	void handleSyncISR();

	void captureInputs();

	APPL_AbccHandlerStatusType _abccHandlerStatus;

	common::EventLoop& _eventLoop;
	encoders::Encoders& _encoders;

	static EtherCAT* _instance;
};

} // namespace ethercat
} // namespace app
