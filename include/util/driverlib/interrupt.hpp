#pragma once

#include <stdint.h>
#include <stddef.h>
#include <assert.h>
#include "tivaware/inc/hw_types.h"
#include "tivaware/inc/hw_memmap.h"
#include "tivaware/inc/hw_ints.h"
#include "tivaware/inc/hw_nvic.h"
#include "tivaware/driverlib/interrupt.h"
#include "tivaware/driverlib/debug.h"

void IntGeneralEnable(uint32_t ui32Interrupt);
void IntGeneralDisable(uint32_t ui32Interrupt);
bool IntGeneralEnabledGet(uint32_t ui32Interrupt);
bool IntGeneralPendGet(uint32_t ui32Interrupt);
void IntUserDataSet(uint32_t ui32Interrupt, void* pvUserData);
void IntUserDataUnset(uint32_t ui32Interrupt);
void* IntUserDataGet(uint32_t ui32Interrupt);

inline void
IntUserDataSet(uint32_t ui32Interrupt, void* pvUserData)
{
	ASSERT(ui32Interrupt < NUM_INTERRUPTS);

	extern void* g_pvUserDataArray[NUM_INTERRUPTS];
	g_pvUserDataArray[ui32Interrupt] = pvUserData;
}

inline void
IntUserDataUnset(uint32_t ui32Interrupt)
{
	ASSERT(ui32Interrupt < NUM_INTERRUPTS);

	extern void* g_pvUserDataArray[NUM_INTERRUPTS];
	g_pvUserDataArray[ui32Interrupt] = NULL;
}

inline void*
IntUserDataGet(uint32_t ui32Interrupt)
{
	ASSERT(ui32Interrupt < NUM_INTERRUPTS);

	extern void* g_pvUserDataArray[NUM_INTERRUPTS];
	void* pvUserData = g_pvUserDataArray[ui32Interrupt];
	return pvUserData;
}

inline void
IntGeneralEnable(uint32_t ui32Interrupt)
{
	ASSERT(ui32Interrupt >= 16
		&& ui32Interrupt < NUM_INTERRUPTS);

	HWREG(NVIC_EN0 + 4 * ((ui32Interrupt - 16) / 32)) =
            (1 << ((ui32Interrupt - 16) & 31));
}

inline void
IntGeneralDisable(uint32_t ui32Interrupt)
{
	ASSERT(ui32Interrupt >= 16
		&& ui32Interrupt < NUM_INTERRUPTS);

	HWREG(NVIC_DIS0 + 4 * ((ui32Interrupt - 16) / 32)) =
            (1 << ((ui32Interrupt - 16) & 31));
}

inline bool
IntGeneralEnabledGet(uint32_t ui32Interrupt)
{
	ASSERT(ui32Interrupt >= 16
		&& ui32Interrupt < NUM_INTERRUPTS);

	return HWREG(NVIC_DIS0 + 4 * ((ui32Interrupt - 16) / 32)) &
            (1 << ((ui32Interrupt - 16) & 31));
}

inline bool
IntGeneralPendGet(uint32_t ui32Interrupt)
{
	ASSERT(ui32Interrupt >= 16
		&& ui32Interrupt < NUM_INTERRUPTS);

	return HWREG(NVIC_PEND0 + 4 * ((ui32Interrupt - 16) / 32)) &
        (1 << ((ui32Interrupt - 16) & 31));
}
