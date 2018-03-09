#include "hohner/SMRS59.hpp"

#include <cassert>
#include <limits>

#include "binary.h" // ETLCPP

#include "util/driverlib/ssi.hpp"

namespace hohner {

/**
 * @brief Constructor
 * @details Stores provided SSIMasterDevice
 * @pre Bit rate of SSI module must be less than 1.5MHz (Encoder limit)
 * @param ssiMasterDevice
 */
SMRS59::SMRS59(device::SSIMasterDevice& ssiMasterDevice)
	:	_ssiMasterDevice(ssiMasterDevice)
{
	assert(_ssiMasterDevice.getBitRate() < MaxBitRate);
	assert(_ssiMasterDevice.getFrameWidth() == Resolution);
}

/**
 * @brief Reads encoder absolute position value. Blocking call
 * @details It reads data from SSIMasterDevice, masks unused bits
 *  and converts received value from Gray to Binary format

 * @pre SSIMasterDevice should not be busy
 * @return returns absolute position of encoder
 */
Position
SMRS59::readPosition()
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

} // namespace hohner
