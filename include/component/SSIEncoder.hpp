#pragma once

#include "binary.h" // ETLCPP
#include "embxx/error/ErrorStatus.h"
#include "embxx/device/context.h"

#include "embxx/util/StaticFunction.h"
#include "embxx/util/EventLoop.h"

namespace component {

using Position = int;

template<typename TEventLoop, typename TSSIMasterDevice, typename TReadHandler>
class SSIEncoder
{
	using SSIMasterDeviceFrameType = typename TSSIMasterDevice::FrameType;
	using EventLoopCtx = embxx::device::context::EventLoop;
	using InterruptCtx = embxx::device::context::Interrupt;

public:
	using EventLoop = TEventLoop;

	using SSIMasterDevice = TSSIMasterDevice;
	using FrameType = typename SSIMasterDevice::FrameType;

	using ReadHandler = TReadHandler;

	constexpr static auto MinResolution = SSIMasterDevice::MinFrameWidth;
	constexpr static auto MaxResolution = SSIMasterDevice::MaxFrameWidth;

	using ErrorCode = typename SSIMasterDevice::ErrorCode;

	//! Constructor
	SSIEncoder(EventLoop& eventLoop,
		SSIMasterDevice& ssiMasterDevice,
		std::size_t resolution)
		:	_eventLoop(eventLoop),
			_ssiMasterDevice(ssiMasterDevice)
	{
		// Check correctness of input values
		assert(resolution >= MinResolution
			&& resolution <= MaxResolution);

		_ssiMasterDevice.setCallback(
			[this](ErrorCode errorCode)
			{
				readComplete(errorCode);
			});

		// Change SSIMaster frame width only if needed
		const auto frameWidth = resolution;
		if(frameWidth != ssiMasterDevice.getFrameWidth())
		{
			ssiMasterDevice.setFrameWidth(resolution);
		}
	}

	template<typename TFunc>
	void asyncReadPosition(Position* position, TFunc&& func)
	{
		// Driver should be not busy
		assert(!isBusy());

		// Target position pointer should be non-null, so store it
		assert(position != nullptr);
		assert(_position == nullptr);
		_position = position;

		// Store provided handler
		_readHandler = std::forward<TFunc>(func);

		// Begin asynchronous read
		_ssiMasterDevice.startReadOne(&_frame, EventLoopCtx());
	}

	//! Reads encoder position value. Blocking call.
	void readPosition(Position& position, ErrorCode& errorCode)
	{
		// Driver should be not busy
		assert(!isBusy());

		// Read one frame from the device. Blocking call
		_ssiMasterDevice.readOne(_frame, errorCode);
		if(embxx::error::ErrorStatus(errorCode))
		{
			// Error occured during read operation
			return;
		}

		// Read success. Process received value and store result
		position = processFrame(_frame);
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

	bool isBusy()
	{
		return _ssiMasterDevice.isBusy();
	}

	EventLoop& getEventLoop()
	{
		return _eventLoop;
	}

private:
	void readComplete(ErrorCode errorCode)
	{
		if(!embxx::error::ErrorStatus(errorCode))
		{
			// Read completed without errors. Process received frame
			const auto position = processFrame(_frame);

			// Target position pointer should be non-null, so write to it
			assert(_position != nullptr);
			*_position = position;
		}

		_position = nullptr;

		// Post callback with appriopriate errorCode
		const auto postSuccess = _eventLoop.postInterruptCtx(
			[this, errorCode]()
			{
				// Read handler should be non-null, so invoke it with given error code
				assert(_readHandler);
				_readHandler(errorCode);
			});
		assert(postSuccess);
		static_cast<void>(postSuccess);
	}

	Position processFrame(const FrameType& frame)
	{
		// Only Gray-To-Binary implemented at the moment
		return etl::gray_to_binary(frame);
	}

	ReadHandler _readHandler;
	FrameType _frame;
	Position* _position = nullptr; //< Not owning pointer used in async operations
	EventLoop& _eventLoop;
	SSIMasterDevice& _ssiMasterDevice; //< Device handler
};

} // namespace component
