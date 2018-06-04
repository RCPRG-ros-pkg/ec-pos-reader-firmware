#pragma once

#include "embxx/error/ErrorCode.h"
#include "embxx/error/ErrorStatus.h"
#include "embxx/util/StaticFunction.h"

#include "tivaware/utils/uartstdio.h"

#include "device/SSIMaster.hpp"
#include "device/OutputPin.hpp"

#include "component/SSIEncoder.hpp"
#include "component/LED.hpp"

#include "app/common/EventLoop.hpp"

namespace app {
namespace encoders {

template<std::uint32_t TSSIBase, std::uint32_t TSSIId, std::uint32_t TSSIInt>
class EncoderBase
{
	using EventLoop = common::EventLoop;

	using SSIMasterDevice = device::SSIMaster<TSSIBase, TSSIId, TSSIInt>;

	using ErrorCode = embxx::error::ErrorCode;

public:
	using Position = component::Position;

	//! Constructor
	EncoderBase(EventLoop& eventLoop)
		:	_ssiMasterDevice(DefaultBitRate, DefaultFrameWidth),
			_ssiEncoder(eventLoop, _ssiMasterDevice, DefaultResolution)
	{
		UARTprintf("[Encoder] ready\n");

		assert(!isBusy());
	}

	//! Captures current encoder positions asynchronously
	template<typename THandler>
	void asyncCaptureInputs(Position* destPosition, THandler&& handler)
	{
		// Module should not be busy and have "active status"
		assert(!isBusy());

		// Store provided handler
		_inputsCapturedHandler = std::forward<THandler>(handler);

		// Begin asynchronous read of position
		_ssiEncoder.asyncReadPosition(destPosition,
			[this](auto errorCode) { positionRead(errorCode); });
	}

	//! Captures current encoder position in blocking way
	void captureInputs(Position& position, ErrorCode& errorCode)
	{
		// Module should not be busy and have "active" status
		assert(!isBusy());

		_ssiEncoder.readPosition(position, errorCode);
		if(embxx::error::ErrorStatus(errorCode))
		{
			// Error occured during reading the position.
			handleReadError(errorCode);
			return;
		}
	}

	//! Returns, whether module is busy or not
	bool isBusy()
	{
		return _ssiEncoder.isBusy();
	}

private:
	using InputsCapturedHandler =
		embxx::util::StaticFunction<void(ErrorCode), 1 * sizeof(void*)>;

	// components typedefs
	using SSIEncoder =
		component::SSIEncoder<EventLoop, SSIMasterDevice,
			embxx::util::StaticFunction<void(ErrorCode), 1 * sizeof(void*)>>;

	// Default SSI settings for encoders.
	constexpr static auto DefaultBitRate = 1250000;
	constexpr static auto DefaultFrameWidth = 13;
	constexpr static auto DefaultResolution = 13;

	void positionRead(ErrorCode errorCode)
	{
		// Async read of position ends. Check its status
		if(embxx::error::ErrorStatus(errorCode))
		{
			// Error occured during reading the position.
			handleReadError(errorCode);
		}

		// Invoke callback and forward error code
		assert(_inputsCapturedHandler);
		_inputsCapturedHandler(errorCode);
	}

	//! Handles read error reported by `captureInputs`.
	void handleReadError(const ErrorCode& /*errorCode*/)
	{
		// UARTprintf("[Encoder] could not read position, ec=%d\n",
		// 	static_cast<int>(errorCode));
	}

	// devices members
	SSIMasterDevice _ssiMasterDevice;

	// components members
	SSIEncoder _ssiEncoder;

	// other members
	InputsCapturedHandler _inputsCapturedHandler;
};

} // namespace encoders
} // namespace app
