#include "app/ethercat/EtherCAT.hpp"

#include <array>

#include "tivaware/utils/uartstdio.h"

extern "C" {

#include "abcc_td.h"
#include "abp.h"
#include "abcc.h"
#include "abcc_cmd_seq_if.h"
#include "ad_obj.h"
#include "abcc_port.h"

#include "ad_obj.h"
#include "app_obj.h"
#include "appl_abcc_handler.h"
#include "abcc_port.h"
#include "etn_obj.h"
#include "opcua_obj.h"
#include "sync_obj.h"
#include "safe_obj.h"

#include "ect.h"
#include "cop.h"

#include "abcc_obj_cfg.h"

} // extern "C"

#define APPL_WRITE_MAP_READ_ACCESS_DESC ( ABP_APPD_DESCR_GET_ACCESS |          \
                                          ABP_APPD_DESCR_MAPPABLE_WRITE_PD )

static UINT32 appl_lSyncInput;

const AD_AdiEntryType APPL_asAdiEntryList[] =
{
   { 51, (char*)"SyncInput", ABP_UINT32, 1, APPL_WRITE_MAP_READ_ACCESS_DESC, { { &appl_lSyncInput, NULL } } },
};

const AD_DefaultMapType APPL_asAdObjDefaultMap[] =
{
   { 51, PD_WRITE, AD_DEFAULT_MAP_ALL_ELEM, 0 },
   { AD_DEFAULT_MAP_END_ENTRY }
};

namespace app {
namespace ethercat {

EtherCAT* EtherCAT::_instance = nullptr;

EtherCAT::EtherCAT(common::EventLoop& eventLoop,
	common::SysTickDriver& sysTickDriver)
	:	_eventLoop(eventLoop),
		_timer(sysTickDriver.allocTimer())
{
	assert(_timer.isValid());

	_instance = this;
	setupABCCHardware();

	UARTprintf("[EtherCAT] preinitialized\n");
	assert(_state == State::PreInit);
	assert(_instance);
}

EtherCAT::~EtherCAT()
{
	_instance = nullptr;
}

void
EtherCAT::setupABCCHardware()
{
	UARTprintf("[EtherCAT] Setting up ABCC hardware...\n");

	const auto errorCode = ABCC_HwInit();
	if(errorCode != ABCC_EC_NO_ERROR)
	{
		UARTprintf("[EtherCAT] ABCC hardware setup error: %d\n",
			errorCode);
		std::exit(EXIT_FAILURE);
	}

	UARTprintf("[EtherCAT] ABCC hardware setup success\n");
}

void
EtherCAT::doInit()
{
	assert(_state == State::PreInit);
	UARTprintf("[EtherCAT] initializing...\n");

	_state = State::Init;

	if(!initApplicationDataObject())
	{
		return;
	}

	if(!startABCCDriver())
	{
		return;
	}

	waitForCommunication();
}

void
EtherCAT::doStart()
{
	assert(_state == State::Ready);
	UARTprintf("[EtherCAT] starting...\n");

	_state = State::Active;

	_eventLoop.busyWait(
		[]()
		{
			if(SYNC_GetMode() == SYNC_MODE_NONSYNCHRONOUS)
			{
				// UARTprintf("SYNC_MODE_NONSYNCHRONOUS\n");
				ABCC_TriggerWrPdUpdate();
			}

			ABCC_RunDriver();

			return false; // continue busy wait
		},
		[]()
		{
			assert(!"Should not get here");
		});
}

bool EtherCAT::initApplicationDataObject()
{
	assert(_state == State::Init);
	UARTprintf("[EtherCAT] initializing AD...\n");

	const auto adStatus = AD_Init(APPL_asAdiEntryList,
		sizeof(APPL_asAdiEntryList) / sizeof(AD_AdiEntryType),
		APPL_asAdObjDefaultMap);
	if(adStatus != APPL_NO_ERROR)
	{
		UARTprintf("[EtherCAT] AD init error: %d\n", adStatus);

		_state = State::Error;
		const auto postSuccess = _eventLoop.post(
			[this]() { _initCallback(Status::UnexpectedError); });
		assert(postSuccess);
		static_cast<void>(postSuccess);

		return false;
	}

	UARTprintf("[EtherCAT] AD initialized\n");

	return true;
}

bool EtherCAT::startABCCDriver()
{
	assert(_state == State::Init);
	UARTprintf("[EtherCAT] starting driver...\n");

	constexpr auto startupTime = std::chrono::milliseconds::zero();
	const auto startDriverStatus = ABCC_StartDriver(startupTime.count());
	if(startDriverStatus != ABCC_EC_NO_ERROR)
	{
		UARTprintf("[EtherCAT] start driver error: %d\n", startDriverStatus);

		_state = State::Error;
		const auto postSuccess = _eventLoop.post(
			[this]() { _initCallback(Status::ModuleNotAnswering); });
		assert(postSuccess);
		static_cast<void>(postSuccess);

		return false;
	}

	UARTprintf("[EtherCAT] driver started\n");

	return true;
}

void EtherCAT::waitForCommunication()
{
	assert(_state == State::Init);
	UARTprintf("[EtherCAT] waiting for communication...\n");

	ABCC_HWReleaseReset();
	auto communicationState = ABCC_NOT_READY_FOR_COMMUNICATION;
	_eventLoop.busyWait(
		[&communicationState]()
		{
			communicationState = ABCC_isReadyForCommunication();
			const auto isReady = (communicationState != ABCC_NOT_READY_FOR_COMMUNICATION);
			return isReady;
		},
		[&]()
		{
			assert(communicationState != ABCC_NOT_READY_FOR_COMMUNICATION);

			if(communicationState == ABCC_COMMUNICATION_ERROR)
			{
				UARTprintf("[EtherCAT] communication init error!\n");

				_state = State::Error;
				const auto postSuccess = _eventLoop.post(
					[this]() { _initCallback(Status::ModuleNotAnswering); });
				assert(postSuccess);
				static_cast<void>(postSuccess);

				return;
			}

			UARTprintf("[EtherCAT] communication established\n");

			_state = State::Ready;
			const auto postSuccess = _eventLoop.post(
				[this]() { _initCallback(Status::Success); });
			assert(postSuccess);
			static_cast<void>(postSuccess);
		});
}

} // namespace ethercat
} // namespace app

