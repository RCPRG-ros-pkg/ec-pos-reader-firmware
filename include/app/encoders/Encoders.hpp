#pragma once

#include "device/SSIMaster.hpp"

#include "component/SSIEncoder.hpp"

#include "embxx/error/ErrorCode.h"

namespace app {
namespace encoders {

class Encoders
{
public:
	using ErrorCode = embxx::error::ErrorCode;

	using Position = component::Position;

	Encoders();

	void readPosition(Position& position, ErrorCode& ec);

private:
	// devices typedefs
	using Encoder0SSIMasterDevice = device::SSIMaster<SSI0_BASE, SYSCTL_PERIPH_SSI0>;
	using Encoder1SSIMasterDevice = device::SSIMaster<SSI1_BASE, SYSCTL_PERIPH_SSI1>;

	// components typedefs
	using Encoder0 = component::SSIEncoder<Encoder0SSIMasterDevice>;
	using Encoder1 = component::SSIEncoder<Encoder1SSIMasterDevice>;

	// default SSI settings for encoders
	constexpr static auto DefaultBitRate = 1000000;
	constexpr static auto DefaultFrameWidth = 13;
	constexpr static auto DefaultResolution = 13;

	// devices members
	Encoder0SSIMasterDevice _encoder0SSIMasterDevice;
	Encoder1SSIMasterDevice _encoder1SSIMasterDevice;

	// components members
	Encoder0 _encoder0;
	Encoder1 _encoder1;

	Position _encoder0LastPosition;
	Position _encoder1LastPosition;
};

} // namespace encoders
} // namespace app
