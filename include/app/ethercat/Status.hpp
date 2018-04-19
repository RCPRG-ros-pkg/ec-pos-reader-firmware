#pragma once

namespace app {
namespace ethercat {

enum class Status
{
   Success,
   ModuleNotDetected,
   ModuleNotAnswering,
   UnexpectedError
};

} // namespace ethercat
} // namespace app
