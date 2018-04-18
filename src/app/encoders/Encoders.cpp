#include "app/encoders/Encoders.hpp"

#include <cassert>

#include "tivaware/utils/uartstdio.h"
#include "embxx/error/ErrorStatus.h"

namespace app {
namespace encoders {

Encoders::Encoders()
	:	_encoder0SSIMasterDevice(DefaultBitRate, DefaultFrameWidth),
		_encoder1SSIMasterDevice(DefaultBitRate, DefaultFrameWidth),
		_encoder0(_encoder0SSIMasterDevice, DefaultResolution),
		_encoder1(_encoder1SSIMasterDevice, DefaultResolution)
{
	UARTprintf("[Encoders] ready\n");
}

void
Encoders::readPosition(Position& position, ErrorCode& ec)
{
	_encoder0.readPosition(position, ec);
	if(embxx::error::ErrorStatus(ec))
	{
		// error occured during reading the position
		// forward error code to the caller and return its previous position
		UARTprintf("[Encoders] could not read position, ec=%d\n",
			static_cast<int>(ec));
		position = _encoder0LastPosition;
		return;
	}

	// read position success
	// backup received position
	if(position != _encoder0LastPosition)
	{
		_encoder0LastPosition = position;
	}
}

} // namespace encoders
} // namespace app