static ABCC_CmdSeqRespStatusType HandleExceptionResp(ABP_MsgType* msg);
static ABCC_CmdSeqRespStatusType HandleExceptionInfoResp(ABP_MsgType* msg);
static ABCC_CmdSeqCmdStatusType ReadExeption(ABP_MsgType* msg);
static ABCC_CmdSeqCmdStatusType ReadExeptionInfo(ABP_MsgType* msg);
static void APPL_SyncIsr();

static volatile ABP_AnbStateType anybusState = ABP_ANB_STATE_SETUP;

constexpr std::array<ABCC_CmdSeqType, 3>
appl_asReadExeptionCmdSeq = {{
   ABCC_CMD_SEQ( ReadExeption,     HandleExceptionResp ),
   ABCC_CMD_SEQ( ReadExeptionInfo, HandleExceptionInfoResp ),
   ABCC_CMD_SEQ_END()
}};

static BOOL unexpectedError = FALSE;

//! Builds the command for reading the exception code
static ABCC_CmdSeqCmdStatusType
ReadExeption(ABP_MsgType* msg)
{
	ABCC_GetAttribute(msg, ABP_OBJ_NUM_ANB, 1, ABP_ANB_IA_EXCEPTION, ABCC_GetNewSourceId());
	return (ABCC_SEND_COMMAND);
}

//! Builds the command for reading the exception info code
static ABCC_CmdSeqCmdStatusType
ReadExeptionInfo(ABP_MsgType* msg)
{
	ABCC_GetAttribute(msg, ABP_OBJ_NUM_NW, 1, ABP_NW_IA_EXCEPTION_INFO, ABCC_GetNewSourceId());
	return (ABCC_SEND_COMMAND);
}

//! Handles the exception code response
static ABCC_CmdSeqRespStatusType
HandleExceptionResp(ABP_MsgType* msg)
{
	if(ABCC_VerifyMessage(msg) != ABCC_EC_NO_ERROR)
	{
		APPL_UnexpectedError();
		return ABCC_EXEC_NEXT_COMMAND;
	}

	UINT8 exceptionCode;
	ABCC_GetMsgData8(msg, &exceptionCode, 0);
	UARTprintf("[EtherCAT] exception code: %x\n", exceptionCode);

	return ABCC_EXEC_NEXT_COMMAND;
}

