#pragma once

#include "binary.h" // ETLCPP

namespace component {

template<typename TSSIMasterDevice>
class SSIEncoder
{
public:
	using PositionType = int;

	constexpr static auto MinResolution = TSSIMasterDevice::MinDataWidth;
	constexpr static auto MaxResolution = TSSIMasterDevice::MaxDataWidth;

	using SSIMasterDevice = TSSIMasterDevice;

	//! Constructor
	SSIEncoder(SSIMasterDevice& ssiMasterDevice,
		std::size_t resolution)
		:	_ssiMasterDevice(ssiMasterDevice)
	{
		assert(resolution >= MinResolution
			&& resolution <= MaxResolution);
		_ssiMasterDevice.setDataWidth(resolution);
	}

	//! Constructor
	SSIEncoder(SSIMasterDevice& ssiMasterDevice,
		std::size_t resolution, int bitRate)
		:	SSIEncoder(ssiMasterDevice, resolution)
	{
		setBitRate(bitRate);
	}

	//! Reads encoder position value. Blocking call.
	PositionType readPosition()
	{
		// read one data item from device
		assert(!_ssiMasterDevice.isBusy());
		auto data = _ssiMasterDevice.readOne();

		// clear unused bits
		const auto resolution = _ssiMasterDevice.getDataWidth();
		data &= ((1 << resolution) - 1);

		// convert from gray code to binary
		const auto value = etl::gray_to_binary(data);

		return value;
	}

	//! Sets bit rate of transmission with encoder
	void setBitRate(int bitRate)
	{
		_ssiMasterDevice.setBitRate(bitRate);
	}

	//! Gets the bit rate of transmission with encoder
	int getBitRate() const
	{
		return _ssiMasterDevice.getBitRate();
	}

	//! Gets the resolution of encoder
	std::size_t getResolution() const
	{
		return _ssiMasterDevice.getDataWidth();
	}

private:
	SSIMasterDevice& _ssiMasterDevice;
};

} // namespace component
