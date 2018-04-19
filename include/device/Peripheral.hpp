#pragma once

#include <cassert>
#include <cstdint>
#include "tivaware/driverlib/sysctl.h"

namespace device {

class PeripheralBase
{
protected:
	PeripheralBase(std::uint32_t id)
		:	_id(id)
	{
		assert(!SysCtlPeripheralReady(id)); // peripheral was not enabled (not used)
		SysCtlPeripheralEnable(id);
	}

public:
	~PeripheralBase()
	{
		assert(SysCtlPeripheralReady(_id)); // peripheral was enabled (used)
		SysCtlPeripheralDisable(_id);
	}

private:
	const std::uint32_t _id;
};

template<std::uint32_t TId>
class Peripheral
	:	public PeripheralBase
{
public:
	static_assert(TId > 0);
	constexpr static auto Id = TId;

	Peripheral()
		:	PeripheralBase(Id)
	{

	}
};

} // namespace device
