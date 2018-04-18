#include "device/SSIMaster.hpp"

#include <algorithm>

#include "init.hpp"

namespace device {

/**
 * @brief Constructor
 * @details [long description]
 *
 * @param ssi [description]
 * @param bitRate [description]
 * @param frameWidth [description]
 */
SSIMasterBase::SSIMasterBase(std::uint32_t baseAddress,
	int bitRate, std::size_t frameWidth)
	:	_baseAddress(baseAddress)
{
	assert(frameWidth >= MinFrameWidth
		&& frameWidth <= MaxFrameWidth);

	const auto dataWidth = frameWidth + 1;
	SSIConfigSetExpClk(_baseAddress,
		ClockHz,
		SSI_FRF_MOTO_MODE_2,
		SSI_MODE_MASTER,
		bitRate,
		dataWidth);
}

/**
 * @brief Reads one data item from SSI slave in blocking way
 * @details [long description]
 * @return [description]
 */
void
SSIMasterBase::readOne(FrameType& frame, ErrorCode& errorCode)
{
	assert(!SSIEnabled(_baseAddress));
	assert(SSIRxEmpty(_baseAddress));
	assert(SSITxEmpty(_baseAddress));

	// insert one dummy data item into Tx FIFO
	SSIDataPutNow(_baseAddress, DummyFrame);

	// start transmission of CLK signal and receive data from slave
	SSIEnable(_baseAddress);

	// wait for one data item to be received
	SSIDataType data;
	SSIDataGet(_baseAddress, &data);

	// end transmission
	SSIDisable(_baseAddress);

	// Check state of MSB, to determine errors
	// position of MSB is (dataWidth - 1) => frameWidth
	const auto msbBitIdx = getFrameWidth();
	const auto isMsbReset = ((data & (1 << msbBitIdx)) == 0);
	if(isMsbReset)
	{
		// MSB is reset, so protocol error occured
		errorCode = ErrorCode::HwProtocolError;
		return;
	}

	// MSB is set, so everything OK.
	// Clear MSB in datagram
	data &= ~(1 << msbBitIdx);

	// check, if unused bits are zeros
	assert((data & (1 << msbBitIdx)) == 0);

	// save result
	frame = data;
	errorCode = ErrorCode::Success;
}

/**
 * @brief Reads multiple data from SSI slave in blocking way
 * @details [long description]
 *
 * @param buffer [description]
 * @param size [description]
 * @param n [description]
 */
std::size_t
SSIMasterBase::read(etl::array_view<FrameType> buffer, std::size_t n,
	ErrorCode& errorCode)
{
	static_cast<void>(errorCode);

	assert(n > 0);
	assert(!SSIEnabled(_baseAddress));
	assert(SSIRxEmpty(_baseAddress));
	assert(SSITxEmpty(_baseAddress));

	if(n <= SSI_FIFO_SIZE)
	{
		// there are as much data to receive as FIFO capacity
		// simply fill Tx FIFO and wait for Rx data

		// fill Tx FIFO first with dummy chars
		for(unsigned i = 0; i < n; ++i)
		{
			SSIDataPutNow(_baseAddress, DummyFrame);
		}

		// start transmission of CLK signal and receive data from slave
		SSIEnable(_baseAddress);

		// generate Rx data
		for(auto& data : buffer)
		{
			SSIDataGet(_baseAddress, &data);
		}
	}
	else
	{
		// there are more data to receive than FIFO capacity
		// Tx FIFO must be complemented during receive operations

		// fill entire Tx FIFO first with dummy chars
		for(unsigned i = 0; i < SSI_FIFO_SIZE; ++i)
		{
			SSIDataPutNow(_baseAddress, DummyFrame);
		}

		// start transmission of CLK signal and receive data from slave
		SSIEnable(_baseAddress);

		// generate Rx data
		auto elapsed = n - SSI_FIFO_SIZE;
		for(auto& data : buffer)
		{
			// wait for one data item to be received
			SSIDataGet(_baseAddress, &data);

			// queue elapsed CLK transmissions
			if(elapsed--)
			{
				SSIDataPutNow(_baseAddress, DummyFrame);
			}
		}
	}

	// stop transmission
	SSIDisable(_baseAddress);

	return 0;
}

//! Sets bit rate of transmission with SSI slave
void
SSIMasterBase::setBitRate(int bitRate)
{
	SSIBitRateSet(_baseAddress, ClockHz, bitRate);
}

//! Gets bit rate of transmission with SSI slave
int
SSIMasterBase::getBitRate() const
{
	return SSIBitRateGet(_baseAddress, ClockHz);
}

//! Sets frame width in transmission with SSI slave
void
SSIMasterBase::setFrameWidth(std::size_t frameWidth)
{
	assert(frameWidth >= MinFrameWidth
		&& frameWidth <= MaxFrameWidth);
	const auto dataWidth = frameWidth + 1;
	SSIDataWidthSet(_baseAddress, dataWidth);
}

//! Gets frame width in transmission with SSI slave
std::size_t
SSIMasterBase::getFrameWidth() const
{
	const auto dataWidth = SSIDataWidthGet(_baseAddress);
	const auto frameWidth = dataWidth - 1;
	return frameWidth;
}

//! Determines, whether SSI master is currently communicating with SSI slave
bool
SSIMasterBase::isBusy() const
{
	return SSIBusy(_baseAddress);
}

} // namespace device
