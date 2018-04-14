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
	constexpr static auto BitRate = 1250000;
	constexpr static auto DataWidth = 13;
	using SSIMasterDevice = device::SSIMaster0<BitRate, DataWidth>;

	constexpr static auto Resolution = DataWidth;
	using SSIEncoder = component::SSIEncoder<SSIMasterDevice, Resolution>;

	SSIMasterDevice _ssiMasterDevice;
	SSIEncoder _ssiEncoder;
};

} // namespace encoders
} // namespace app
