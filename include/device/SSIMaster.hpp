#pragma once

#include <cstdint>
#include <type_traits>
#include <limits>

#include "util/driverlib/ssi.hpp"

#include "device/Peripheral.hpp"

#include "array_view.h"
#include "embxx/error/ErrorCode.h"

namespace device {

class SSIMasterBase
{
protected:
	//! Constructor
	SSIMasterBase(std::uint32_t baseAddress,
		int bitRate, std::size_t frameWidth);

public:
	using ErrorCode = embxx::error::ErrorCode;

	static constexpr std::size_t MinFrameWidth = (SSI_MIN_DATA_WIDTH - 1);
	static constexpr std::size_t MaxFrameWidth = (SSI_MAX_DATA_WIDTH - 1);
	static_assert(MinFrameWidth < MaxFrameWidth);

	using FrameType = SSIDataType;
	static_assert(std::numeric_limits<FrameType>::digits >= MaxFrameWidth,
		"Underlying frame type must have at least MaxFrameWidth bits size");

	//! Reads one frame item from SSI slave in blocking way
	void readOne(FrameType& frame, ErrorCode& errorCode);

	//! Reads multiple frame from SSI slave in blocking way
	std::size_t read(etl::array_view<FrameType> buffer, std::size_t n,
		ErrorCode& errorCode);

	//! Sets bit rate of transmission with SSI slave
	void setBitRate(int bitRate);

	//! Gets bit rate of transmission with SSI slave
	int getBitRate() const;

	//! Sets frame width in transmission with SSI slave
	void setFrameWidth(std::size_t frameWidth);

	//! Gets frame width in transmission with SSI slave
	std::size_t getFrameWidth() const;

	//! Determines, whether SSI master is currently communicating with SSI slave
	bool isBusy() const;

private:
	constexpr static auto DummyFrame = FrameType();

	const std::uint32_t _baseAddress;
};

template<std::uint32_t TBaseAddress, std::uint32_t TPeripheralId>
class SSIMaster
	:	public Peripheral<TPeripheralId>,
		public SSIMasterBase
{
public:
	static_assert(TBaseAddress != 0);
	constexpr static auto BaseAddress = TBaseAddress;

	using PeripheralType = Peripheral<TPeripheralId>;

	SSIMaster(int bitRate, std::size_t frameWidth)
		:	PeripheralType::Peripheral(),
			SSIMasterBase(BaseAddress, bitRate, frameWidth)
	{

	}
};

} // namespace device



