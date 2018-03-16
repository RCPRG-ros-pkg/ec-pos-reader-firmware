#pragma once

#include "abcc_td.h"
#include "abp.h"

//! Enable only SPI driver
#define ABCC_CFG_DRV_PARALLEL                      (FALSE)
#define ABCC_CFG_DRV_SPI                           (TRUE)
#define ABCC_CFG_DRV_SERIAL                        (FALSE)
#define ABCC_CFG_DRV_PARALLEL_30                   (FALSE)

//! Disable getting or setting operating mode by switched.
//! Instead fix operating mode to SPI
#define ABCC_CFG_OP_MODE_GETTABLE                  (FALSE)
#define ABCC_CFG_OP_MODE_SETTABLE                  (FALSE)
#define ABCC_CFG_ABCC_OP_MODE_40 ABP_OP_MODE_SPI

//! Configure SPI message fragment length.
//! Most of small messages are under 32 octets
#define ABCC_CFG_SPI_MSG_FRAG_LEN                  (32)

//! Enable module ID checking from MI0 and MI1 pins
#define ABCC_CFG_MODULE_ID_PINS_CONN               (TRUE)

//! Enable module detection checking from MD0 pin
#define ABCC_CFG_MOD_DETECT_PINS_CONN              (TRUE)

//! Configure message handling internals
#define ABCC_CFG_MAX_NUM_APPL_CMDS                 (2)
#define ABCC_CFG_MAX_NUM_ABCC_CMDS                 (2)
#define ABCC_CFG_MAX_MSG_SIZE                      (255)
#define ABCC_CFG_MAX_PROCESS_DATA_SIZE             (512)

//! Disable both SYNC and usage of SYNC signal
#define ABCC_CFG_SYNC_ENABLE                    (TRUE)
#define ABCC_CFG_USE_ABCC_SYNC_SIGNAL           (TRUE)

//! Enable interrupts from IRQ pin
#define ABCC_CFG_POLL_ABCC_IRQ_PIN                 (FALSE)
#define ABCC_CFG_INT_ENABLED                       (TRUE)
// #define ABCC_CFG_INT_ENABLE_MASK_SPI            ()
#define ABCC_CFG_HANDLE_INT_IN_ISR_MASK (0)

//! Configure anybus watchdog to be called every 1 second
#define ABCC_CFG_WD_TIMEOUT_MS                     (1000)

//! Disable remapping
#define ABCC_CFG_REMAP_SUPPORT_ENABLED             (FALSE)

//! Configuration of ADI
#define ABCC_CFG_STRUCT_DATA_TYPE                  (FALSE)
#define ABCC_CFG_ADI_GET_SET_CALLBACK              (FALSE)
#define ABCC_CFG_64BIT_ADI_SUPPORT                 (FALSE)

//! Command sequencer configuration
#define ABCC_CFG_MAX_NUM_CMD_SEQ                      (4)
#define ABCC_CFG_CMD_SEQ_MAX_NUM_RETRIES              (0)

//! Enable all debug prints
#define ABCC_CFG_ERR_REPORTING_ENABLED             (TRUE)
#define ABCC_CFG_DEBUG_EVENT_ENABLED               (TRUE)
#define ABCC_CFG_DEBUG_ERR_ENABLED                 (TRUE)
#define ABCC_CFG_DEBUG_MESSAGING                   (TRUE)
#define ABCC_CFG_DEBUG_CMD_SEQ_ENABLED             (FALSE)

//! Set startup timeout to be 1.5 seconds.
//! After that, if driver is still not responding, APPL_MODULE_NOT_ANSWERING
//! will be returned
#define ABCC_CFG_STARTUP_TIME_MS           (1500)
