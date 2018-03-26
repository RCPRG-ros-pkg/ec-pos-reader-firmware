#pragma once

#include <cstdint>
#include "tivaware/inc/hw_memmap.h"
#include "tivaware/inc/hw_gpio.h"
#include "tivaware/inc/hw_types.h"
#include "tivaware/driverlib/gpio.h"
#include "tivaware/driverlib/rom.h"
#include "tivaware/driverlib/rom_map.h"

#include "device/Peripheral.hpp"

namespace device {

class GPIOBase
{
public:
	class PinBase
	{
	public:
		void write(bool state);

		bool read() const;

		void setAsDigitalOutput()
		{
			MAP_GPIOPinTypeGPIOOutput(_baseAddress, _mask);
		}

		void setAsDigitalInput()
		{
			MAP_GPIOPinTypeGPIOInput(_baseAddress, _mask);
		}

	protected:
		PinBase(std::uint8_t mask, GPIOBase& gpio)
			:	_baseAddress(gpio.getBaseAddress()),
				_mask(mask)
		{

		}

	private:
		const std::uint32_t _baseAddress;
		const std::uint8_t _mask;
	} __attribute__((__packed__)); // unaligned access possible

	// check, if structure is right packed
	static_assert(sizeof(PinBase)
		==	sizeof(std::uint32_t) + sizeof(std::uint8_t));

	template<std::size_t TPosition>
	class Pin
		:	public PinBase
	{
	public:
		static_assert(TPosition >= 0 && TPosition < 8);
		static constexpr auto Position = TPosition;
		static constexpr std::uint8_t Mask = (1 << Position);

		Pin(GPIOBase& gpio)
			:	PinBase(Mask, gpio)
		{

		}
	};

	std::uint32_t getBaseAddress() const
	{
		return _baseAddress;
	}

protected:
	//! Constructor
	GPIOBase(std::uint32_t baseAddress)
		:	_baseAddress(baseAddress)
	{

	}

private:
	const std::uint32_t _baseAddress;
};

template<std::uint32_t TPeriphID, std::uint32_t TBaseAddress>
class GPIO
	:	public Peripheral<TPeriphID>,
		public GPIOBase
{
public:
	using PeripheralType = Peripheral<TPeriphID>;

	static_assert(TBaseAddress > 0);
	constexpr static auto BaseAddress = TBaseAddress;

	//! Constructor
	GPIO()
		:	PeripheralType::Peripheral(),
			GPIOBase(BaseAddress)
	{

	}
};

using GPIOA = GPIO<SYSCTL_PERIPH_GPIOA, GPIO_PORTA_BASE>;
using GPIOB = GPIO<SYSCTL_PERIPH_GPIOB, GPIO_PORTB_BASE>;
using GPIOC = GPIO<SYSCTL_PERIPH_GPIOC, GPIO_PORTC_BASE>;
using GPIOD = GPIO<SYSCTL_PERIPH_GPIOD, GPIO_PORTD_BASE>;
using GPIOE = GPIO<SYSCTL_PERIPH_GPIOE, GPIO_PORTE_BASE>;
using GPIOF = GPIO<SYSCTL_PERIPH_GPIOF, GPIO_PORTF_BASE>;

} // namespace device
