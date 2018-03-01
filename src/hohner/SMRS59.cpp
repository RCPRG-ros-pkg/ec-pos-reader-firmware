#include "hohner/SMRS59.hpp"

#include <cassert>

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
	// assert(SSIGetBitRate(ssiMasterDevice.getBaseAddress()) < 1500000); // < 1.5MHz
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
	static_assert(device::SSIMasterDevice::DataWidth == 13);
	assert(!_ssiMasterDevice.isBusy());

	// read 13bit SSI frame from Encoder(Slave)
	auto data = _ssiMasterDevice.read();

	// clear unused bits
	data &= ((1 << 13) - 1);

	// convert from gray code to binary
	const auto value = etl::gray_to_binary(data);

	return Position(value);
}

} // namespace hohner
