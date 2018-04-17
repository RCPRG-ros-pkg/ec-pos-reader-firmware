#pragma once

#include "device/SSIMaster.hpp"

#include "component/SSIEncoder.hpp"

namespace app {
namespace encoders {

class Encoders
{
public:
	Encoders();

	int readPosition();

private:
	// devices typedefs
	using Encoder0SSIMasterDevice = device::SSIMaster<SSI0_BASE, SYSCTL_PERIPH_SSI0>;
	using Encoder1SSIMasterDevice = device::SSIMaster<SSI1_BASE, SYSCTL_PERIPH_SSI1>;

	// components typedefs
	using SSIEncoder0 = component::SSIEncoder<Encoder0SSIMasterDevice>;
	using SSIEncoder1 = component::SSIEncoder<Encoder1SSIMasterDevice>;

	// default SSI settings for encoders
	constexpr static auto DefaultBitRate = 1500000;
	constexpr static auto DefaultResolution = 13;

	// devices members
	Encoder0SSIMasterDevice _encoder0SSIMasterDevice;
	Encoder1SSIMasterDevice _encoder1SSIMasterDevice;

	// components members
	SSIEncoder0 _ssiEncoder0;
	SSIEncoder1 _ssiEncoder1;
};

} // namespace encoders
} // namespace app
