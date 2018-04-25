#pragma once

#include <cstdint>
#include "tivaware/inc/hw_memmap.h"
#include "tivaware/inc/hw_gpio.h"
#include "tivaware/inc/hw_types.h"
#include "tivaware/driverlib/gpio.h"
#include "tivaware/driverlib/rom.h"
#include "tivaware/driverlib/rom_map.h"

namespace device {

class OutputPinBase
{
protected:
	//! Constructor
    OutputPinBase(std::uint32_t baseAddress, std::uint8_t pinMask);

public:
	//! Write state to pin
    void write(bool state);

    //! Read state of pin
    bool read();

private:
    const std::uint32_t _baseAddress;
    const std::uint8_t _pinMask;
};

template<std::uint32_t TBaseAddress, std::size_t TPinNumber>
class OutputPin
    :   public OutputPinBase
{
public:
    constexpr static std::uint32_t BaseAddress = TBaseAddress;
    static_assert(BaseAddress != 0);

    constexpr static std::size_t PinNumber = TPinNumber;
    static_assert(PinNumber >= 0 && PinNumber < 8);

    constexpr static std::uint8_t PinMask = (1 << PinNumber);

    OutputPin()
        :   OutputPinBase(BaseAddress, PinMask)
    {

    }
};

} // namespace device
