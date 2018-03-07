/**
 * init.hpp
 * Contains declarations for initialization functions
 * Author: akowalew
 */

#pragma once

extern "C" {

//! configure clock
void preinitHardware();

//! configure IO
void initIO();

//! configure hardware peripherials
void initHardware();

} // extern "C"
