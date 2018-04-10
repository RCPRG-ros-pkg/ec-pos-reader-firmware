#pragma once

#include "device/SSIMaster.hpp"

#include "hohner/SMRS59.hpp"

#include "app/common/SysTickDriver.hpp"

#include "app/ModuleState.hpp"

namespace app {
namespace encoders {

class Encoders
{
public:
	Encoders(common::SysTickDriver& sysTickDriver);

	//! Starts module
	void start();

	//! Stops module
	void stop();

	//! Checks, if module is in active state
	bool isActive() const
	{
		return _moduleState == ModuleState::Active;
	}

	//! Checks, if module is in failed state
	bool isFailed() const
	{
		return _moduleState == ModuleState::Error;
	}

	//! Checks, if module is in ready state
	bool isReady() const
	{
		return _moduleState == ModuleState::Ready;
	}

	ModuleState getState() const
	{
		return _moduleState;
	}

private:
	using ErrorStatus = embxx::error::ErrorStatus;
	using ErrorCode = ErrorStatus::ErrorCodeType;

	template<typename T>
	using StaticFunction = embxx::util::StaticFunction<T>;

	using ErrorCallback = StaticFunction<void(const ErrorStatus&)>;

	using Timer = common::SysTickDriver::Timer;

	constexpr static auto BitRate = 1250000;
	constexpr static auto DataWidth = 13;
	using SSIMasterDevice = device::SSIMaster0<BitRate, DataWidth>;

	constexpr static auto Resolution = DataWidth;
	using Encoder = hohner::SMRS59<SSIMasterDevice, Resolution>;

	void doStart();

	void execWait();

	void readPosition();

	Timer _timer;
	SSIMasterDevice _ssiMasterDevice;
	Encoder _encoder;
	hohner::Position _position;
	ErrorCallback _errorCallback;
	ModuleState _moduleState = ModuleState::Ready;
	bool _running = false;
};

} // namespace encoders
} // namespace app
