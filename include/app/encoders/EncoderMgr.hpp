#pragma once

#include <tuple>

#include "embxx/error/ErrorCode.h"

#include "device/SSIMaster.hpp"
#include "device/OutputPin.hpp"

#include "component/SSIEncoder.hpp"
#include "component/LED.hpp"

#include "app/encoders/Encoder0.hpp"
#include "app/encoders/Encoder1.hpp"

namespace app {
namespace encoders {

class EncoderMgr
{
public:
	using ErrorCode = embxx::error::ErrorCode;
	using Position = component::Position;

	using Encoders = std::tuple<
		Encoder0
		,Encoder1
	>;

	//! Constructor
	EncoderMgr();

	//! Captures current encoder position or returns and error on fail
	void captureInputs(Position& position, ErrorCode& ec);

	//! Returns, whether module is in fail state or not
	bool isFailed() const;

	//! Returns, whether module is in active state or not
	bool isActive() const;

private:
	Encoders _encoders;
};

} // namespace encoders
} // namespace app
