#include "device/SSIMasterDevice.hpp"

#include <algorithm>

namespace device {

void
SSIMasterDevice::read(DataType* buffer, std::size_t size)
{
	assert(buffer != nullptr);
	assert(size <= FIFOSize); // implemented only for buffers max to FIFO capacity
	assert(SSIEnabled(_baseAddress));
	assert(SSIIdle(_baseAddress));
	assert(SSIRxEmpty(_baseAddress));

	for(int i = 0; i < static_cast<int>(size); ++i)
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

bool
SSIMasterDevice::isBusy()
{
	return SSIBusy(_baseAddress);
}

} // namespace device
