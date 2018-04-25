#include "app/encoders/Encoders.hpp"

#include <cassert>

#include "tivaware/utils/uartstdio.h"
#include "embxx/error/ErrorStatus.h"

namespace app {
namespace encoders {

Encoders::Encoders()
	:	_encodersList()
{
}

void
Encoders::captureInputs(Position& position, ErrorCode& errorCode)
{
	std::get<0>(_encodersList).captureInputs(position, errorCode);
}

/**
 * @brief Returns, whether module is in active state or not
 * @details [long description]
 *
 * @param get_encodersList [description]
 */
bool
Encoders::isActive() const
{
	return std::get<0>(_encodersList).isActive();
}

} // namespace encoders
} // namespace app
