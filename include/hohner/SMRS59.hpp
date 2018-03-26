#pragma once

#include "binary.h" // ETLCPP

#include "hohner/Position.hpp"

namespace hohner {

/**
 * @brief Hohner's Rotary Encoder class, SMRS59 series
 * @details
 */
template<typename TSSIMasterDevice, std::size_t Resolution>
class SMRS59
{
public:
	//! Constructor
	SMRS59(TSSIMasterDevice& ssiMasterDevice)
		:	_ssiMasterDevice(ssiMasterDevice)
	{

	}

	//! Reads encoder position value. Blocking call.
	Position readPosition()
	{
		// read one frame from device
		assert(!_ssiMasterDevice.isBusy());
		auto data = _ssiMasterDevice.read();

		// clear unused bits
		assert(_ssiMasterDevice.getFrameWidth() == Resolution);
		data &= ((1 << Resolution) - 1);

		// convert from gray code to binary
		const auto value = etl::gray_to_binary(data);

		return Position(value);
	}

private:
	TSSIMasterDevice& _ssiMasterDevice;
};

} // namespace hohner
