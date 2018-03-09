#include "device/SSIMasterDevice.hpp"

#include <algorithm>
#include <limits>

//! CPU clock speed
extern int clockHz;

namespace device {

/**
 * @brief Constructor
 * @details Configures device to work at specified bit rate
 * Preconditions:
 * - SSI module must be configured to work under Master mode
 * - Data width of module must be same as DataWidth static member
 *
 * @param baseAddress base address of SSI module
 */
SSIMasterDevice::SSIMasterDevice(std::uint32_t baseAddress,
	int bitRate, std::size_t frameWidth)
	:	_baseAddress(baseAddress)
{
	assert(bitRate > 0);
	assert(frameWidth >= MinFrameWidth
		&& frameWidth <= MaxFrameWidth);

	configure(bitRate, frameWidth);
}

void
SSIMasterDevice::read(DataType* buffer, std::size_t size, std::size_t n)
{
	// Data type must be able to contain at least one frame
	using DataType = device::SSIMasterDevice::DataType;
	constexpr auto DataTypeResolution = std::numeric_limits<DataType>::digits;
	static_assert(DataTypeResolution >= MaxFrameWidth);

	assert(buffer != nullptr);
	assert(size >= n);
	assert(size <= FIFOSize); // implemented only for buffers max to FIFO capacity
	assert(SSIEnabled(_baseAddress));
	assert(SSIIdle(_baseAddress));
	assert(SSIRxEmpty(_baseAddress));

	for(int i = 0; i < static_cast<int>(n); ++i)
	{
		assert(SSITxNotFull(_baseAddress));
		SSIPutDataNow(_baseAddress, DummyData);
	}

	const auto begin = buffer;
	const auto end = buffer + size;
	std::generate(begin, end, [this]() {
		assert(SSIRxNotEmpty(_baseAddress));
		const auto data = SSIGetDataNow(_baseAddress);
		return static_cast<DataType>(data);
	});
}

SSIMasterDevice::DataType
SSIMasterDevice::read()
{
	// Data type must be able to contain at least one frame
	using DataType = device::SSIMasterDevice::DataType;
	constexpr auto DataTypeResolution = std::numeric_limits<DataType>::digits;
	static_assert(DataTypeResolution >= MaxFrameWidth);

	assert(SSIEnabled(_baseAddress));
	assert(SSIIdle(_baseAddress));
	assert(SSIRxEmpty(_baseAddress));

	// insert Dummy data to init transfer
	assert(SSITxNotFull(_baseAddress));
	SSIPutDataNow(_baseAddress, DummyData);

	// wait for transfer to be completed
	while(SSIBusy(_baseAddress));

	// data should be available
	assert(SSIRxNotEmpty(_baseAddress));
	const auto data = SSIGetDataNow(_baseAddress);

	return static_cast<DataType>(data);
}

void
SSIMasterDevice::configure(int bitRate, std::size_t frameWidth)
{
	assert(bitRate != 0);
	assert(frameWidth >= MinFrameWidth
		&& frameWidth <= MaxFrameWidth);

	SSIConfigSetExpClk(_baseAddress, clockHz, SSI_FRF_TI,
		SSI_MODE_MASTER, bitRate, frameWidth);
	SSIEnable(_baseAddress);
	assert(SSIGetMode(_baseAddress) == SSIMode::Master);
	assert(SSIGetDataWidth(_baseAddress) == frameWidth);

	// cache this information, because reading it from registers is difficult
	_bitRate = bitRate;
}

} // namespace device


