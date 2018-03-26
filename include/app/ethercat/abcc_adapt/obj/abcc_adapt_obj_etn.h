#pragma once


/*------------------------------------------------------------------------------
** Ethernet Object (0xF9)
**------------------------------------------------------------------------------
*/
/*
** Attribute 1: MAC Address (Array[6] of UINT8)
*/
#define ETN_IA_MAC_ADDRESS_ENABLE               TRUE
#define ETN_IA_MAC_ADDRESS_VALUE                "\x00\x30\x11\x11\xFF\xFF"

/*
** Attribute 2: Enable Host IP Configuration Protocol  (BOOL - TRUE/FALSE)
*/
#define ETN_IA_ENABLE_HICP_ENABLE               FALSE
#define ETN_IA_ENABLE_HICP_VALUE                TRUE

/*
** Attribute 3: Enable Web Server  (BOOL - TRUE/FALSE)
*/
#define ETN_IA_ENABLE_WEB_ENABLE                FALSE
#define ETN_IA_ENABLE_WEB_VALUE                 TRUE

/*
** Attribute 4: Enable ModbusTCP  (BOOL - TRUE/FALSE). Reserved in 40-series.
*/
#define ETN_IA_ENABLE_MOD_TCP_ENABLE            FALSE
#define ETN_IA_ENABLE_MOD_TCP_VALUE             TRUE

/*
** Attribute 5: Enable Web ADI access  (BOOL - TRUE/FALSE)
*/
#define ETN_IA_ENABLE_WEB_ADI_ACCESS_ENABLE     FALSE
#define ETN_IA_ENABLE_WEB_ADI_ACCESS_VALUE      TRUE

/*
** Attribute 6: Enable FTP server  (BOOL - TRUE/FALSE)
*/
#define ETN_IA_ENABLE_FTP_ENABLE                FALSE
#define ETN_IA_ENABLE_FTP_VALUE                 TRUE

/*
** Attribute 7: Enable admin mode  (BOOL - TRUE/FALSE)
*/
#define ETN_IA_ENABLE_ADMIN_MODE_ENABLE         FALSE
#define ETN_IA_ENABLE_ADMIN_MODE_VALUE          FALSE

/*
** Attribute 8: Network status  (UINT16)
*/
#define ETN_IA_NETWORK_STATUS_ENABLE            FALSE

/*
** Attribute 9: Port 1 MAC Address (Array[6] of UINT8)
*/
#define ETN_IA_PORT1_MAC_ADDRESS_ENABLE         TRUE
#define ETN_IA_PORT1_MAC_ADDRESS_VALUE          "\x00\x30\x11\x00\x00\x01"

/*
** Attribute 10: Port 2 MAC Address (Array[6] of UINT8)
*/
#define ETN_IA_PORT2_MAC_ADDRESS_ENABLE         TRUE
#define ETN_IA_PORT2_MAC_ADDRESS_VALUE          "\x00\x30\x11\x00\x00\x02"

/*
** Attribute 11: Enable Address Conflict Detection   (BOOL - TRUE/FALSE)
*/
#define ETN_IA_ENABLE_ACD_ENABLE                FALSE
#define ETN_IA_ENABLE_ACD_VALUE                 TRUE

/*
** Attribute 12: Enable Port 1 state   (ENUM)
*/
#define ETN_IA_PORT1_STATE_ENABLE               TRUE
#define ETN_IA_PORT1_STATE_VALUE                ABP_ETN_IA_PORT_STATE_ENABLE

/*
** Attribute 13: Enable Port 2 state   (ENUM)
*/
#define ETN_IA_PORT2_STATE_ENABLE               TRUE
#define ETN_IA_PORT2_STATE_VALUE                ABP_ETN_IA_PORT_STATE_ENABLE

/*
** Attribute 14: Enable web update  (BOOL - TRUE/FALSE).
*/
#define ETN_IA_ENABLE_WEB_UPDATE_ENABLE         FALSE
#define ETN_IA_ENABLE_WEB_UPDATE_VALUE          TRUE

/*
** Attribute 15: Enable HICP reset (BOOL - TRUE/FALSE)
*/
#define ETN_IA_ENABLE_HICP_RESET_ENABLE         FALSE
#define ETN_IA_ENABLE_HICP_RESET_VALUE          FALSE

/*
** Attribute 16: IP Configuration  (Struct of {UINT32, UINT32, UINT32})
*/
#define ETN_IA_IP_CONFIGURATION_ENABLE          FALSE

/*
** Attribute 17: IP address byte 0-2 (Array[3] of UINT8)
*/
#define ETN_IA_IP_ADDRESS_BYTE_0_2_ENABLE       FALSE
#define ETN_IA_IP_ADDRESS_BYTE_0_2_VALUE        "\xC0\xA8\x00"

#define ETN_OBJ_USE_SET_ATTR_SUCCESS_CALLBACK   FALSE
