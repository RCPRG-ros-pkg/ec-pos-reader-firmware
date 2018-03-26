#include "app/encoders/Encoders.hpp"

#include "tivaware/utils/uartstdio.h"

namespace app {
namespace encoders {

Encoders::Encoders(common::SysTickDriver& sysTickDriver)
	:	_timer(sysTickDriver.allocTimer()),
		_ssiMasterDevice(),
		_encoder(_ssiMasterDevice)
{
	assert(_timer.isValid());

	UARTprintf("[Encoders] initialized\n");
}

void
Encoders::startReading()
{
	readPosition();
}

void
Encoders::readPosition()
{
	const auto position = _encoder.readPosition();
	if(position != _position)
	{
		_position = position;
		UARTprintf("[Encoders] position: %d\n", _position.value());
	}

	_timer.asyncWait(std::chrono::milliseconds(1),
		[this](const auto& errorStatus)
		{
			if(errorStatus)
			{
				UARTprintf("[Encoders] error during wait, code=%d\n",
					static_cast<int>(errorStatus.code()));
				return;
			}

			readPosition();
		});
}

} // namespace encoders
} // namespace app
