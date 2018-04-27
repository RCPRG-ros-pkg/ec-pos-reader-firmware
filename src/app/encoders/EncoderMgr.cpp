#include "app/encoders/EncoderMgr.hpp"

#include <cassert>

#include "tivaware/utils/uartstdio.h"
#include "embxx/error/ErrorStatus.h"

namespace app {
namespace encoders {

EncoderMgr::EncoderMgr(EventLoop& _eventLoop)
	:	_encoders(_eventLoop)
{
}

void
EncoderMgr::captureInputs(Position& position, ErrorCode& errorCode)
{
	std::get<0>(_encoders).captureInputs(position, errorCode);
}

/**
 * @brief Returns, whether module is in active state or not
 * @details [long description]
 *
 * @param get_encoders [description]
 */
bool
EncoderMgr::isActive() const
{
	return std::get<0>(_encoders).isActive();
}

} // namespace encoders
} // namespace app
