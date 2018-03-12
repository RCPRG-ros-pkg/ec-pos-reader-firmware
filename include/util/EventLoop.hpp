#pragma once

#include "embxx/util/EventLoop.h"

#include "util/Mutex.hpp"
#include "util/ConditionVariable.hpp"

namespace util {

constexpr static auto EventLoopSize = 1024U;
using EventLoop = embxx::util::EventLoop<
	EventLoopSize, Mutex, ConditionVariable>;

} // namespace util
