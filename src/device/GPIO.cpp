#include "device/GPIO.hpp"

namespace device {

void
GPIOBase::PinBase::write(bool state)
{
	const auto value = (state ? 0xFF : 0x00);
	HWREG(_baseAddress + (GPIO_O_DATA + (_mask << 2))) = value;
}

bool
GPIOBase::PinBase::read() const
{
	const auto value = (HWREG(_baseAddress + (GPIO_O_DATA + (_mask << 2))));
	return (value ? true : false);
}

} // namespace device
