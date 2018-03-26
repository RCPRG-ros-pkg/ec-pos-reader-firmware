#include "hohner/SMRS59.hpp"

#include <cassert>
#include <limits>


#include "util/driverlib/ssi.hpp"

namespace hohner {

/**
 * @brief Constructor
 * @details Stores provided SSIMasterDevice
 * @pre Bit rate of SSI module must be less than 1.5MHz (Encoder limit)
 * @param ssiMasterDevice
 */
SMRS59::SMRS59(dev::SSIMasterDevice& ssiMasterDevice)
	:	_ssiMasterDevice(ssiMasterDevice)
{
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

}

} // namespace hohner
