#pragma once

/*------------------------------------------------------------------------------
** CANopen Object (0xFB)
**------------------------------------------------------------------------------
*/
/*
** Attribute 1: Vendor ID (UINT32 - 0x00000000-0xFFFFFFFF)
*/
#define COP_IA_VENDOR_ID_ENABLE                 TRUE
#define COP_IA_VENDOR_ID_VALUE                  0xFFFFFFFF

/*
** Attribute 2: Product Code (UINT32 - 0x00000000-0xFFFFFFFF)
*/
#define COP_IA_PRODUCT_CODE_ENABLE              TRUE
#define COP_IA_PRODUCT_CODE_VALUE               0xFFFFFFFF

/*
** Attribute 3: Major Revision (UINT16 - 0x0000-0xFFFF)
*/
#define COP_IA_REV_MAJOR_ENABLE                 TRUE
#define COP_IA_REV_MAJOR_VALUE                  0xFFFF

/*
** Attribute 4: Minor Revision (UINT16 - 0x0000-0xFFFF)
*/
#define COP_IA_REV_MINOR_ENABLE                 TRUE
#define COP_IA_REV_MINOR_VALUE                  0xFFFF

/*
** Attribute 5: Serial Number (UINT32 - 0x00000000-0xFFFFFFFF)
*/
#define COP_IA_SERIAL_NUMBER_ENABLE             TRUE
#define COP_IA_SERIAL_NUMBER_VALUE              0xFFFFFFFF

/*
** Attribute 6: Manufacturer Device Name (Array of CHAR)
*/
#define COP_IA_MANF_DEV_NAME_ENABLE             TRUE
#define COP_IA_MANF_DEV_NAME_VALUE              "VSA encoders"

/*
** Attribute 7: Manufacturer Hardware Version (Array of CHAR)
*/
#define COP_IA_MANF_HW_VER_ENABLE               TRUE
#define COP_IA_MANF_HW_VER_VALUE                "1.0.0"

/*
** Attribute 8: Manufacturer Software Version (Array of CHAR)
*/
#define COP_IA_MANF_SW_VER_ENABLE               TRUE
#define COP_IA_MANF_SW_VER_VALUE                "1.0.0"
