#pragma once

#include <cassert>
#include <cstdint>

#include "util/driverlib/ssi.hpp"

namespace device {

class SSIMasterDevice
{
public:
	constexpr static std::size_t DataWidth = 13;
	using DataType = std::uint16_t;

	//! Constructor
	explicit SSIMasterDevice(std::uint32_t baseAddress);

	//! Blocking reads a bunch of data from SSI Slave device.
	void read(DataType* buffer, std::size_t size);

	//! Blocking reads single data element from SSI Slave device.
	DataType read();

	//! Checks, if SSI is currently transfering.
	bool isBusy();

	//! Gets base address of SSI module
	std::uint32_t getBaseAddress() const;

private:
	//! Size of Rx and Tx FIFO for SSI (both are equal)
	static constexpr std::size_t FIFOSize = 16;

	//! Dummy data, which will be used during writing to SSI
	static constexpr SSIMasterDevice::DataType DummyData = 0;

	const std::uint32_t _baseAddress;
};

/**
 * @brief Constructor
 * @details Stores base address of SSI module.
 * Preconditions:
 * - SSI module must be configured to work under Master mode
 * - Data width of module must be same as DataWidth static member
 *
 * @param baseAddress base address of SSI module
 */
inline
SSIMasterDevice::SSIMasterDevice(std::uint32_t baseAddress)
	:	_baseAddress(baseAddress)
{
	assert(SSIGetMode(_baseAddress) == SSIMode::Master);
	assert(SSIGetDataWidth(_baseAddress) == DataWidth);
}

/**
 * @brief Gets base address of SSI module
 * @return base address of SSI module
 */
inline std::uint32_t
SSIMasterDevice::getBaseAddress() const
{
	return _baseAddress;
}

} // namespace device
