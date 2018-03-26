#pragma once

#include <cassert>
#include <cstdint>
#include "tivaware/driverlib/sysctl.h"

namespace device {

class PeripheralBase
{
public:

protected:
	PeripheralBase(std::uint32_t id)
		:	_id(id)
	{
		assert(!SysCtlPeripheralReady(id)); // peripheral was not enabled (not used)
		SysCtlPeripheralEnable(id);
	}

	~PeripheralBase()
	{
		assert(SysCtlPeripheralReady(_id)); // peripheral was enabled (used)
		SysCtlPeripheralDisable(_id);
	}

private:
	const std::uint32_t _id;
};

template<std::uint32_t TID>
class Peripheral
	:	public PeripheralBase
{
public:
	static_assert(TID > 0);
	constexpr static auto ID = TID;

	Peripheral()
		:	PeripheralBase(ID)
	{

	}
};

} // namespace device
