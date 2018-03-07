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
** COPYRIGHT NOTIFICATION (c) 2017 HMS Industrial Networks AB                 **
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
** User configuration of the Anybus objects implementation.
**
** All configurations in this file should be possible to override if defined
** in abcc_platform_cfg.h. This allows the target platform to adapt the
** product configuration to realize its implementation for specific needs.
********************************************************************************
********************************************************************************
** Services:
********************************************************************************
********************************************************************************
*/

#ifndef ABCC_OBJ_CFG_H_
#define ABCC_OBJ_CFG_H_

#include "abcc_drv_cfg.h"

//! Enabled network objects
#define ECT_OBJ_ENABLE                          TRUE // EtherCAT
#define COP_OBJ_ENABLE                          TRUE // CanOpen

//! Disabled network objects
#define CFN_OBJ_ENABLE                          FALSE
#define EPL_OBJ_ENABLE                          FALSE
#define PRT_OBJ_ENABLE                          FALSE
#define CCL_OBJ_ENABLE                          FALSE
#define EIP_OBJ_ENABLE                          FALSE
#define MOD_OBJ_ENABLE                          FALSE
#define DEV_OBJ_ENABLE                          FALSE
#define DPV1_OBJ_ENABLE                         FALSE

//! Enabled host objects
#define ETN_OBJ_ENABLE                          TRUE // Ethernet
#define APP_OBJ_ENABLE                          TRUE // Application

//! Disabled host objects
#define SAFE_OBJ_ENABLE                         FALSE
#define SYNC_OBJ_ENABLE                         FALSE
#define OPCUA_OBJ_ENABLE                        FALSE

#include "obj/abcc_adapt_obj_ect.h"
#include "obj/abcc_adapt_obj_cop.h"

#include "obj/abcc_adapt_obj_etn.h"
#include "obj/abcc_adapt_obj_sync.h"
#include "obj/abcc_adapt_obj_app.h"

#endif  /* inclusion lock */
