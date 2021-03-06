#pragma once

/*------------------------------------------------------------------------------
** Sync Object (0xEE)
**------------------------------------------------------------------------------
*/
/*
** Attribute 1: Cycle time (UINT32 - 0x00000000-0xFFFFFFFF)
*/
#define SYNC_IA_CYCLE_TIME_ENABLE               TRUE
#define SYNC_IA_CYCLE_TIME_VALUE                0L

/*
** Attribute 2: Output valid (UINT32 - 0x00000000-0xFFFFFFFF)
*/
#define SYNC_IA_OUTPUT_VALID_ENABLE             TRUE
#define SYNC_IA_OUTPUT_VALID_VALUE              0

/*
** Attribute 3: Input capture (UINT32 - 0x00000000-0xFFFFFFFF)
*/
#define SYNC_IA_INPUT_CAPTURE_ENABLE            TRUE
#define SYNC_IA_INPUT_CAPTURE_VALUE             0

/*
** Attribute 4: Output processing (UINT32 - 0x00000000-0xFFFFFFFF)
*/
#define SYNC_IA_OUTPUT_PROCESSING_ENABLE        TRUE

#ifndef NDEBUG
# define SYNC_IA_OUTPUT_PROCESSING_VALUE         11000L
#else
# define SYNC_IA_OUTPUT_PROCESSING_VALUE         700L
#endif

//
// ** Attribute 5: Input processing (UINT32 - 0x00000000-0xFFFFFFFF)
//
#define SYNC_IA_INPUT_PROCESSING_ENABLE         TRUE

#ifndef NDEBUG
# define SYNC_IA_INPUT_PROCESSING_VALUE          250000L
#else
# define SYNC_IA_INPUT_PROCESSING_VALUE          175000L
#endif

/*
** Attribute 6: Min cycle time (UINT32 - 0x00000000-0xFFFFFFFF)
*/
#define SYNC_IA_MIN_CYCLE_TIME_ENABLE           TRUE

#ifndef NDEBUG
# define RDPDI_TO_SYNC_DELAY                     150000L
#else
# define RDPDI_TO_SYNC_DELAY                     80000L
#endif
#define SYNC_IA_MIN_CYCLE_TIME_VALUE             (RDPDI_TO_SYNC_DELAY + SYNC_IA_INPUT_PROCESSING_VALUE)
/*
** Attribute 7: Sync mode (UINT16 - 0: Nonsynchronous operation
**                                  1: Synchronous operation)
*/
#define SYNC_IA_SYNC_MODE_ENABLE                TRUE
#define SYNC_IA_SYNC_MODE_VALUE                 0x0001

/*
** Attribute 8: Supported sync modes (UINT16 - Bit 0: 1 = Nonsynchronous mode supported
**                                             Bit 1: 1 = Synchronous mode supported
**                                             Bit 2-15: Reserved)
*/
#define SYNC_IA_SUPPORTED_SYNC_MODES_ENABLE     TRUE
#define SYNC_IA_SUPPORTED_SYNC_MODES_VALUE      0x0003

