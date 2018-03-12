#pragma once

#include <cassert>
#include <cstdint>

#include "util/driverlib/ssi.hpp"

extern int clockHz;

namespace dev {

class SSIMasterDevice
{
public:
	//! Data type, which can contain one data frame. Used in read operations
	using DataType = std::uint16_t;

	//! Minimum frame width
	static constexpr std::size_t MinFrameWidth = 4;

	//! Maximum frame width
	static constexpr std::size_t MaxFrameWidth = 16;

	//! Constructor
	explicit SSIMasterDevice(std::uint32_t baseAddress,
		int bitRate, std::size_t frameWidth);

	//! Blocking reads a bunch of data from Slave device.
	void read(DataType* buffer, std::size_t size, std::size_t n);

	//! Blocking reads single data element from Slave device.
	DataType read();

	//! Returns bit rate, at which device is currently running
	int getBitRate() const;

	//! Returns data width, with which device is currently running
	std::size_t getFrameWidth() const;

	//! Checks, if device is currently transfering.
	bool isBusy() const;

private:
	//! Configures device to work as master with specified attributes
	void configure(int bitRate, std::size_t dataWidth);

	//! Size of Rx and Tx FIFO for SSI (both are equal)
	static constexpr std::size_t FIFOSize = 8;

	//! Dummy data, which will be used during writing to SSI
	static constexpr SSIMasterDevice::DataType DummyData = 0;

	//! Base address of Peripheral SSI module
	const std::uint32_t _baseAddress;
};

/**
 * @brief Returns bit rate, at which SSI device is currently running
 * @details
 */
inline int
SSIMasterDevice::getBitRate() const
{
	return SSIGetBitRate(_baseAddress, clockHz);
}

/**
 * @brief Checks, if device is currently transferring data
 * @details
 */
inline bool
SSIMasterDevice::isBusy() const
{
	return SSIBusy(_baseAddress);
}

/**
 * @brief Returns data width, with which device is currently running
 * @details [long description]
 * @post Returned frame frame will be in range [`MinFrameWidth`, `MaxFrameWidth`]
 * @return frame width
 */
inline std::size_t
SSIMasterDevice::getFrameWidth() const
{
	const auto frameWidth = SSIGetDataWidth(_baseAddress);
	assert(frameWidth >= MinFrameWidth
		&& frameWidth <= MaxFrameWidth);
	return frameWidth;
}

} // namespace dev
