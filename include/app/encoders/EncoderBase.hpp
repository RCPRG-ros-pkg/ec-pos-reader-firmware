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

template<std::uint32_t TSSIBase, std::uint32_t TSSIId, std::uint32_t TSSIInt,
	std::uint32_t RedLEDPinGPIOBase, std::size_t RedLEDPinNumber,
	std::uint32_t GreenLEDPinGPIOBase, std::size_t GreenLEDPinNumber>
class EncoderBase
{
public:
	// devices typedefs
	using SSIMasterDevice = device::SSIMaster<TSSIBase, TSSIId, TSSIInt>;
	using RedLEDPin = device::OutputPin<RedLEDPinGPIOBase, RedLEDPinNumber>;
	using GreenLEDPin = device::OutputPin<GreenLEDPinGPIOBase, GreenLEDPinNumber>;

	using ErrorCode = embxx::error::ErrorCode;
	using Position = component::Position;

	using EventLoop = common::EventLoop;

	using InputsCapturedHandler =
		embxx::util::StaticFunction<void(ErrorCode), 1 * sizeof(void*)>;

	//! Constructor
	EncoderBase(EventLoop& eventLoop)
		:	_ssiMasterDevice(DefaultBitRate, DefaultFrameWidth),
			_redLEDPin(),
			_greenLEDPin(),
			_ssiEncoder(eventLoop, _ssiMasterDevice, DefaultResolution),
			_redLED(_redLEDPin),
			_greenLED(_greenLEDPin)
	{
		detectEncoder();

		assert(_status != Status::Init);
		assert(_destPosition == nullptr);
	}

	template<typename THandler>
	void asyncCaptureInputs(Position* destPosition, THandler&& handler)
	{
		// Module should not be busy and have "active status"
		assert(!isBusy());
		assert(_status == Status::Active);

		// Target position pointer should be non-null, so store it
		assert(destPosition != nullptr);
		assert(_destPosition == nullptr);
		_destPosition = destPosition;

		// Store provided handler
		_inputsCapturedHandler = std::forward<THandler>(handler);

		// Begin asynchronous read of position
		_ssiEncoder.asyncReadPosition(&_position,
			[this](auto errorCode) { positionRead(errorCode); });
	}

	//! Captures current encoder position or returns and error on fail
	void captureInputs(Position& position, ErrorCode& errorCode)
	{
		// Module should not be busy and have "active" status
		assert(!isBusy());
		assert(_status == Status::Active);

		_ssiEncoder.readPosition(position, errorCode);
		if(embxx::error::ErrorStatus(errorCode))
		{
			// Error occured during reading the position.
			handleReadError(position, errorCode);
			return;
		}

		// Read position success. If changed, save current encoder position.
		handleReadSuccess(position);
	}

	bool isBusy()
	{
		return _ssiEncoder.isBusy();
	}

	//! Returns, whether module is in active state or not
	bool isActive() const
	{
		return (_status == Status::Active);
	}

private:
	// components typedefs
	using SSIEncoder =
		component::SSIEncoder<EventLoop, SSIMasterDevice,
			embxx::util::StaticFunction<void(ErrorCode), 1 * sizeof(void*)>>;
	using RedLED = component::LED<RedLEDPin>;
	using GreenLED = component::LED<GreenLEDPin>;

	// Default SSI settings for encoders.
	constexpr static auto DefaultBitRate = 1500000;
	constexpr static auto DefaultFrameWidth = 13;
	constexpr static auto DefaultResolution = 13;

	//! Default value of maximum number of retries to read the position.
	constexpr static auto DefaultMaxReadRetries = 16;

	//! Represents internal status of module.
	enum class Status
	{
		Init, //< Occurs only in constructing phase.
		Active, //< Encoder is detected and it is correct operating.
		Failed //< Encoder not detected or number of failed reads exceeded.
	};

	void positionRead(ErrorCode errorCode)
	{
		if(embxx::error::ErrorStatus(errorCode))
		{
			// Error occured during reading the position.
			handleReadError(_position, errorCode);
			// TODO: WRONG BEHAVIOUR!!!
		}
		else
		{
			// Read position success.
			handleReadSuccess(_position);
		}

		assert(_destPosition != nullptr);
		*_destPosition = _position;
		_destPosition = nullptr;

		assert(_inputsCapturedHandler);
		_inputsCapturedHandler(errorCode);
	}

	//! Handles read error reported by `captureInputs`.
	void handleReadError(Position& position, ErrorCode& errorCode)
	{
		assert(_status == Status::Active);
		assert(_maxReadRetries >= 0);
		assert(_readRetries >= 0 && _readRetries < _maxReadRetries);
		if(++_readRetries == _maxReadRetries)
		{
			// The number of retries has been exceeded.
			UARTprintf("[Encoder] read position error after %d retries, errorCode=%d\n",
				_maxReadRetries, static_cast<int>(errorCode));

			// Change status of module into failed
			_status = Status::Failed;
			_greenLED.turnOff();
			_redLED.turnOn();
			return;
		}

		// The number of retries has been not exceeded.
		// Return the last known correct position to the caller and signal success
		position = _lastPosition;
		errorCode = ErrorCode::Success;
	}

	void handleReadSuccess(const Position& position)
	{
		if(position != _lastPosition)
		{
			_lastPosition = position;
		}
	}

	//! Detects, if encoder is connected or not.
	void detectEncoder()
	{
		UARTprintf("[Encoder] detecting encoder...\n");

		// Read position and check for errors to determine connection
		Position position = 0; // Initialize with zero to supress "maybe-unitialized" warn
		ErrorCode errorCode;
		_ssiEncoder.readPosition(position, errorCode);
		if(embxx::error::ErrorStatus(errorCode))
		{
			// Read position error. Assume, that encoder is not connected.
			UARTprintf("[Encoder] encoder not connected, error code: %d\n",
				errorCode);

			// Change status of module into failed
			_status = Status::Failed;
			_redLED.turnOn();
			return;
		}

		// Read position success, encoder ready. Change status to active.
		UARTprintf("[Encoder] encoder connected, position: %d\n",
			position);
		UARTprintf("[Encoder] active\n");
		_status = Status::Active;
		_greenLED.turnOn();
		return;
	}

	// devices members
	SSIMasterDevice _ssiMasterDevice;
	RedLEDPin _redLEDPin;
	GreenLEDPin _greenLEDPin;

	// components members
	SSIEncoder _ssiEncoder;
	RedLED _redLED;
	GreenLED _greenLED;

	// other members
	InputsCapturedHandler _inputsCapturedHandler;
	Position _position;
	Position* _destPosition = nullptr; //< Used in async calls
	Position _lastPosition;
	int _maxReadRetries = DefaultMaxReadRetries;
	int _readRetries = 0;
	Status _status = Status::Init;
};

} // namespace encoders
} // namespace app
