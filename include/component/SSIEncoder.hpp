#pragma once

#include "binary.h" // ETLCPP

namespace component {

template<typename TSSIMasterDevice, std::size_t Resolution>
class SSIEncoder
{
public:
	//! Constructor
	SSIEncoder(TSSIMasterDevice& ssiMasterDevice)
		:	_ssiMasterDevice(ssiMasterDevice)
	{

	}

	//! Reads encoder position value. Blocking call.
	int readPosition()
	{
		// read one frame from device
		assert(!_ssiMasterDevice.isBusy());
		auto data = _ssiMasterDevice.read();

		// clear unused bits
		assert(_ssiMasterDevice.getFrameWidth() == Resolution);
		data &= ((1 << Resolution) - 1);

		// convert from gray code to binary
		const auto value = etl::gray_to_binary(data);

		return value;
	}

private:
	TSSIMasterDevice& _ssiMasterDevice;
};

} // namespace component
