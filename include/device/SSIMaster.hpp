#pragma once

#include <cstdint>
#include <type_traits>
#include <limits>
#include <cassert>

#include "array_view.h"

#include "embxx/util/StaticFunction.h"
#include "embxx/device/context.h"
#include "embxx/error/ErrorCode.h"
#include "embxx/error/ErrorStatus.h"

#include "util/driverlib/ssi.hpp"
#include "util/driverlib/interrupt.hpp"

#include "init.hpp"

#include "device/Peripheral.hpp"

namespace device {

template<std::uint32_t TBaseAddress, std::uint32_t TId, std::uint32_t TIntNumber>
class SSIMaster
	:	public Peripheral<TId>
{
public:
	constexpr static auto BaseAddress = TBaseAddress;
	static_assert(BaseAddress != 0,
		"Specified BaseAddress is invalid");

	constexpr static std::uint32_t IntNumber = TIntNumber;
	static_assert(IntNumber < NUM_INTERRUPTS,
		"Specified IntNumber is invalid");

	static constexpr std::size_t MinDataWidth = (SSI_MIN_DATA_WIDTH - 2);
	static constexpr std::size_t MaxDataWidth = (SSI_MAX_DATA_WIDTH - 2);
	static_assert(MinDataWidth < MaxDataWidth,
		"Invalid relation between MinDataWidth and MaxDataWidth");

	using DataType = SSIDataType;
	static_assert(std::numeric_limits<DataType>::digits >= MaxDataWidth,
		"Underlying data type must have at least MaxDataWidth bits size");

	using ErrorCode = embxx::error::ErrorCode; //< Error code using in Read operations
	using EventLoopCtx = embxx::device::context::EventLoop;
	using InterruptCtx = embxx::device::context::Interrupt;

	/**
	 * @brief Constructor
	 * @details [long description]
	 *
	 * @param ssi [description]
	 * @param bitRate [description]
	 * @param dataWidth [description]
	 */
	SSIMaster(int bitRate, std::size_t dataWidth)
		:	Peripheral<TId>::Peripheral()
	{
		// Interrupts should be locked
		assert(IntGeneralEnabledGet(IntNumber) == false);

		// Check corectness of input parameters
		assert(dataWidth >= MinDataWidth
			&& dataWidth <= MaxDataWidth);
		assert(bitRate > 0);

		// Be sure, that during construction SSI is disabled
		assert(!SSIIsEnabled(BaseAddress));

		// Be sure, that during construction SSI Int flags are zeroed
		assert(SSIIntEnabledGet(BaseAddress) == 0);

		// Configure SSI module to work as true SSI master
		const auto realDataWidth = dataWidth + 2;
		SSIConfigSetExpClk(BaseAddress,
			ClockHz,
			SSI_FRF_MOTO_MODE_2,
			SSI_MODE_MASTER,
			bitRate,
			realDataWidth);

		// Enable EndOfTransmission signalling,
		//  because it will be used to invoke interrupt after data receive
		SSIEOTEnable(BaseAddress);

		// Register interrupt handler and set user data pointer (to this object)
		IntRegister(IntNumber, ssiISR);
		IntUserDataSet(IntNumber, static_cast<void*>(this));

		// After construction, the device should be not busy
		assert(!isBusy(InterruptCtx()));

		// Unlock interrupts
		IntGeneralEnable(IntNumber);
	}

	/**
	 * @brief Destructor
	 * @details [long description]
	 */
	~SSIMaster()
	{
		// Lock interrupts
		IntGeneralDisable(IntNumber);

		// During destruction, device should be idle
		assert(!isBusy(InterruptCtx()));

		// Unregister interrupt handler and unset user data pointer
		IntUserDataUnset(IntNumber);
		IntUnregister(IntNumber);
	}

	/**
	 * @brief Sets callback for asynchronous operations
	 * @details It should be called from event loop context, but the callback
	 *  itself should be called in interrupt context.
	 *
	 * @param handler [description]
	 */
	template<typename THandler>
	void setReadHandler(THandler&& handler)
	{
		// Lock interupts and check, that device should not be busy
		assert(!isBusy(EventLoopCtx()));

		// Store provided handler
		_readHandler = std::forward<THandler>(handler);
	}

	/**
	 * @brief Starts read of one data item asynchronously in event loop context
	 * @details [long description]
	 *
	 * @param data [description]
	 * @param x [description]
	 */
	void
	startReadOne(DataType* data, EventLoopCtx)
	{
		// Lock interrupts and check, that device should not be busy.
		assert(!isBusy(EventLoopCtx()));

		// Start read, as in the interrupt context
		startReadOne(data, InterruptCtx());
	}

	/**
	 * @brief Starts read of one data item asynchronously in interrupt context
	 * @details [long description]
	 *
	 * @param destData [description]
	 * @param x [description]
	 */
	void
	startReadOne(DataType* destData, InterruptCtx)
	{
		// Device should not be busy
		assert(!isBusy(InterruptCtx()));

		// Pointer to destData should be non-null, so store it
		assert(destData != nullptr);
		_destData = destData;

		// Both queues should be empty, so insert one dummy data item to Tx FIFO
		assert(SSIRxEmpty(BaseAddress));
		assert(SSITxEmpty(BaseAddress));
		SSIDataPutNow(BaseAddress, DummyData);

		// There should be no pending interrupts for TX and EOT,
		//  and SSI interrupts should be disabled, so enable them
		assert((SSIRawIntStatus(BaseAddress) & SSI_TXFF) == 0);
		assert(SSIIntEnabledGet(BaseAddress) == 0);
		SSIIntEnable(BaseAddress, SSI_TXFF);

		// SSI should be now disabled, so enable it
		assert(!SSIIsEnabled(BaseAddress));
		SSIEnable(BaseAddress);
	}

	/**
	 * @brief Reads one data item from SSI slave in blocking way in event loop context.
	 * @details [long description]
	 *
	 * @param data [description]
	 * @param ec [description]
	 */
	void
	readOne(DataType& destData, ErrorCode& ec, EventLoopCtx)
	{
		// Lock interrupts and check, that device should not be busy
		assert(!isBusy(EventLoopCtx()));

		// Read one as in the interrupt context
		readOne(destData, ec, InterruptCtx());
	}

	/**
	 * @brief Reads one data item from SSI slave in blocking way in interrupt context.
	 * @details [long description]
	 *
	 * @param destData [description]
	 * @param ec [description]
	 * @param x [description]
	 */
	void
	readOne(DataType& destData, ErrorCode& ec, InterruptCtx)
	{
		// Device should not be busy
		assert(!isBusy(InterruptCtx()));

		// Both queues should be empty, so insert one dummy data item to Tx FIFO
		assert(SSIRxEmpty(BaseAddress));
		assert(SSITxEmpty(BaseAddress));
		SSIDataPutNow(BaseAddress, DummyData);

		// SSI Interrupts should be disabled, because this is blocking call
		assert(SSIIntEnabledGet(BaseAddress) == 0);

		// SSI should be disabled, so enable it
		assert(!SSIIsEnabled(BaseAddress));
		SSIEnable(BaseAddress);

		// Wait for one data item to be received
		SSIDataType data;
		SSIDataGet(BaseAddress, &data);

		// SSI should be enabled, so disable it
		assert(SSIIsEnabled(BaseAddress));
		SSIDisable(BaseAddress);

		// Process received data, detect errors
		processData(data, destData, ec);
	}

	/**
	 * @brief Checks, if device is busy in event loop context
	 * @details [long description]
	 * @return [description]
	 */
	bool
	isBusy(EventLoopCtx)
	{
		// Lock interrupts
		IntGeneralDisable(IntNumber);

		// Check if busy, as in the interrupt context
		const auto busy = isBusy(InterruptCtx());

		// Unlock interrupts
		IntGeneralEnable(IntNumber);

		return busy;
	}

	//! Sets bit rate of transmission with SSI slave
	void
	setBitRate(int bitRate)
	{
		// Lock interrupts and check, that device should not be busy.
		// It should not be busy, because it is forbidden
		//  to modify bit rate, when SSI is enabled.
		assert(!isBusy(EventLoopCtx()));

		// Check correctness of input arguments
		assert(bitRate > 0);

		// Update the bit rate
		SSIBitRateSet(BaseAddress, ClockHz, bitRate);
	}

	//! Gets bit rate of transmission with SSI slave
	int
	getBitRate() const
	{
		// There is no need to lock interrupts, because
		//  this attribute is not modified in the ISR

		return SSIBitRateGet(BaseAddress, ClockHz);
	}

	//! Sets data width in transmission with SSI slave
	void
	setDataWidth(std::size_t dataWidth)
	{
		// Lock interrupts and check, that device should not be busy.
		// It should not be busy, because 'dataWidth' is a shared resource.
		assert(!isBusy(EventLoopCtx()));

		// Check correctness of input arguments
		assert(dataWidth >= MinDataWidth
			&& dataWidth <= MaxDataWidth);

		// Update the data width
		const auto realDataWidth = dataWidth + 2;
		SSIDataWidthSet(BaseAddress, realDataWidth);
	}

	//! Gets data width in transmission with SSI slave
	std::size_t
	getDataWidth() const
	{
		// There is no need to lock interrupts, because
		//  this attribute is not modified in the ISR

		const auto realDataWidth = SSIDataWidthGet(BaseAddress);
		const auto dataWidth = realDataWidth - 2;
		return dataWidth;
	}

private:
	//! Dummy data item to place in TxFIFO, used only to invoke CLK transmission
	constexpr static auto DummyData = DataType();

	//! Alias for static function type
	template<typename T, std::size_t TSize>
	using Function = embxx::util::StaticFunction<T, TSize>;

	//! Alias for read handler function. Will store only 'this' pointer
	using ReadHandler = Function<void(ErrorCode), 1 * sizeof(void*)>;

	/**
	 * @brief Checks, if device is busy in interrupt context
	 * @details [long description]
	 *
	 * @param  [description]
	 * @return [description]
	 */
	bool
	isBusy(InterruptCtx)
	{
		if(SSIIsEnabled(BaseAddress))
		{
			// Right after disabling interrupts, SSI was still enabled,
			//  so ISR was not yet invoked (which could disable the SSI),
			//  so callback was not yet invoked,
			//  so device's state is still "busy"
			return true;
		}

		// After disabling the interrupts, SSI is disabled,
		//  so ISR was invoked (which has disabled the SSI),
		//  so callback was invoked,
		//  so device's state is "not busy", "idle"
		return false;
	}

	/**
	 * @brief SSI ISR handling procedure
	 * @details [long description]
	 *
	 * @param  [description]
	 */
	void handleISR(InterruptCtx)
	{
		// Device should be busy since interrupt was fired
		assert(isBusy(InterruptCtx()));

		// Valid interrupt should occur
		// NOTE1: There is no need to clear SSI_TXFF interrupt
		// NOTE2: following line is commented because of bug in TM4C123 chip.
		//  more info at errata: http://www.ti.com/lit/pdf/spmz849, bug SSI#07
		// assert(SSIMaskedIntStatus(BaseAddress) == SSI_TXFF);

		// SSI interrupts should be still enabled, so disable them
		assert(SSIIntEnabledGet(BaseAddress) == SSI_TXFF);
		SSIIntDisable(BaseAddress, SSI_TXFF);

		// SSI interrupt should be not pending more
		assert(IntGeneralPendGet(BaseAddress) == false);

		// SSI should be still enabled and idle, so disable it
		assert(SSIIsEnabled(BaseAddress));
		assert(SSIIdle(BaseAddress));
		SSIDisable(BaseAddress);

		// Rx FIFO should have only one data item, so read it
		assert(SSIRxNotEmpty(BaseAddress));
		const auto data = SSIDataGetNow(BaseAddress);
		assert(SSIRxEmpty(BaseAddress));

		// Process received data, detect errors
		DataType destData;
		ErrorCode errorCode;
		processData(data, destData, errorCode);
		if(errorCode == ErrorCode::Success)
		{
			// No errors detected in received data.
			// Rx buffer should be non-null, so store received data to it
			assert(_destData != nullptr);
			*_destData = destData;
		}
		else
		{
			// Received data contains errors
			// Since error code is set, there are no more things to do
		}

		// Read handler should be non-null, so invoke it with given error code
		assert(_readHandler);
		_readHandler(errorCode);
	}

	/**
	 * @brief Interrupt Service Routine
	 * @details [long description]
	 */
	static void ssiISR()
	{
		// Retrieve stored UserData pointer (pointing to this object),
		//  and cast it to this object type
		using ThisType = SSIMaster<BaseAddress, TId, IntNumber>;
		auto userData = IntUserDataGet(IntNumber);
		auto instance = static_cast<ThisType*>(userData);

		// Obtained pointer should be non-null, so invoke ISR handler on it
		assert(instance != nullptr);
		instance->handleISR(InterruptCtx());
	}

	/**
	 * @brief Processes received SSI datagram. Informs about errors
	 * @details [long description]
	 *
	 * @param data [description]
	 * @param destData [description]
	 * @param ec [description]
	 */
	void
	processData(SSIDataType data,
		DataType& destData, ErrorCode& ec)
	{
		// Check state of MSB and LSB, to determine errors.
		// Typically, MSB will be set (steady clock HIGH)
		//  and LSB will be reset (SSI slave is waiting for timeout).
		const auto dataWidth = SSIDataWidthGet(BaseAddress);
		const auto msbBitIdx = (dataWidth - 1);
		if(const auto isMSBReset = ((data & (1 << msbBitIdx)) == 0); isMSBReset)
		{
			// MSB is reset, so protocol error occured
			ec = ErrorCode::HwProtocolError;
			return;
		}
		else if(const auto isLSBSet = ((data & (1 << 0)) != 0); isLSBSet)
		{
			// MSB is set, so protocol error occured
			ec = ErrorCode::HwProtocolError;
			return;
		}

		// Clear MSB in datagram and shift right by one to ignore LSB
		data &= ~(1 << msbBitIdx);
		data >>= 1;

		// Unused bits should be all zeros
		assert((data & ~((1 << msbBitIdx) - 1)) == 0);

		// Save the result and signal correctness of received data
		destData = data;
		ec = ErrorCode::Success;
	}

	// Private members
	ReadHandler _readHandler; //< Read handler for async operations
	DataType* _destData = nullptr; //< Non owning pointer to receive buffer for async operations
};

} // namespace device
