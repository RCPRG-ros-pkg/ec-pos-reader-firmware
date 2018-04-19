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

	const auto dataWidth = frameWidth + 2;
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
SSIMasterBase::readOne(FrameType& frame, ErrorCode& ec)
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

	processData(data, frame, ec);
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
	const auto dataWidth = frameWidth + 2;
	SSIDataWidthSet(_baseAddress, dataWidth);
}

//! Gets frame width in transmission with SSI slave
std::size_t
SSIMasterBase::getFrameWidth() const
{
	const auto dataWidth = SSIDataWidthGet(_baseAddress);
	const auto frameWidth = dataWidth - 2;
	return frameWidth;
}

//! Determines, whether SSI master is currently communicating with SSI slave
bool
SSIMasterBase::isBusy() const
{
	return SSIBusy(_baseAddress);
}

void
SSIMasterBase::processData(SSIDataType data,
	FrameType& frame, ErrorCode& ec)
{
	// Check state of MSB and LSB, to determine errors.
	// Typically, MSB should be set (steady clock HIGH)
	//  and LSB should be reset (SSI slave is waiting for timeout).
	const auto dataWidth = SSIDataWidthGet(_baseAddress);
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

	// check, if unused bits are zeros
	assert((data & ~((1 << msbBitIdx) - 1)) == 0);

	// save result
	frame = data;
	ec = ErrorCode::Success;
}

} // namespace device
