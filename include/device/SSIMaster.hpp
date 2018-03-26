#pragma once

#include <cassert>
#include <cstdint>
#include <limits>

#include "tivaware/driverlib/sysctl.h"

#include "util/driverlib/ssi.hpp"

#include "device/Peripheral.hpp"

extern int clockHz;

namespace device {

class SSIMasterBase
{
public:
	//! Minimum frame width
	static constexpr std::size_t MinFrameWidth = 4;

	//! Maximum frame width
	static constexpr std::size_t MaxFrameWidth = 16;

	//! Data type, which can contain one data frame. Used in read operations
	using DataType = std::uint16_t;
	static_assert(std::numeric_limits<DataType>::digits > MinFrameWidth);
	static_assert(std::numeric_limits<DataType>::digits <= MaxFrameWidth);

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

protected:
	//! Constructor
	explicit SSIMasterBase(std::uint32_t baseAddress,
		int bitRate, std::size_t frameWidth);

private:
	//! Configures device to work as master with specified attributes
	void configure(int bitRate, std::size_t dataWidth);

	//! Size of Rx and Tx FIFO for SSI (both are equal)
	static constexpr std::size_t FIFOSize = 8;

	//! Dummy data, which will be used during writing to SSI
	static constexpr SSIMasterBase::DataType DummyData = 0;

	//! Base address of Peripheral SSI module
	const std::uint32_t _baseAddress;
};

/**
 * @brief Returns bit rate, at which SSI device is currently running
 * @details
 */
inline int
SSIMasterBase::getBitRate() const
{
	return SSIGetBitRate(_baseAddress, clockHz);
}

/**
 * @brief Checks, if device is currently transferring data
 * @details
 */
inline bool
SSIMasterBase::isBusy() const
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
SSIMasterBase::getFrameWidth() const
{
	const auto frameWidth = SSIGetDataWidth(_baseAddress);
	assert(frameWidth >= MinFrameWidth
		&& frameWidth <= MaxFrameWidth);
	return frameWidth;
}

template<std::uint32_t TPeripheralID, std::uint32_t TBaseAddress,
	int TBitRate, std::size_t TFrameWidth>
class SSIMaster
	:	public Peripheral<TPeripheralID>,
		public SSIMasterBase
{
public:
	using PeripheralType = Peripheral<TPeripheralID>;

	static_assert(TBaseAddress > 0);
	constexpr static std::uint32_t BaseAddress = TBaseAddress;

	static_assert(TBitRate > 0);
	constexpr static int BitRate = TBitRate;

	static_assert(TFrameWidth >= MinFrameWidth);
	static_assert(TFrameWidth <= MaxFrameWidth);
	constexpr static std::size_t FrameWidth = TFrameWidth;

	SSIMaster()
		:	PeripheralType::Peripheral(),
			SSIMasterBase(BaseAddress, BitRate, FrameWidth)
	{}
};

template<int TBitRate, std::size_t TFrameWidth>
using SSIMaster0 = SSIMaster<SYSCTL_PERIPH_SSI0, SSI0_BASE, TBitRate, TFrameWidth>;

template<int TBitRate, std::size_t TFrameWidth>
using SSIMaster1 = SSIMaster<SYSCTL_PERIPH_SSI1, SSI1_BASE, TBitRate, TFrameWidth>;

template<int TBitRate, std::size_t TFrameWidth>
using SSIMaster2 = SSIMaster<SYSCTL_PERIPH_SSI2, SSI2_BASE, TBitRate, TFrameWidth>;

template<int TBitRate, std::size_t TFrameWidth>
using SSIMaster3 = SSIMaster<SYSCTL_PERIPH_SSI3, SSI3_BASE, TBitRate, TFrameWidth>;

} // namespace device
