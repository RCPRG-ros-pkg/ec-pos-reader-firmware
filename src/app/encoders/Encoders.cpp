#include "app/encoders/Encoders.hpp"

#include "tivaware/utils/uartstdio.h"

#include <cassert>

namespace app {
namespace encoders {

Encoders::Encoders(common::SysTickDriver& sysTickDriver)
	:	_timer(sysTickDriver.allocTimer()),
		_ssiMasterDevice(),
		_encoder(_ssiMasterDevice)
{
	assert(_timer.isValid());

	UARTprintf("[Encoders] ready\n");
	assert(_moduleState == ModuleState::Ready);
}

void
Encoders::start()
{
	if(_moduleState == ModuleState::Active)
	{
		return; // nothing to do
	}

	UARTprintf("[Encoders] starting...\n");

	_moduleState = ModuleState::Active;
	_running = true;
	execWait();

	UARTprintf("[Encoders] started\n");
	assert(_moduleState == ModuleState::Active);
	assert(_running);
}

void
Encoders::stop()
{
	if(_moduleState != ModuleState::Active)
	{
		return;
	}

	UARTprintf("[Encoders] stopping...\n");

	_running = false; // signal stop request

	assert(!_running);
}

void
Encoders::execWait()
{
	assert(_moduleState == ModuleState::Active);

	_timer.asyncWait(std::chrono::milliseconds(1),
		[this](const auto& errorStatus)
		{
			assert(!errorStatus); // no errors will occur
			static_cast<void>(errorStatus);

			if(_running)
			{
				// keep reading
				readPosition();
				execWait();
			}
			else
			{
				// stop request signalled
				_moduleState = ModuleState::Ready;
				UARTprintf("[Encoders] stopped\n");
			}
		});
}

void
Encoders::readPosition()
{
	const auto position = _encoder.readPosition();
	if(position != _position)
	{
		_position = position;
		UARTprintf("[Encoders] position changed: %d\n",
			_position.value());
	}
}

} // namespace encoders
} // namespace app
