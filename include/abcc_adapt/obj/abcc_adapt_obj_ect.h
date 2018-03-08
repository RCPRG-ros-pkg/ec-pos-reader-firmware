#pragma once

/*------------------------------------------------------------------------------
** EtherCAT Object (0xF5)
**------------------------------------------------------------------------------
*/
/*
** Attribute 1: Vendor ID (UINT32 - 0x00000000-0xFFFFFFFF)
*/
#define ECT_IA_VENDOR_ID_ENABLE                 TRUE
#define ECT_IA_VENDOR_ID_VALUE                  0xFFFFFFFF

/*
** Attribute 2: Product Code type (UINT32 - 0x00000000-0xFFFFFFFF)
*/
#define ECT_IA_PRODUCT_CODE_ENABLE              TRUE
#define ECT_IA_PRODUCT_CODE_VALUE               0xFFFFFFFF

/*
** Attribute 3: Revision High Word (UINT16 - 0x0000-0xFFFF)
*/
#define ECT_IA_MAJOR_REV_ENABLE                 TRUE
#define ECT_IA_REVISION_HW_VALUE                0xFFFF

/*
** Attribute 4: Revision Low Word (UINT16 - 0x0000-0xFFFF)
*/
#define ECT_IA_MINOR_REV_ENABLE                 TRUE
#define ECT_IA_REVISION_LW_VALUE                0xFFFF

/*
** Attribute 5: Serial number (UINT32 - function/variable/0x00000000-0xFFFFFFFF)
*/
#define ECT_IA_SERIAL_NUMBER_ENABLE             TRUE
#define ECT_IA_SERIAL_NUMBER_VALUE              0x12345678

/*
** Attribute 6: Manufacturer device name (Array of CHAR)
*/
#define ECT_IA_MANF_DEVICE_NAME_ENABLE          TRUE
#define ECT_IA_MANF_DEVICE_NAME_VALUE           "VSA encoders"

/*
** Attribute 7: Manufacturer hardware version (Array of CHAR)
*/
#define ECT_IA_MANF_HW_VERSION_ENABLE           TRUE
#define ECT_IA_MANF_HW_VERSION_VALUE            "FF"

/*
** Attribute 8: Manufacturer software version (Array of CHAR)
*/
#define ECT_IA_MANF_SW_VERSION_ENABLE           TRUE
#define ECT_IA_MANF_SW_VERSION_VALUE            "FF"

/*
** Attribute 9: ENUM ADIs (Array of UINT16 - {0x0001-0xFFFF})
*/
#define ECT_IA_ENUM_ADIS_ENABLE                 FALSE
#define ECT_IA_ENUM_ADIS_VALUE                  { 0x1357 }
#define ECT_IA_ENUM_ADIS_ARRAY_SIZE             1

/*
** Attribute 10: Device type (UINT32 - 0x00000000-0xFFFFFFFF)
*/
#define ECT_IA_DEVICE_TYPE_ENABLE               TRUE
#define ECT_IA_DEVICE_TYPE_VALUE                0xFFFFFFFF

/*
** Attribute 11: Write PD assembly instance translation
** (Array of UINT16 - {0x1A00-0x1BFF})
*/
#define ECT_IA_WR_PD_ASSY_INST_TRANS_ENABLE     FALSE
#define ECT_IA_WR_PD_ASSY_INST_TRANS_VALUE      { 0x1BFF }
#define ECT_IA_WR_PD_ASSY_INST_TRANS_SIZE       1

/*
** Attribute 12: Read PD assembly instance translation
** (Array of UINT16 - {0x1600-0x17FF})
*/
#define ECT_IA_RD_PD_ASSY_INST_TRANS_ENABLE     FALSE
#define ECT_IA_RD_PD_ASSY_INST_TRANS_VALUE      { 0x17FF }
#define ECT_IA_RD_PD_ASSY_INST_TRANS_SIZE       1

/*
** Attribute 13: ADI translation (Array of (Struct of {UINT16, UINT16}))
*/
#define ECT_IA_ADI_TRANS_ENABLE                 FALSE
#define ECT_IA_ADI_TRANS_VALUE                  { { 0xE000, 0x10F8 } }
#define ECT_IA_ADI_TRANS_SIZE                   1

/*
** Attribute 15: Object subindex translation
** (Array of (Struct of {UINT16, UINT16, UINT8 }))
*/
#define ECT_IA_OBJ_SUB_TRANS_ENABLE             FALSE
#define ECT_IA_OBJ_SUB_TRANS_VALUE              { { 0xE000, 0x10F3, 0x01 } }
#define ECT_IA_OBJ_SUB_TRANS_SIZE               1

/*
** Attribute 16: Enable FoE (BOOL - TRUE/FALSE)
*/
#define ECT_IA_ENABLE_FOE_ENABLE                FALSE
#define ECT_IA_ENABLE_FOE_VALUE                 TRUE

/*
** Attribute 17: Enable EoE (BOOL - TRUE/FALSE)
*/
#define ECT_IA_ENABLE_EOE_ENABLE                TRUE
#define ECT_IA_ENABLE_EOE_VALUE                 TRUE

/*
** Attribute 19: Set Device ID as Configured Station Alias (BOOL - TRUE/FALSE)
*/
#define ECT_IA_SET_DEV_ID_AS_CSA_ENABLE         FALSE
#define ECT_IA_SET_DEV_ID_AS_CSA_VALUE          FALSE

/*
** Attribute 20: EtherCAT state
*/
#define ECT_IA_ETHERCAT_STATE_ENABLE            TRUE

/*
** Attribute 21: State Transition Timeouts (Array of UINT32)
*/
#define ECT_IA_STATE_TIMEOUTS_ENABLE            FALSE
#define ECT_IA_STATE_TIMEOUTS_VALUE             { 1000, 5000, 1000, 200 }

/*
** Attribute 22: Compare Identity Lists (BOOL - TRUE/FALSE)
*/
#define ECT_IA_COMP_IDENT_LISTS_ENABLE          FALSE
#define ECT_IA_COMP_IDENT_LISTS_VALUE           TRUE

/*
** Attribute 23: FSoE Status Indicator
*/
#define ECT_IA_FSOE_STATUS_IND_ENABLE           FALSE

/*
** Attribute 24: Clear IdentALSts (BOOL - TRUE/FALSE)
*/
#define ECT_IA_CLEAR_IDENT_AL_STS_ENABLE        FALSE
#define ECT_IA_CLEAR_IDENT_AL_STS_VALUE         FALSE
