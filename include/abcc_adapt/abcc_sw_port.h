/*******************************************************************************
********************************************************************************
**                                                                            **
** ABCC Starter Kit version 3.04.01 (2018-01-23)                              **
**                                                                            **
** Delivered with:                                                            **
**    ABP            7.55.01 (2017-12-20)                                     **
**    ABCC Driver    5.04.01 (2017-12-18)                                     **
**                                                                            */
/*******************************************************************************
********************************************************************************
** COPYRIGHT NOTIFICATION (c) 2015 HMS Industrial Networks AB                 **
**                                                                            **
** This code is the property of HMS Industrial Networks AB.                   **
** The source code may not be reproduced, distributed, or used without        **
** permission. When used together with a product from HMS, permission is      **
** granted to modify, reproduce and distribute the code in binary form        **
** without any restrictions.                                                  **
**                                                                            **
** THE CODE IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY KIND. HMS DOES NOT    **
** WARRANT THAT THE FUNCTIONS OF THE CODE WILL MEET YOUR REQUIREMENTS, OR     **
** THAT THE OPERATION OF THE CODE WILL BE UNINTERRUPTED OR ERROR-FREE, OR     **
** THAT DEFECTS IN IT CAN BE CORRECTED.                                       **
********************************************************************************
********************************************************************************
** Platform dependent macros and functions required by the ABCC driver and
** Anybus objects implementation to be platform independent.
** The description of the macros are found in abcc_port.h. Abcc_port.h is found
** in the public ABCC40 driver interface.
********************************************************************************
********************************************************************************
** Services:
**    ABCC_PORT_DebugPrint()           - Print debug info
**    ABCC_PORT_UseCritical()          - Used if any preparation is needed
**                                       before calling
**                                       "ABCC_PORT_EnterCritical()".
**    ABCC_PORT_EnterCritical()        - Disables all interrupts, if they are
**                                       not already disabled.
**    ABCC_PORT_ExitCritical()         - Restore interrupts to the state they
**                                       were in when
**                                       "ABCC_PORT_EnterCritical()" was called.
**    ABCC_PORT_MemCpy()               - Port of standard memcpy()
**    ABCC_PORT_StrCpyToNative()       - Copy native char string to octet string
**    ABCC_PORT_StrCpyToPacked()       - Copy octetstring to native char* string
**    ABCC_PORT_CopyOctets()           - Copy octet aligned buffer.
**    ABCC_PORT_Copy8()                - Copy octet aligned 8 bit data.
**    ABCC_PORT_Copy16()               - Copy octet aligned 16 bit data.
**    ABCC_PORT_Copy32()               - Copy octet aligned 32 bit data.
**    ABCC_PORT_Copy64()               - Copy octet aligned 64 bit data.
********************************************************************************
********************************************************************************
*/

#ifndef ABCC_SW_PORT_H_
#define ABCC_SW_PORT_H_

#include "abcc_td.h"
#include "stdio.h"
#include "string.h"

#include "tivaware/utils/uartstdio.h"
#include "tivaware/driverlib/interrupt.h"

//! IO print utility used in debugging
#define ABCC_PORT_DebugPrint( args ) (UARTprintf args)

//! Should perform any preparation before EnterCritical and ExitCritical
//! In this case, does nothing
#define ABCC_PORT_UseCritical()

//! Enters critical section by globally disabling interrupts
#define ABCC_PORT_EnterCritical() IntMasterDisable()

//! Exits critical section by re-enabling interrupts globally
#define ABCC_PORT_ExitCritical() IntMasterEnable()

//! Performs low-level memory copy of bytes
#define ABCC_PORT_MemCpy( pbDest, pbSource, iNbrOfOctets )                     \
   memcpy( pbDest, pbSource, iNbrOfOctets );

#define ABCC_PORT_CopyOctets( pxDest, iDestOctetOffset, pxSrc, iSrcOctetOffset,\
                              iNumOctets )                                     \
        ABCC_PORT_MemCpy( (void*)&( (UINT8*)(pxDest) )[ iDestOctetOffset ],    \
                          (void*)&( (UINT8*)(pxSrc) )[ iSrcOctetOffset ],      \
                          iNumOctets )

#define ABCC_PORT_StrCpyToNative( pxDest, pxSrc, iSrcOctetOffset, iNbrOfChars )\
        ABCC_PORT_MemCpy( (void*)(pxDest),                                     \
                          (void*)&( (UINT8*)(pxSrc) )[ iSrcOctetOffset ],      \
                          iNbrOfChars )

#define ABCC_PORT_StrCpyToPacked( pxDest, iDestOctetOffset, pxSrc,             \
                                  iNbrOfChars )                                \
        ABCC_PORT_MemCpy( (void*)&( (UINT8*)(pxDest) )[ iDestOctetOffset ],    \
                           pxSrc, iNbrOfChars )

#define ABCC_PORT_Copy8( pxDest, iDestOctetOffset, pxSrc, iSrcOctetOffset )    \
        ( (UINT8*)(pxDest) )[ iDestOctetOffset ] =                             \
        ( (UINT8*)(pxSrc) )[ iSrcOctetOffset ]

#define ABCC_PORT_Copy16( pxDest, iDestOctetOffset, pxSrc, iSrcOctetOffset )   \
        ABCC_PORT_CopyOctets( pxDest, iDestOctetOffset, pxSrc, iSrcOctetOffset, 2 )

#define ABCC_PORT_Copy32( pxDest, iDestOctetOffset, pxSrc, iSrcOctetOffset )   \
        ABCC_PORT_CopyOctets( pxDest, iDestOctetOffset, pxSrc, iSrcOctetOffset, 4 )

#endif  /* inclusion lock */
