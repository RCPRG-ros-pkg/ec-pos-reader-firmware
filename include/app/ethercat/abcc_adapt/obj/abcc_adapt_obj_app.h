#pragma once

/*------------------------------------------------------------------------------
** Application data Object (0xFE)
**------------------------------------------------------------------------------
*/
/*
** These defines shall be set to the max number of process data mapping entries
** that will be required by the implementation.
** Note that each mapping entry represents a 'range' of elements from one ADI,
** meaning that if only some elements from a multi-element ADI are to be mapped
** it will require as many mapping entries as there are separate and non-
** continuous ranges of elements to map.
** Do not forget to consider remap scenarios if ABCC_CFG_REMAP_SUPPORT_ENABLED
** is enabled in abcc_drv_cfg.h.
*/
#define AD_MAX_NUM_WRITE_MAP_ENTRIES             ( 64 )
#define AD_MAX_NUM_READ_MAP_ENTRIES              ( 64 )

/*
** Attributes 5, 6, 7: Min, max and default attributes  - (BOOL - TRUE/FALSE)
**
** Enabling this will also enable and include functions that performs runtime
** min/max range checks for 'SetAttribute' operations targeting ADI elements,
** which will increase code ROM consumption.
** If disabled no range checks will be made, and the min/max will be the full
** range of each data type.
*/
#define AD_IA_MIN_MAX_DEFAULT_ENABLE            ( FALSE )

/*------------------------------------------------------------------------------
** Application Object (0xFF)
**------------------------------------------------------------------------------
*/
/*
** Attribute 1: Configured (BOOL - TRUE/FALSE)
*/
#define APP_IA_CONFIGURED_ENABLE                TRUE
#define APP_IA_CONFIGURED_VALUE                 FALSE

/*
** Attribute 2: Supported languages (Array of ENUM - {ABP_LANG_*})
*/
#define APP_IA_SUP_LANG_ENABLE                  TRUE
#define APP_IA_SUP_LANG_VALUE                   { ABP_LANG_ENG }
#define APP_IA_SUP_LANG_ARRAY_SIZE              1

/*
** Attribute 3: Serial number (UINT32 - function/variable/0x00000000-0xFFFFFFFF)
*/
#define APP_IA_SER_NUM_ENABLE                   FALSE
#define APP_IA_SER_NUM_VALUE                    0

/*
** Attribute 4: Parameter control sum (Array[16] of UINT8, {0x00-0xFF})
*/
#define APP_IA_PAR_CRTL_SUM_ENABLE              FALSE
#define APP_IA_PAR_CRTL_SUM_VALUE               { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }

/*
** Attribute 5: Candidate firmware available (BOOL - TRUE/FALSE)
*/
#define APP_IA_FW_AVAILABLE_ENABLE              TRUE
#define APP_IA_FW_AVAILABLE_VALUE               FALSE

/*
** Attribute 6: Hardware configurable address (BOOL - TRUE/FALSE)
*/
#define APP_IA_HW_CONF_ADDR_ENABLE              TRUE
#define APP_IA_HW_CONF_ADDR_VALUE               FALSE
