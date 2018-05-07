#pragma once

#include "app/common/EventLoop.hpp"

#include "embxx/util/StaticFunction.h"
#include "embxx/error/ErrorCode.h"

#include "app/ethercat/Status.hpp"
#include "app/encoders/EncoderMgr.hpp"

#include "app/ethercat/abcc_appl/appl_abcc_handler.h"
#include "app/ethercat/abcc_abp/abp.h"

extern "C" void ABCC_CbfSyncIsr();
extern "C" void ABCC_CbfEvent(UINT16);
extern "C" void ABCC_CbfUserInitReq();
extern "C" void ABCC_CbfAnbStateChanged(ABP_AnbStateType);

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

	enum class State
	{
		Idle,
		Init,
		WaitForComm,
		Run,
		Error
	};

	void setupABCCHardware();

	void initialize();

	void waitForCommunication();

	void run();

	void handleSyncISR();

	void handleEvent(std::uint16_t event);

	void captureInputs();

	void captureInputsAsync();

	State _state = State::Idle;

	common::EventLoop& _eventLoop;
	encoders::EncoderMgr& _encoderMgr;

	static EtherCAT* _instance;
};

} // namespace ethercat
} // namespace app
