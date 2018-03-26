#pragma once

#include <cstdint>
#include <limits>

#include "tivaware/driverlib/sysctl.h"

#include "util/driverlib/ssi.hpp"

#include "device/Peripheral.hpp"

namespace device {

class SPIBase
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

	// //! Blocking reads a bunch of data from Slave device.
	// void read(DataType* buffer, std::size_t size, std::size_t n);

	// //! Blocking reads single data element from Slave device.
	// DataType read();

	//! Returns bit rate, at which device is currently running
	int getBitRate() const;

	//! Returns data width, with which device is currently running
	std::size_t getFrameWidth() const;

	//! Checks, if device is currently transfering.
	bool isBusy() const;

protected:
	SPIBase(std::uint32_t baseAddress)
		:	_baseAddress(baseAddress)
	{}

private:
	const std::uint32_t _baseAddress;
};

template<std::uint32_t TPeripheralID, std::uint32_t TBaseAddress>
class SPI
	:	public Peripheral<TPeripheralID>,
		public SPIBase
{
public:
	using PeripheralType = Peripheral<TPeripheralID>;

	static_assert(TBaseAddress > 0);
	constexpr static std::uint32_t BaseAddress = TBaseAddress;

	// static_assert(TBitRate > 0);
	// constexpr static int BitRate = TBitRate;

	// static_assert(TFrameWidth >= MinFrameWidth);
	// static_assert(TFrameWidth <= MaxFrameWidth);
	// constexpr static std::size_t FrameWidth = TFrameWidth;

	SPI()
		:	PeripheralType::Peripheral(),
			SPIBase(BaseAddress)
	{

	}
};

using SPI0 = SPI<SYSCTL_PERIPH_SSI0, SSI0_BASE>;
using SPI1 = SPI<SYSCTL_PERIPH_SSI1, SSI1_BASE>;
using SPI2 = SPI<SYSCTL_PERIPH_SSI2, SSI2_BASE>;
using SPI3 = SPI<SYSCTL_PERIPH_SSI3, SSI3_BASE>;

} // namespace device
