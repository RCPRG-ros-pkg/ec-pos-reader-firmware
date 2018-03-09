#pragma once

#include "device/SSIMasterDevice.hpp"

#include "hohner/Position.hpp"

namespace hohner {

/**
 * @brief Hohner's Rotary Encoder class, SMRS59 series
 * @details
 */
class SMRS59
{
public:
	//! Resolution at which works the encoder. Also frame width in SSI.
	static constexpr std::size_t Resolution = 13;

	//! Miximum bit rate, with which encoder can work.
	static constexpr int MaxBitRate = 1500000;

	//! Constructor
	SMRS59(device::SSIMasterDevice& ssiMasterDevice);

	//! Reads encoder position value. Blocking call.
	Position readPosition();

private:
	device::SSIMasterDevice& _ssiMasterDevice;
};

} // namespace hohner
