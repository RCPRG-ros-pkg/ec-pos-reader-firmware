#pragma once

#include "app/common/EventLoop.hpp"

#include "embxx/util/StaticFunction.h"
#include "embxx/error/ErrorCode.h"

#include "app/encoders/Encoder0.hpp"
#include "app/encoders/Encoder1.hpp"

#include "app/ethercat/abcc_appl/appl_abcc_handler.h"
#include "app/ethercat/abcc_abp/abp.h"

extern "C" void ABCC_CbfSyncIsr();
extern "C" void ABCC_CbfEvent(UINT16);
extern "C" void ABCC_CbfUserInitReq();
extern "C" void ABCC_CbfAnbStateChanged(ABP_AnbStateType);
extern "C" void setEncoder0Settings(const struct AD_AdiEntry* psAdiEntry,
	UINT8 bNumElements, UINT8 bStartIndex);
extern "C" void getEncoder0Inputs(const struct AD_AdiEntry* adiEntry,
	UINT8 numElements, UINT8 startIndex);
extern "C" void getEncoder1Inputs(const struct AD_AdiEntry* adiEntry,
	UINT8 numElements, UINT8 startIndex);

namespace app {
namespace ethercat {

//! Anybus CompactCom EtherCAT handler module
class EtherCAT
{
public:
	using ErrorCode = embxx::error::ErrorCode;

	EtherCAT(common::EventLoop& eventLoop,
		encoders::Encoder0& encoder0,
		encoders::Encoder1& encoder1);

	~EtherCAT();

	void start();

private:
	friend void ::ABCC_CbfSyncIsr();
	friend void ::ABCC_CbfEvent(UINT16);
	friend void ::ABCC_CbfUserInitReq();
	friend void ::ABCC_CbfAnbStateChanged(ABP_AnbStateType);
	friend void ::setEncoder0Settings(const struct AD_AdiEntry *, UINT8, UINT8);
	friend void ::getEncoder0Inputs(const struct AD_AdiEntry *, UINT8, UINT8);
	friend void ::getEncoder1Inputs(const struct AD_AdiEntry *, UINT8, UINT8);

	enum class State
	{
		Idle,
		DriverInit,
		WaitForComm,
		Run,
		Error
	};

	void setupABCCHardware();

	void initDriver();

	void waitForCommunication();

	void run();

	void handleSyncISR();

	void captureInputs();

	void captureEncoder0Inputs();

	void captureEncoder1Inputs();

	void captureInputsAsync();

	State _state = State::Idle;
	ABP_AnbStateType _anbState = ABP_ANB_STATE_SETUP;

	common::EventLoop& _eventLoop;
	encoders::Encoder0& _encoder0;
	encoders::Encoder1& _encoder1;

	static EtherCAT* _instance;
};

} // namespace ethercat
} // namespace app
