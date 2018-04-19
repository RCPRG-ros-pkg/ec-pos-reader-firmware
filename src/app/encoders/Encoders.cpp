#include "app/encoders/Encoders.hpp"

#include <cassert>

#include "tivaware/utils/uartstdio.h"
#include "embxx/error/ErrorStatus.h"

namespace app {
namespace encoders {

Encoders::Encoders(RedLEDPin redLEDPin, GreenLEDPin greenLEDPin)
	:	_ssiMasterDevice(DefaultBitRate, DefaultFrameWidth),
		_redLEDPin(redLEDPin),
		_greenLEDPin(greenLEDPin),
		_ssiEncoder(_ssiMasterDevice, DefaultResolution),
		_redLED(_redLEDPin),
		_greenLED(_greenLEDPin)
{
	_redLEDPin.setAsDigitalOutput();
	_greenLEDPin.setAsDigitalOutput();

	detectEncoder();

	assert(_status != Status::Init);
	assert(_redLED.isTurnedOn()
		|| _greenLED.isTurnedOn());
}

void
Encoders::captureInputs(Position& position, ErrorCode& errorCode)
{
	assert(_status == Status::Active);
	assert(_redLED.isTurnedOff());
	assert(_greenLED.isTurnedOn());

	_ssiEncoder.readPosition(position, errorCode);
	if(embxx::error::ErrorStatus(errorCode))
	{
		// Error occured during reading the position.
		handleReadError(position, errorCode);
		return;
	}

	// Read position success. If changed, save current encoder position.
	if(position != _lastPosition)
	{
		_lastPosition = position;
	}
}

void
Encoders::handleReadError(Position& position, ErrorCode& errorCode)
{
	assert(_status == Status::Active);
	assert(_redLED.isTurnedOff());
	assert(_greenLED.isTurnedOn());
	assert(_maxReadRetries >= 0);
	assert(_readRetries >= 0 && _readRetries < _maxReadRetries);
	if(++_readRetries == _maxReadRetries)
	{
		// The number of retries has been exceeded.
		UARTprintf("[Encoders] read position error after %d retries, errorCode=%d\n",
			_maxReadRetries, static_cast<int>(errorCode));

		// Change status of module into failed
		_status = Status::Failed;
		_greenLED.turnOff();
		_redLED.turnOn();
		return;
	}

	// The number of retries has been not exceeded.
	// Return the last known correct position to the caller and signal success
	position = _lastPosition;
	errorCode = ErrorCode::Success;
}

void
Encoders::detectEncoder()
{
	assert(_redLED.isTurnedOff());
	assert(_greenLED.isTurnedOff());
	UARTprintf("[Encoders] detecting encoder...\n");

	// Read position and check for errors to determine connection
	Position position;
	ErrorCode errorCode;
	_ssiEncoder.readPosition(position, errorCode);
	if(embxx::error::ErrorStatus(errorCode))
	{
		// Read position error. Assume, that encoder is not connected.
		UARTprintf("[Encoders] encoder not connected, error code: %d\n",
			errorCode);

		// Change status of module into failed
		_status = Status::Failed;
		_redLED.turnOn();
		return;
	}

	// Read position success, encoder ready. Change status to active.
	UARTprintf("[Encoders] encoder connected, position: %d\n",
		position);
	UARTprintf("[Encoders] active\n");
	_status = Status::Active;
	_greenLED.turnOn();
	return;
}

/**
 * @brief Returns, whether module is in fail state or not
 */
bool
Encoders::isFailed() const
{
	return (_status == Status::Failed);
}

/**
 * @brief Returns, whether module is in active state or not
 */
bool
Encoders::isActive() const
{
	return (_status == Status::Active);
}

} // namespace encoders
} // namespace app
