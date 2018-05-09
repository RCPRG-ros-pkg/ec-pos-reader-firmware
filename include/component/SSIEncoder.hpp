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
	using SSIMasterDeviceDataType = typename TSSIMasterDevice::DataType;
	using EventLoopCtx = embxx::device::context::EventLoop;
	using InterruptCtx = embxx::device::context::Interrupt;

public:
	using EventLoop = TEventLoop;

	using SSIMasterDevice = TSSIMasterDevice;
	using DataType = typename SSIMasterDevice::DataType;

	using ReadHandler = TReadHandler;

	constexpr static auto MinResolution = SSIMasterDevice::MinDataWidth;
	constexpr static auto MaxResolution = SSIMasterDevice::MaxDataWidth;

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

		_ssiMasterDevice.setReadHandler(
			[this](ErrorCode errorCode)
			{
				readComplete(errorCode);
			});

		// Change SSIMaster data width only if needed
		const auto dataWidth = resolution;
		if(dataWidth != ssiMasterDevice.getDataWidth())
		{
			ssiMasterDevice.setDataWidth(resolution);
		}

		// Postcondition, driver should not be busy
		assert(!isBusy());
	}

	//! Reads encoder position value asynchronously
	template<typename TFunc>
	void asyncReadPosition(Position* destPosition, TFunc&& func)
	{
		// Driver should not be busy
		assert(!isBusy());

		// Check correctness of input arguments
		assert(destPosition != nullptr);
		_destPosition = destPosition;

		// Store provided handler
		_readHandler = std::forward<TFunc>(func);

		// Begin asynchronous read
		_ssiMasterDevice.startReadOne(&_data, EventLoopCtx());
	}

	//! Reads encoder position value. Blocking call.
	void readPosition(Position& destPosition, ErrorCode& errorCode)
	{
		// Driver should not be busy
		assert(!isBusy());

		// Read one data item from the device. Blocking call
		_ssiMasterDevice.readOne(_data, errorCode, EventLoopCtx());
		if(embxx::error::ErrorStatus(errorCode))
		{
			// Error occured during read operation
			return;
		}

		// Read success. Process received value and store result
		destPosition = processData(_data);
	}

	//! Gets the resolution of encoder
	std::size_t getResolution() const
	{
		const auto dataWidth = _ssiMasterDevice.getDataWidth();

		const auto resolution = dataWidth;
		assert(resolution >= MinResolution
			&& resolution <= MaxResolution);

		return resolution;
	}

	//! Checks, if driver is busy or not
	bool isBusy()
	{
		return _ssiMasterDevice.isBusy(EventLoopCtx());
	}

	//! Returns reference to used EventLoop object
	EventLoop& getEventLoop()
	{
		return _eventLoop;
	}

private:
	//! Handler, which will be called by the device after async read
	void readComplete(ErrorCode errorCode)
	{
		if(!embxx::error::ErrorStatus(errorCode))
		{
			// Read completed without errors. Process received data
			const auto position = processData(_data);

			// Store received position.
			*_destPosition = position;
		}

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

	Position processData(const DataType& data)
	{
		// Only Gray-To-Binary implemented at the moment
		return etl::gray_to_binary(data);
	}

	ReadHandler _readHandler; //< Handler to be invoked after asyncReadPosition
	DataType _data; //< Buffer used in read operations
	Position* _destPosition = nullptr; //< Not owning pointer used in async operations
	EventLoop& _eventLoop;
	SSIMasterDevice& _ssiMasterDevice; //< Device handler
};

} // namespace component
