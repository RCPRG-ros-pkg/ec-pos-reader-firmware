#include "device/SSIMaster.hpp"

#include <algorithm>

namespace device {

/**
 * @brief Constructor
 * @details [long description]
 *
 * @param ssi [description]
 * @param bitRate [description]
 * @param dataWidth [description]
 */
SSIMasterBase::SSIMasterBase(std::uint32_t baseAddress,
	int bitRate, std::size_t dataWidth)
	:	_baseAddress(baseAddress)
{
	SSIConfigSetExpClk(_baseAddress,
		ClockHz,
		SSI_FRF_TI,
		SSI_MODE_MASTER,
		bitRate,
		dataWidth);
}

/**
 * @brief Constructor
 * @details [long description]
 *
 * @param ssi [description]
 * @param e [description]
 * @param h [description]
 */
SSIMasterBase::SSIMasterBase(std::uint32_t baseAddress)
	:	SSIMasterBase(baseAddress, DefaultBitRate, DefaultDataWidth)
{

}

/**
 * @brief Reads one data item from SSI slave in blocking way
 * @details [long description]
 * @return [description]
 */
SSIMasterBase::DataType
SSIMasterBase::readOne()
{
	assert(!SSIEnabled(_baseAddress));
	assert(SSIRxEmpty(_baseAddress));
	assert(SSITxEmpty(_baseAddress));

	// insert one dummy data item into Tx FIFO
	SSIDataPutNow(_baseAddress, DummyData);

	// start transmission of CLK signal and receive data from slave
	SSIEnable(_baseAddress);

	// wait for one data item to be received
	DataType data;
	SSIDataGet(_baseAddress, &data);

	// end transmission
	SSIDisable(_baseAddress);

	return data;
}

/**
 * @brief Reads multiple data from SSI slave in blocking way
 * @details [long description]
 *
 * @param buffer [description]
 * @param size [description]
 * @param n [description]
 */
void
SSIMasterBase::read(etl::array_view<DataType> buffer, std::size_t n)
{
	assert(buffer != nullptr);
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
			SSIDataPutNow(_baseAddress, DummyData);
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
			SSIDataPutNow(_baseAddress, DummyData);
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
				SSIDataPutNow(_baseAddress, DummyData);
			}
		}
	}

	// stop transmission
	SSIDisable(_baseAddress);
}

} // namespace device
