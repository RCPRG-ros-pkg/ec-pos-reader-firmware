#pragma once

#include "device/SSIMaster.hpp"
#include "device/GPIO.hpp"

#include "component/SSIEncoder.hpp"
#include "component/LED.hpp"

#include "embxx/error/ErrorCode.h"

namespace app {
namespace encoders {

class Encoders
{
public:
	using ErrorCode = embxx::error::ErrorCode;
	using Position = component::Position;

	// devices typedefs
	using SSIMasterDevice = device::SSIMaster<SSI0_BASE, SYSCTL_PERIPH_SSI0>;
	using RedLEDPin = device::GPIOB::Pin<0>;
	using GreenLEDPin = device::GPIOB::Pin<1>;

	//! Constructor
	Encoders(RedLEDPin redLEDPin, GreenLEDPin greenLEDPin);

	//! Captures current encoder position or returns and error on fail
	void captureInputs(Position& position, ErrorCode& ec);

	//! Returns, whether module is in fail state or not
	bool isFailed() const;

	//! Returns, whether module is in active state or not
	bool isActive() const;

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
	void handleReadError(Position& position, ErrorCode& ec);

	//! Detects, if encoder is connected or not.
	void detectEncoder();

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