//! Handles the exception info code response
static ABCC_CmdSeqRespStatusType
HandleExceptionInfoResp( ABP_MsgType* msg )
{
	if(ABCC_VerifyMessage(msg) != ABCC_EC_NO_ERROR)
	{
		APPL_UnexpectedError();
		return ABCC_EXEC_NEXT_COMMAND;
	}

	UINT8 exceptionInfo;
	ABCC_GetMsgData8(msg, &exceptionInfo, 0);
	UARTprintf("[EtherCAT] exception info: %x\n", exceptionInfo);

	return ABCC_EXEC_NEXT_COMMAND;
}

void
ABCC_CbfUserInitReq()
{
	const auto moduleType = ABCC_ModuleType();
	const auto networkType = ABCC_NetworkType();
	if(moduleType != ABP_MODULE_TYPE_ABCC_40)
	{
		UARTprintf("[EtherCAT] unsupported module type: %d\n", moduleType);
		std::exit(EXIT_FAILURE);
	}
	else if(networkType != ABP_NW_TYPE_ECT)
	{
		UARTprintf("[EtherCAT] unsupported network type: %d\n", networkType);
		std::exit(EXIT_FAILURE);
	}

	ABCC_UserInitComplete();
}

static const char*
anybusStateToString(ABP_AnbStateType anybusState)
{
	static std::array<const char*, 8> anybusStatesStrings = {{
		"ABP_ANB_STATE_SETUP",
		"ABP_ANB_STATE_NW_INIT",
		"ABP_ANB_STATE_WAIT_PROCESS",
		"ABP_ANB_STATE_IDLE",
		"ABP_ANB_STATE_PROCESS_ACTIVE",
		"ABP_ANB_STATE_ERROR",
		"",
		"ABP_ANB_STATE_EXCEPTION"
	}};

	return anybusStatesStrings[anybusState];
}

void
ABCC_CbfAnbStateChanged(ABP_AnbStateType newAnybusState)
{
	UARTprintf("[EtherCAT] anybus status changed: %s->%s\n",
		anybusStateToString(anybusState),
		anybusStateToString(newAnybusState));
	anybusState = newAnybusState;

	switch(anybusState)
	{
		case ABP_ANB_STATE_PROCESS_ACTIVE:
			ABCC_TriggerWrPdUpdate();
			break;

		case ABP_ANB_STATE_EXCEPTION:
			/* Trigger message sequence for reading exception data */
			ABCC_AddCmdSeq(&appl_asReadExeptionCmdSeq[0], NULL);
			break;

		case ABP_ANB_STATE_ERROR:
			break;

		default:
			break;
	}
}

void
ABCC_CbfDriverError(ABCC_SeverityType severity,
	ABCC_ErrorCodeType errorCode, UINT32 additionalInfo)
{
	UARTprintf("[EtherCAT] driver error: severity=%d, code=%d, info=%d\n",
		severity, errorCode, additionalInfo);
	switch(severity)
	{
		case ABCC_SEV_FATAL:
		case ABCC_SEV_WARNING:
			APPL_UnexpectedError();
			break;
		default:
			break;
	}
}

BOOL
ABCC_CbfUpdateWriteProcessData(void* writeProcessData)
{
	return AD_UpdatePdWriteData(writeProcessData);
}

void
ABCC_CbfNewReadPd(void* readProcessData)
{
	AD_UpdatePdReadData(readProcessData);
}

void
APPL_UnexpectedError()
{
	UARTprintf("[EtherCAT] unexpected error occured\n");
	unexpectedError = TRUE;
	assert(!"Not implemented");
}

