#pragma once

#include <cstdint>
#include <type_traits>
#include <limits>

#include "util/driverlib/ssi.hpp"

#include "init.hpp"

#include "device/Peripheral.hpp"

#include "array_view.h"

namespace device {

class SSIMasterBase
{
protected:
	//! Constructor
	SSIMasterBase(std::uint32_t baseAddress,
		int bitRate, std::size_t dataWidth);

	//! Constructor
	explicit
	SSIMasterBase(std::uint32_t baseAddress);

public:
	static constexpr std::size_t MinDataWidth = SSI_MIN_DATA_WIDTH;
	static constexpr std::size_t MaxDataWidth = SSI_MAX_DATA_WIDTH;
	static_assert(MinDataWidth < MaxDataWidth);

	using DataType = SSIDataType;
	static_assert(std::numeric_limits<DataType>::digits >= MaxDataWidth,
		"Underlying data type must have at least MaxDataWidth bits size");

	//! Reads one data item from SSI slave in blocking way
	DataType readOne();

	//! Reads multiple data from SSI slave in blocking way
	void read(etl::array_view<DataType> buffer, std::size_t n);

	//! Sets bit rate of transmission with SSI slave
	void setBitRate(int bitRate)
	{
		SSIBitRateSet(_baseAddress, ClockHz, bitRate);
	}

	//! Gets bit rate of transmission with SSI slave
	int getBitRate() const
	{
		return SSIBitRateGet(_baseAddress, ClockHz);
	}

	//! Sets data width in transmission with SSI slave
	void setDataWidth(std::size_t dataWidth)
	{
		SSIDataWidthSet(_baseAddress, dataWidth);
	}

	//! Gets data width in transmission with SSI slave
	std::size_t getDataWidth() const
	{
		return SSIDataWidthGet(_baseAddress);
	}

	//! Determines, whether SSI master is currently communicating with SSI slave
	bool isBusy() const
	{
		return SSIBusy(_baseAddress);
	}

private:
	constexpr static auto DefaultBitRate = 1000000;

	constexpr static auto DefaultDataWidth = 8;

	constexpr static auto DummyData = DataType();

	const std::uint32_t _baseAddress;
};

template<std::uint32_t TBaseAddress, std::uint32_t TPeripheralId>
class SSIMaster
	:	public SSIMasterBase,
		public Peripheral<TPeripheralId>
{
public:
	static_assert(TBaseAddress != 0);
	constexpr static auto BaseAddress = TBaseAddress;

	using PeripheralType = Peripheral<TPeripheralId>;

	SSIMaster()
		:	SSIMasterBase(BaseAddress),
			PeripheralType::Peripheral()
	{

	}

	SSIMaster(int bitRate, std::size_t dataWidth)
		:	SSIMasterBase(BaseAddress, bitRate, dataWidth),
			PeripheralType::Peripheral()
	{

	}
};

} // namespace device



