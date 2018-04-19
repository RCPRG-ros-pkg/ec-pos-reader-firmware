/**
 * init.hpp
 * Contains declarations for initialization functions
 * Author: akowalew
 */

#pragma once

extern "C" {

constexpr auto ClockHz = 80000000;

//! configure clock
void preinitHardware();

//! configure IO
void initIO();

//! configure hardware peripherials
void initHardware();

} // extern "C"