void
ABCC_CbfReceiveMsg(ABP_MsgType* receivedMsg)
{
	const int destObj = ABCC_GetMsgDestObj(receivedMsg);
	switch(destObj)
	{
		case ABP_OBJ_NUM_ETN:
			UARTprintf("[EtherCAT] received ABP_OBJ_NUM_ETN\n");
			ETN_ProcessCmdMsg(receivedMsg);
			break;

		case ABP_OBJ_NUM_ECT:
			UARTprintf("[EtherCAT] received ABP_OBJ_NUM_ECT\n");
			ECT_ProcessCmdMsg(receivedMsg);
			break;

		case ABP_OBJ_NUM_APPD:
			UARTprintf("[EtherCAT] received ABP_OBJ_NUM_APPD\n");
			AD_ProcObjectRequest(receivedMsg);
			break;

		case ABP_OBJ_NUM_APP:
			UARTprintf("[EtherCAT] received ABP_OBJ_NUM_APP\n");
			APP_ProcessCmdMsg(receivedMsg);
			break;

		case ABP_OBJ_NUM_SYNC:
			UARTprintf("[EtherCAT] received ABP_OBJ_NUM_SYNC\n");
			SYNC_ProcessCmdMsg(receivedMsg);
			break;

		default:
			UARTprintf("[EtherCAT] received unknown object: %d\n", destObj);
			// We have received a command to an unsupported object.
			ABP_SetMsgErrorResponse(receivedMsg, 1, ABP_ERR_UNSUP_OBJ);
			ABCC_SendRespMsg(receivedMsg);
			break;
	}
}

UINT16
ABCC_CbfAdiMappingReq(const AD_AdiEntryType** const adiEntry,
	const AD_DefaultMapType** const defaultMap)
{
	return AD_AdiMappingReq(adiEntry, defaultMap);
}

void
ABCC_CbfSyncIsr()
{
	APPL_SyncIsr();
}

void
ABCC_CbfEvent(UINT16 iEvents)
{
	// other values not acceptable in SPI mode with this driver
	assert(iEvents == 0);
	static_cast<void>(iEvents);
}

void
ABCC_CbfWdTimeout()
{
	UARTprintf("[EtherCAT] watchdog timeout\n");
}

void
ABCC_CbfWdTimeoutRecovered()
{
	UARTprintf("[EtherCAT] watchdog timeout recovered\n");
}

void
APPL_Reset()
{
	UARTprintf("[EtherCAT] reset request!\n");
	assert(!"Not implemented");
}

int readEncoder()
{
   return 100;
}

static void
triggerAdiSyncInputCapture()
{
   /*
   **  Copy the sensor to the sync input.
   **  appl_lSensorValue represent for example a measured speed.
   */
   appl_lSyncInput = readEncoder();

   /*
   ** Always update the ABCC with the latest write process data at the end of
   ** this function.
   */
   ABCC_TriggerWrPdUpdate();
}

void
APPL_SyncIsr()
{
   // ABCC_PORT_DebugPrint(("SYNC ISR\n"));

   /*
   ** This is the the start of the sync cycle. This point is as close to the
   ** SYNC Input Capture Point as this example gets. The measurement pin for
   ** input processing measurements is set here.
   ** Optimally this measurement should be done on the interrupt pin, but the
   ** GPIO needs to be toggled so that ABCC_GpioReset() can cause a sloping
   ** flank at the end point of input processing.
   */
#if ABCC_CFG_SYNC_MEASUREMENT_IP
   ABCC_GpioSet();
#endif

   /*
   ** PORTING ALERT!
   ** Some applications require a PLL being locked to the sync signal before
   ** PROCESS_ACTIVE is entered. It is also possible to do measurement to verify
   ** the cycle time before allowing PROCESS_ACTIVE.
   ** Only ABP_APPSTAT_NO_ERROR will allow the ABCC to enter PROCESS_ACTIVE.
   **
   ** In those cases this call must be relocated.
   ** In this example PROCESS_ACTIVE will be allowed as soon as the first sync
   ** interrupt appears.
   */
   if( ABCC_GetAppStatus() != ABP_APPSTAT_NO_ERROR )
   {
      if( ABCC_GetAppStatus() == ABP_APPSTAT_NOT_SYNCED )
      {
         ABCC_SetAppStatus( ABP_APPSTAT_NO_ERROR );
      }
      else
      {
         return;
      }
   }

   /*
   ** PORTING ALERT!
   ** The InputCaptureTime attribute in the sync object defines the time in
   ** nano seconds that shall be waited from this point before capturing the
   ** input data and send it to the ABCC.
   ** This means that a timer shall be started here, and when it expires
   ** triggerAdiSyncInputCapture() shall be called.
   ** In this example the input capture  time is ignored and the
   ** function is called directly (InputCaptureTime = 0).
   */
   triggerAdiSyncInputCapture();
}
