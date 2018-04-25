#include "device/OutputPin.hpp"

#include "binary.h" // ETLCPP

namespace device {

OutputPinBase::OutputPinBase(std::uint32_t baseAddress, std::uint8_t pinMask)
	:	_baseAddress(baseAddress),
		_pinMask(pinMask)
{
	assert(_baseAddress != 0);
	assert(etl::count_bits(_pinMask) == 1);
	GPIOPinTypeGPIOOutput(_baseAddress, _pinMask);
}

void
OutputPinBase::write(bool state)
{
	const auto value = (state ? _pinMask : 0x00);
	GPIOPinWrite(_baseAddress, _pinMask, value);
}

bool
OutputPinBase::read()
{
	const auto value = GPIOPinRead(_baseAddress, _pinMask);
	return (value ? true : false);
}


} // namespace device
