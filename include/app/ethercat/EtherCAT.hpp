#pragma once

#include "app/common/SysTickDriver.hpp"

#include "embxx/util/StaticFunction.h"

#include "app/ethercat/Status.hpp"

namespace app {
namespace ethercat {

//! Anybus CompactCom EtherCAT handler module
class EtherCAT
{
public:
	EtherCAT(common::EventLoop& eventLoop,
		common::SysTickDriver& sysTickDriver);

	~EtherCAT();

	template<typename TCallback>
	void initialize(TCallback&& callback);

	template<typename TCallback>
	void start(TCallback&& callback);

private:
	using Timer = common::SysTickDriver::Timer;
	using ErrorStatus = embxx::error::ErrorStatus;
	using ErrorCode = ErrorStatus::ErrorCodeType;

	template<typename T>
	using StaticFunction = embxx::util::StaticFunction<T>;

	using InitCallback = StaticFunction<void(Status)>;
	using StartCallback = StaticFunction<void(Status)>;

	enum class State
	{
		PreInit,
		Init,
		Ready,
		Active,
		Error
	};

	friend void ABCC_CbfUserInitReq();

	void setupABCCHardware();

	void doInitialization();

	void doStart();

	bool detectModule();

	bool initApplicationDataObject();

	bool startABCCDriver();

	void waitForCommunication();

	common::EventLoop& _eventLoop;
	Timer _timer;
	InitCallback _initCallback;
	StartCallback _startCallback;
	State _state = State::PreInit;

	static EtherCAT* _instance;
};

template<typename TCallback>
inline void
EtherCAT::initialize(TCallback&& callback)
{
	assert(_state == State::PreInit);
	_initCallback = std::forward<TCallback>(callback);
	doInitialization();
}

template<typename TCallback>
inline void
EtherCAT::start(TCallback &&callback)
{
	assert(_state == State::Ready);
	_startCallback = std::forward<TCallback>(callback);
	doStart();
}

} // namespace ethercat
} // namespace app
