#pragma once

#include "app/common/EventLoop.hpp"

#include "embxx/util/StaticFunction.h"
#include "embxx/error/ErrorCode.h"

#include "app/ethercat/Status.hpp"
#include "app/encoders/EncoderMgr.hpp"

#include "app/ethercat/abcc_appl/appl_abcc_handler.h"

extern "C" void ABCC_CbfSyncIsr();
extern "C" void ABCC_CbfEvent(UINT16);

namespace app {
namespace ethercat {

//! Anybus CompactCom EtherCAT handler module
class EtherCAT
{
public:
	using ErrorCode = embxx::error::ErrorCode;

	EtherCAT(common::EventLoop& eventLoop,
		encoders::EncoderMgr& encoderMgr);

	~EtherCAT();

	void start();

private:
	friend void ::ABCC_CbfSyncIsr();
	friend void ::ABCC_CbfEvent(UINT16);

	using Position = encoders::EncoderMgr::Position;

	void setupABCCHardware();

	void handleSyncISR();

	void handleEvent(std::uint16_t event);

	void captureInputs();

	void captureInputsAsync();

	APPL_AbccHandlerStatusType _abccHandlerStatus;

	common::EventLoop& _eventLoop;
	encoders::EncoderMgr& _encoderMgr;

	Position _position; //< Used in async calls
	volatile bool _called = true;
	static EtherCAT* _instance;
};

} // namespace ethercat
} // namespace app
