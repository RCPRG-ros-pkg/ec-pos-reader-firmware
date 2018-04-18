#pragma once

#include "binary.h" // ETLCPP
#include "embxx/error/ErrorStatus.h"

namespace component {

using Position = int;

template<typename TSSIMasterDevice>
class SSIEncoder
{
	using SSIMasterDeviceFrameType = typename TSSIMasterDevice::FrameType;

public:
	using SSIMasterDevice = TSSIMasterDevice;

	constexpr static auto MinResolution = SSIMasterDevice::MinFrameWidth;
	constexpr static auto MaxResolution = SSIMasterDevice::MaxFrameWidth;

	using ErrorCode = typename SSIMasterDevice::ErrorCode;

	//! Constructor
	SSIEncoder(SSIMasterDevice& ssiMasterDevice,
		std::size_t resolution)
		:	_ssiMasterDevice(ssiMasterDevice)
	{
		assert(resolution >= MinResolution
			&& resolution <= MaxResolution);

		// Change SSIMaster frame width only if needed
		const auto frameWidth = resolution;
		if(frameWidth != ssiMasterDevice.getFrameWidth())
		{
			ssiMasterDevice.setFrameWidth(resolution);
		}
	}

	//! Reads encoder position value. Blocking call.
	void readPosition(Position& position, ErrorCode& errorCode)
	{
		assert(!_ssiMasterDevice.isBusy());

		// read one frame from the device
		ErrorCode ec;
		SSIMasterDeviceFrameType frame;
		_ssiMasterDevice.readOne(frame, ec);
		if(embxx::error::ErrorStatus(ec))
		{
			// error occured during read operation
			errorCode = ec;
			return;
		}

		// read success
		// process received value
		position = etl::gray_to_binary(frame);
		errorCode = ec; // copy succeeded error code
	}

	//! Gets the resolution of encoder
	std::size_t getResolution() const
	{
		const auto frameWidth = _ssiMasterDevice.getFrameWidth();

		const auto resolution = frameWidth;
		assert(resolution >= MinResolution
			&& resolution <= MaxResolution);

		return resolution;
	}

private:
	SSIMasterDevice& _ssiMasterDevice;
};

} // namespace component
