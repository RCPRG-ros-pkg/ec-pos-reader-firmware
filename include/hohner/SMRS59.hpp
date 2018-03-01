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
	//! Constructor
	SMRS59(device::SSIMasterDevice& ssiMasterDevice);

	//! Reads encoder position value. Blocking call
	Position readPosition();

private:
	device::SSIMasterDevice& _ssiMasterDevice;
};

} // namespace hohner
