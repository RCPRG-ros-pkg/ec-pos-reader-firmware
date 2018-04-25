#pragma once

#include "device/SSIMaster.hpp"
#include "device/OutputPin.hpp"

#include "component/SSIEncoder.hpp"
#include "component/LED.hpp"

#include <tuple>

#include "embxx/error/ErrorCode.h"

#include "app/encoders/Encoder.hpp"

namespace app {
namespace encoders {

class Encoders
{
public:
	using ErrorCode = embxx::error::ErrorCode;
	using Position = component::Position;

	using Encoder0 = Encoder<SSI0_BASE,
		GPIO_PORTB_BASE, 0,
		GPIO_PORTB_BASE, 1>;

	using Encoder1 = Encoder<SSI2_BASE,
		GPIO_PORTB_BASE, 2,
		GPIO_PORTB_BASE, 3>;

	using EncodersList = std::tuple<Encoder0, Encoder1>;

	//! Constructor
	Encoders();

	//! Captures current encoder position or returns and error on fail
	void captureInputs(Position& position, ErrorCode& ec);

	//! Returns, whether module is in fail state or not
	bool isFailed() const;

	//! Returns, whether module is in active state or not
	bool isActive() const;

private:
	EncodersList _encodersList;
};

} // namespace encoders
} // namespace app
