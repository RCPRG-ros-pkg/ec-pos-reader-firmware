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
** This is an example implementation of the application abcc handler.
** It includes the following section:
** 1. ADI definition and default mappings
** 2. ABCC driver callback implementations (process data, default mapping ...)
** 3. State machine implementation for message sequencing, e.g. during user init
**    or reading exception state
** 4. ABCC handler main function to be called from the main loop, including a main
**    state machine for handling startup and restart of the ABCC.
********************************************************************************
********************************************************************************
*/

#include <stdio.h>
#include "abcc_td.h"
#include "abp.h"
#include "abcc.h"
#include "abcc_cmd_seq_if.h"
#include "ad_obj.h"
#include "abcc_port.h"

#include "ad_obj.h"         /* Application data object:   254                 */
#include "app_obj.h"        /* Application object:        255                 */
#include "appl_abcc_handler.h"
#include "abcc_port.h"
#include "etn_obj.h"
#include "opcua_obj.h"
#include "sync_obj.h"
#include "safe_obj.h"

#include "eip.h"
#include "prt.h"
#include "epl.h"
#include "dpv1.h"
#include "ect.h"
#include "dev.h"
#include "mod.h"
#include "cop.h"
#include "ccl.h"
#include "cfn.h"

#include "abcc_obj_cfg.h"
#include "appl_adi_config.h"

/*------------------------------------------------------------------------------
** Current state of the ABCC
**------------------------------------------------------------------------------
*/
appl_AbccHandlerStateType appl_eAbccHandlerState = APPL_INIT;

/*------------------------------------------------------------------------------
** Current anybus state
**------------------------------------------------------------------------------
*/
static volatile ABP_AnbStateType appl_eAnbState = ABP_ANB_STATE_SETUP;

/*------------------------------------------------------------------------------
** Set to TRUE when an unexpected error occur. The main state machine will
** return APPL_MODULE_UNEXPECTED_ERROR when this flag is set.
**------------------------------------------------------------------------------
*/
static BOOL appl_fUnexpectedError = FALSE;

void APPL_UnexpectedError(void)
{
   appl_fUnexpectedError = TRUE;
}

void APPL_Reset(void)
{
   appl_eAbccHandlerState = APPL_DEVRESET;
}

UINT16  ABCC_CbfAdiMappingReq(const AD_AdiEntryType**  const ppsAdiEntry,
                               const AD_DefaultMapType** const ppsDefaultMap)
{
   return AD_AdiMappingReq(ppsAdiEntry, ppsDefaultMap);
}

BOOL ABCC_CbfUpdateWriteProcessData(void* pxWritePd)
{
   /*
   ** AD_UpdatePdWriteData is a general function that updates all ADI:s according
   ** to current map.
   ** If the ADI mapping is fixed there is potential for doing that in a more
   ** optimized way, for example by using memcpy.
   */
   return(AD_UpdatePdWriteData(pxWritePd));
}

void ABCC_CbfNewReadPd(void* pxReadPd)
{
   /*
   ** AD_UpdatePdReadData is a general function that updates all ADI:s according
   ** to current map.
   ** If the ADI mapping is fixed there is potential for doing that in a more
   ** optimized way, for example by using memcpy.
   */
   AD_UpdatePdReadData(pxReadPd);
}

void ABCC_CbfDriverError(ABCC_SeverityType eSeverity, ABCC_ErrorCodeType iErrorCode, UINT32 lAddInfo)
{
   switch(eSeverity)
   {
      case ABCC_SEV_FATAL:
      case ABCC_SEV_WARNING:
         APPL_UnexpectedError();
         break;
      default:
         break;
   }

   (void)iErrorCode;
   (void)lAddInfo;
}

void ABCC_CbfReceiveMsg(ABP_MsgType* psReceivedMsg)
{
   switch( ABCC_GetMsgDestObj(psReceivedMsg))
   {
   case ABP_OBJ_NUM_ECT:
      ECT_ProcessCmdMsg(psReceivedMsg);
      break;

   case ABP_OBJ_NUM_APPD:
      AD_ProcObjectRequest(psReceivedMsg);
      break;

   case ABP_OBJ_NUM_APP:
      APP_ProcessCmdMsg(psReceivedMsg);
      break;

   case ABP_OBJ_NUM_SYNC:
      SYNC_ProcessCmdMsg(psReceivedMsg);
      break;

   default:
      /*
      ** We have received a command to an unsupported object.
      */
      ABP_SetMsgErrorResponse(psReceivedMsg, 1, ABP_ERR_UNSUP_OBJ);
      ABCC_SendRespMsg(psReceivedMsg);
      break;
   }
}

void ABCC_CbfWdTimeout(void)
{
   ABCC_PORT_DebugPrint(("ABCC watchdog timeout"));
}

void ABCC_CbfWdTimeoutRecovered(void)
{
   ABCC_PORT_DebugPrint(("ABCC watchdog recovered"));
}

// }
