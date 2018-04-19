/**
 * ssi.hpp
 * Utilities, extending TI Driverlib library SysTick module.
 * Adds funtions not present in SysTick module, like SysTickClear etc
 * Author: akowalew
 */

#pragma once

#include <cstdint>
#include <cassert>
#include "tivaware/inc/hw_types.h"
#include "tivaware/inc/hw_nvic.h"
#include "tivaware/inc/hw_memmap.h"
#include "tivaware/driverlib/systick.h"

void SysTickClear();

inline void
SysTickClear()
{
    HWREG(NVIC_ST_CURRENT) = 0; // dummy write to clear sys-tick and pending interrupt
}
