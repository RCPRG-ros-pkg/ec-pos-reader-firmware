#pragma once

#include "device/SSIMaster.hpp"
#include "device/OutputPin.hpp"

#include "component/SSIEncoder.hpp"
#include "component/LED.hpp"

#include "embxx/error/ErrorCode.h"
#include "tivaware/utils/uartstdio.h"
#include "embxx/error/ErrorStatus.h"

namespace app {
namespace encoders {

template<std::uint32_t TSSIBase,
	std::uint32_t RedLEDPinGPIOBase, std::size_t RedLEDPinNumber,
	std::uint32_t GreenLEDPinGPIOBase, std::size_t GreenLEDPinNumber>
class Encoder
{
public:
	// devices typedefs
	using SSIMasterDevice = device::SSIMaster<TSSIBase>;
	using RedLEDPin = device::OutputPin<RedLEDPinGPIOBase, RedLEDPinNumber>;
	using GreenLEDPin = device::OutputPin<GreenLEDPinGPIOBase, GreenLEDPinNumber>;

	using ErrorCode = embxx::error::ErrorCode;
	using Position = component::Position;

	//! Constructor
	Encoder()
		:	_ssiMasterDevice(DefaultBitRate, DefaultFrameWidth),
			_redLEDPin(),
			_greenLEDPin(),
			_ssiEncoder(_ssiMasterDevice, DefaultResolution),
			_redLED(_redLEDPin),
			_greenLED(_greenLEDPin)
	{
		detectEncoder();

		assert(_status != Status::Init);
		assert(_redLED.isTurnedOn()
			|| _greenLED.isTurnedOn());
	}

	//! Captures current encoder position or returns and error on fail
	void captureInputs(Position& position, ErrorCode& errorCode)
	{
		assert(_status == Status::Active);
		assert(_redLED.isTurnedOff());
		assert(_greenLED.isTurnedOn());

		_ssiEncoder.readPosition(position, errorCode);
		if(embxx::error::ErrorStatus(errorCode))
		{
			// Error occured during reading the position.
			handleReadError(position, errorCode);
			return;
		}

		// Read position success. If changed, save current encoder position.
		if(position != _lastPosition)
		{
			_lastPosition = position;
		}
	}

	//! Returns, whether module is in active state or not
	bool isActive() const
	{
		return (_status == Status::Active);
	}

private:
	// components typedefs
	using SSIEncoder = component::SSIEncoder<SSIMasterDevice>;
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

	//! Handles read error reported by `captureInputs`.
	void handleReadError(Position& position, ErrorCode& errorCode)
	{
		assert(_status == Status::Active);
		assert(_redLED.isTurnedOff());
		assert(_greenLED.isTurnedOn());
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

	//! Detects, if encoder is connected or not.
	void detectEncoder()
	{
		assert(_redLED.isTurnedOff());
		assert(_greenLED.isTurnedOff());
		UARTprintf("[Encoder] detecting encoder...\n");

		// Read position and check for errors to determine connection
		Position position;
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
	Position _lastPosition;
	int _maxReadRetries = DefaultMaxReadRetries;
	int _readRetries = 0;
	Status _status = Status::Init;
};

} // namespace encoders
} // namespace app
