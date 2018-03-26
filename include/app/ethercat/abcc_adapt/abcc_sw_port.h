#pragma once

#include "abcc_td.h"
#include "stdio.h"
#include "string.h"

#include "tivaware/utils/uartstdio.h"
#include "tivaware/driverlib/interrupt.h"

//! IO print utility used in debugging
//! Invokes UARTprintf with provided args
#define ABCC_PORT_DebugPrint(args) \
	(UARTprintf args)

//! Should perform any preparation before EnterCritical and ExitCritical
//! In this case, does nothing
#define ABCC_PORT_UseCritical() \
	/* do nothing */

//! Enters critical section by globally disabling interrupts
#define ABCC_PORT_EnterCritical() \
	IntMasterDisable()

//! Exits critical section by re-enabling interrupts globally
#define ABCC_PORT_ExitCritical() \
	IntMasterEnable()

//! Performs low-level memory copy of bytes
//! Uses `memcpy` internally from the standard library
#define ABCC_PORT_MemCpy(dst, src, nbytes) \
   memcpy(dst, src, nbytes)

//! Copies octets(bytes) from source with offset to the destination with offset
#define ABCC_PORT_CopyOctets(dst, dstOffset, src, srcOffset, nbytes) \
    ABCC_PORT_MemCpy((void*)((UINT8*)(dst) + dstOffset), \
    	(void*)((UINT8*)(src) + srcOffset), nbytes)

//! Copies string from source with offset to the destination.
//! Doesn't take care about terminating null char
#define ABCC_PORT_StrCpyToNative(dst, src, srcOffset, nbytes) \
    ABCC_PORT_MemCpy((void*)(dst), (void*)((UINT8*)(src) + srcOffset), nbytes)

//! Copies string from source to the destination with offset.
//! Doesn't take care about terminating null char
#define ABCC_PORT_StrCpyToPacked(dst, dstOffset, src, nbytes) \
    ABCC_PORT_MemCpy((void*)((UINT8*)(dst + dstOffset)), src, nbytes)

//! Copies a byte from source with offset to the destination with offset
#define ABCC_PORT_Copy8(dst, dstOffset, src, srcOffset) \
    ((UINT8*)(dst))[dstOffset] = ((UINT8*)(src))[srcOffset]

//! Copies a word from source with offset to the destination with offset
//! Uses internaly ABCC_PORT_CopyOctets (with nbytes=2)
#define ABCC_PORT_Copy16( dst, dstOffset, src, srcOffset ) \
    ABCC_PORT_CopyOctets(dst, dstOffset, src, srcOffset, 2)

//! Copies a dword from source with offset to the destination with offset
//! Uses internally ABCC_PORT_CopyOctets (with nbytes=4)
#define ABCC_PORT_Copy32( dst, dstOffset, src, srcOffset ) \
    ABCC_PORT_CopyOctets(dst, dstOffset, src, srcOffset, 4)
