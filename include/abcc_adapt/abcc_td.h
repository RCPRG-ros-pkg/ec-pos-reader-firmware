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
** Definition of ABCC types
********************************************************************************
********************************************************************************
*/

#ifndef ABCC_TD_H
#define ABCC_TD_H

#include <stdint.h>
#include <stdbool.h>

//! Target system is little endian
/* #define ABCC_SYS_BIG_ENDIAN  */

//! Target system has 8bit char type
/* #define ABCC_SYS_16_BIT_CHAR */

//! Typedefs for basic types
typedef bool     BOOL;
typedef uint8_t  BOOL8;
typedef uint8_t  UINT8;
typedef int8_t   INT8;
typedef uint16_t UINT16;
typedef int16_t  INT16;
typedef uint32_t UINT32;
typedef int32_t  INT32;
typedef float    FLOAT32;

//! Typedefs for LittleEndian types
typedef INT16     LeINT16;
typedef INT32     LeINT32;
typedef UINT16    LeUINT16;
typedef UINT32    LeUINT32;

//! Typedefs for BigEndian types
typedef INT16     BeINT16;
typedef INT32     BeINT32;
typedef UINT16    BeUINT16;
typedef UINT32    BeUINT32;

//! Values for boolean constants
#ifndef FALSE
   #define FALSE     false
#endif
#ifndef TRUE
   #define TRUE      ( !FALSE )
#endif

//! Value of null pointer
#ifndef NULL
#define NULL 0
#endif

#ifdef __cplusplus
   #define CPLUSPLUS
#endif

#ifdef CPLUSPLUS
   #define EXTFUNC extern "C"
#else
   #define EXTFUNC extern
#endif

#endif  /* inclusion lock */
