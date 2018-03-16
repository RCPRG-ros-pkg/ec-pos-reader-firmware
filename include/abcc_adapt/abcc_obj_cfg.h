#pragma once

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
#define SYNC_OBJ_ENABLE                         TRUE

//! Disabled host objects
#define SAFE_OBJ_ENABLE                         FALSE
#define OPCUA_OBJ_ENABLE                        FALSE

#include "obj/abcc_adapt_obj_ect.h"
#include "obj/abcc_adapt_obj_cop.h"

#include "obj/abcc_adapt_obj_etn.h"
#include "obj/abcc_adapt_obj_sync.h"
#include "obj/abcc_adapt_obj_app.h"
