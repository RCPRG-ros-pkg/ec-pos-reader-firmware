#pragma once

#include "device/SSIMaster.hpp"

#include "hohner/SMRS59.hpp"

#include "app/common/SysTickDriver.hpp"

namespace app {
namespace encoders {

class Encoders
{
public:
	Encoders(common::SysTickDriver& sysTickDriver);

	void startReading();

private:
	using Timer = common::SysTickDriver::Timer;

	constexpr static auto BitRate = 1250000;
	constexpr static auto DataWidth = 13;
	using SSIMasterDevice = device::SSIMaster0<BitRate, DataWidth>;

	constexpr static auto Resolution = DataWidth;
	using Encoder = hohner::SMRS59<SSIMasterDevice, Resolution>;

	void readPosition();

	Timer _timer;
	SSIMasterDevice _ssiMasterDevice;
	Encoder _encoder;
	hohner::Position _position;
};

} // namespace encoders
} // namespace app
