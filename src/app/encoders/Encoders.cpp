#include "app/encoders/Encoders.hpp"

#include "tivaware/utils/uartstdio.h"

#include <cassert>

namespace app {
namespace encoders {

Encoders::Encoders()
	:	_encoder0SSIMasterDevice(),
		_encoder1SSIMasterDevice(),
		_ssiEncoder0(_encoder0SSIMasterDevice,
			DefaultResolution, DefaultBitRate),
		_ssiEncoder1(_encoder1SSIMasterDevice,
			DefaultResolution, DefaultBitRate)
{
	UARTprintf("[Encoders] ready\n");
}

int
Encoders::readPosition()
{
	return _ssiEncoder0.readPosition();
}

} // namespace encoders
} // namespace app
