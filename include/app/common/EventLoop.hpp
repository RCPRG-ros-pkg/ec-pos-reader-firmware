#pragma once

#include "embxx/util/EventLoop.h"

#include "app/common/Mutex.hpp"
#include "app/common/ConditionVariable.hpp"

namespace app {
namespace common {

constexpr static auto EventLoopSize = 1024U;
using EventLoop = embxx::util::EventLoop<
	EventLoopSize, Mutex, ConditionVariable>;

} // namespace common
} // namespace app
