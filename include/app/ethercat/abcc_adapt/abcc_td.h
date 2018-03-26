#pragma once

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
   #define TRUE      (!FALSE)
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
