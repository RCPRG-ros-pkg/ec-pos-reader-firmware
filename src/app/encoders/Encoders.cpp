#include "app/encoders/Encoders.hpp"

#include "tivaware/utils/uartstdio.h"

#include <cassert>

namespace app {
namespace encoders {

Encoders::Encoders()
	:	_ssiMasterDevice(),
		_ssiEncoder(_ssiMasterDevice)
{
	UARTprintf("[Encoders] ready\n");
}

int
Encoders::readPosition()
{
	return _ssiEncoder.readPosition();
}

} // namespace encoders
} // namespace app
