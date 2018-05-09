#include "app/ethercat/EtherCAT.hpp"

#include "tivaware/utils/uartstdio.h"

#include "app/ethercat/abcc_drv/abcc.h"

#include "embxx/error/ErrorStatus.h"

#include "util/driverlib/systick.hpp"

extern "C" {

#include "abcc_td.h"
#include "abcc.h"
#include "appl_adi_config.h"
#include "appl_abcc_handler.h"
#include "app/ethercat/abcc_obj/ad_obj.h"
#include "app/ethercat/abcc_obj/sync_obj.h"

} // extern "C"

/*------------------------------------------------------------------------------
** Access descriptor for the ADIs
**------------------------------------------------------------------------------
*/
#define APPL_WRITE_MAP_READ_ACCESS_DESC (ABP_APPD_DESCR_GET_ACCESS |          \
                                          ABP_APPD_DESCR_MAPPABLE_WRITE_PD)
struct EncoderInputs
{
	BOOL frameError;
	UINT32 position;
};

// struct EncoderSettings
// {
// 	UINT8 resolution;
// 	UINT32 bitRate;
// };

EncoderInputs encoder0Inputs;
EncoderInputs encoder1Inputs;

// EncoderSettings encoder0Settings;
// EncoderSettings encoder1Settings;

static const AD_StructDataType encoder0InputsADIStruct[] =
{
	{ (char*)"Frame error", ABP_BOOL, 1, APPL_WRITE_MAP_READ_ACCESS_DESC, 0, { { &encoder0Inputs.frameError, NULL } } },
	{ (char*)"Position", ABP_UINT32, 1, APPL_WRITE_MAP_READ_ACCESS_DESC, 0, { { &encoder0Inputs.position, NULL } } }
};

static const AD_StructDataType encoder1InputsADIStruct[] =
{
	{ (char*)"Frame error", ABP_BOOL, 1, APPL_WRITE_MAP_READ_ACCESS_DESC, 0, { { &encoder1Inputs.frameError, NULL } } },
	{ (char*)"Position", ABP_UINT32, 1, APPL_WRITE_MAP_READ_ACCESS_DESC, 0, { { &encoder1Inputs.position, NULL } } }
};

// static const AD_StructDataType encoder0SettingsADIStruct[] =
// {
// 	{ (char*)"Resolution", ABP_UINT8, 1, ABP_APPD_DESCR_SET_ACCESS | ABP_APPD_DESCR_GET_ACCESS, 0, { { &encoder0Settings.resolution, NULL } } },
// 	{ (char*)"Bit rate", ABP_UINT32, 1, ABP_APPD_DESCR_SET_ACCESS | ABP_APPD_DESCR_GET_ACCESS, 0, { { &encoder0Settings.bitRate, NULL } } }
// };

// static const AD_StructDataType encoder1SettingsADIStruct[] =
// {
// 	{ (char*)"Resolution", ABP_UINT8, 1, ABP_APPD_DESCR_SET_ACCESS | ABP_APPD_DESCR_GET_ACCESS, 0, { { &encoder1Settings.resolution, NULL } } },
// 	{ (char*)"Bit rate", ABP_UINT32, 1, ABP_APPD_DESCR_SET_ACCESS | ABP_APPD_DESCR_GET_ACCESS, 0, { { &encoder1Settings.bitRate, NULL } } }
// };

const AD_AdiEntryType APPL_asAdiEntryList[] =
{
	{ 1, (char*)"Encoder0 Inputs", ABP_UINT8, 2, APPL_WRITE_MAP_READ_ACCESS_DESC,  { { NULL, NULL } }, encoder0InputsADIStruct, NULL, NULL },
	{ 2, (char*)"Encoder1 Inputs", ABP_UINT8, 2, APPL_WRITE_MAP_READ_ACCESS_DESC,  { { NULL, NULL } }, encoder1InputsADIStruct, NULL, NULL }
	// { 3, (char*)"Encoder0 Settings", ABP_UINT8, 2, ABP_APPD_DESCR_SET_ACCESS | ABP_APPD_DESCR_GET_ACCESS,  { { NULL, NULL } }, encoder0SettingsADIStruct, NULL, setEncoder0Settings },
	// { 4, (char*)"Encoder1 Settings", ABP_UINT8, 2, ABP_APPD_DESCR_SET_ACCESS | ABP_APPD_DESCR_GET_ACCESS,  { { NULL, NULL } }, encoder1SettingsADIStruct, NULL, NULL }
};

/*------------------------------------------------------------------------------
** Map all adi:s in both directions
**------------------------------------------------------------------------------
** 1. AD instance | 2. Direction | 3. Num elements | 4. Start index |
**------------------------------------------------------------------------------
*/
const AD_DefaultMapType APPL_asAdObjDefaultMap[] =
{
	{ 1, PD_WRITE, 1, 0 },
	{ 1, PD_WRITE, 1, 1 },
	{ 2, PD_WRITE, 1, 0 },
	{ 2, PD_WRITE, 1, 1 },
	{ AD_DEFAULT_MAP_END_ENTRY }
};

namespace app {
namespace ethercat {

EtherCAT* EtherCAT::_instance = nullptr;

EtherCAT::EtherCAT(common::EventLoop& eventLoop,
	encoders::Encoder0& encoder0,
	encoders::Encoder1& encoder1)
	:	_eventLoop(eventLoop),
		_encoder0(encoder0),
		_encoder1(encoder1)
{
	setupABCCHardware();
	_instance = this;

	UARTprintf("[EtherCAT] ready\n");
	assert(_instance != nullptr);
	assert(_state == State::Idle);
}

EtherCAT::~EtherCAT()
{
	assert(_state == State::Idle);

	_instance = nullptr;

	UARTprintf("[EtherCAT] deinitialized");
	assert(_instance == nullptr);
}

void
EtherCAT::setupABCCHardware()
{
	assert(_state == State::Idle);
	UARTprintf("[EtherCAT] Setting up ABCC hardware...\n");

	const auto errorCode = ABCC_HwInit();
	assert(errorCode == ABCC_EC_NO_ERROR);

	UARTprintf("[EtherCAT] ABCC hardware setup success\n");
}

void
EtherCAT::start()
{
	assert(_state == State::Idle);
	UARTprintf("[EtherCAT] starting...\n");

	initDriver();

	UARTprintf("[EtherCAT] started\n");
}

void
EtherCAT::initDriver()
{
	assert(_state == State::Idle);
	UARTprintf("[EtherCAT] initializing driver...\n");

	_state = State::DriverInit;

	if(!ABCC_ModuleDetect())
	{
		UARTprintf("[EtherCAT] module not detected\n");
		_state = State::Error;

		return;
	}

	if(AD_Init(APPL_asAdiEntryList, APPL_GetNumAdi(),
		APPL_asAdObjDefaultMap) != APPL_NO_ERROR)
	{
		UARTprintf("[EtherCAT] could not initialize AD\n");
		_state = State::Error;

		return;
	}

	constexpr auto StartupTimeMs = 0;
	if(ABCC_StartDriver(StartupTimeMs) != ABCC_EC_NO_ERROR)
	{
		UARTprintf("[EtherCAT] module not answering\n");
		_state = State::Error;

		return;
	}

    ABCC_HWReleaseReset();

	UARTprintf("[EtherCAT] init success\n");
	waitForCommunication();
}

void
EtherCAT::waitForCommunication()
{
	assert(_state == State::DriverInit);

	_state = State::WaitForComm;

	// Execute busy wait, to cyclically check the communication status
	//  in order to detect moment, when ABCC is ready
	_eventLoop.busyWait(
		[this]()
		{
			assert(_state == State::WaitForComm);

			const auto commState = ABCC_isReadyForCommunication();
			if(commState == ABCC_READY_FOR_COMMUNICATION)
			{
				UARTprintf("[EtherCAT] module ready for communication\n");
				run();

				return true; // Ready for communication, so end busy wait
			}
			else if(commState == ABCC_COMMUNICATION_ERROR)
			{
				UARTprintf("[EtherCAT] module not answering\n");
				_state = State::Error;

				return true; // Error, so end busy wait
			}

			return false; // Not ready for communication, so don't end the wait
		},
		[]()
		{
			/* Busy wait ends, do nothing */
		});

	UARTprintf("[EtherCAT] waiting for communication...\n");
}

void
EtherCAT::run()
{
	assert(_state == State::WaitForComm);

	_state = State::Run;

	// Cyclically poll ABCC driver
	_eventLoop.busyWait(
		[this]()
		{
			// Module is active, keep communication with ABCC
			if(const auto errorCode = ABCC_RunDriver();
				errorCode != ABCC_EC_NO_ERROR)
			{
				UARTprintf("[EtherCAT] driver error during run, ec=%d\n",
					errorCode);
				_state = State::Error;
			}

			const auto endWait = (_state != State::Run);
			return endWait;
		},
		[]()
		{
			UARTprintf("[EtherCAT] stopped\n");
		});

	UARTprintf("[EtherCAT] running...\n");
}

void
EtherCAT::captureInputs()
{
	encoders::Encoder0::Position position = 0;
	ErrorCode ec;
	_encoder0.captureInputs(position, ec);
	if(embxx::error::ErrorStatus(ec))
	{
		encoder0Inputs.frameError = true;
	}
	else
	{
		// inputs capture success
		encoder0Inputs.position = position;
		encoder0Inputs.frameError = false;
	}

	encoders::Encoder1::Position position1 = 0;
	ErrorCode ec1;
	_encoder1.captureInputs(position1, ec1);
	if(embxx::error::ErrorStatus(ec1))
	{
		encoder1Inputs.frameError = true;
	}
	else
	{
		// inputs capture success
		encoder1Inputs.position = position1;
		encoder1Inputs.frameError = false;
	}

	/*
	** Always update the ABCC with the latest write process data at the end of
	** this function.
	*/
	ABCC_TriggerWrPdUpdate();
}

void
EtherCAT::handleSyncISR()
{
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
	if(ABCC_GetAppStatus() != ABP_APPSTAT_NO_ERROR)
	{
		if(ABCC_GetAppStatus() == ABP_APPSTAT_NOT_SYNCED)
		{
			ABCC_SetAppStatus(ABP_APPSTAT_NO_ERROR);
		}
		else
		{
			UARTprintf("[EtherCAT] returning\n");
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
	captureInputs();
}

} // namespace ethercat
} // namespace app

void ABCC_CbfSyncIsr(void)
{
	const auto instance = app::ethercat::EtherCAT::_instance;
	assert(instance != nullptr);
	instance->handleSyncISR();
}

void ABCC_CbfEvent(UINT16)
{
	/* do nothing */
}

void ABCC_CbfUserInitReq()
{
	const auto moduleType = ABCC_ModuleType();
	const auto networkType = ABCC_NetworkType();

	// Only ABCC B40 EtherCAT chip supported
	assert(moduleType == ABP_MODULE_TYPE_ABCC_40);
	assert(networkType == ABP_NW_TYPE_ECT);

	// No user init needed, so directly end this phase
	ABCC_UserInitComplete();
}

void ABCC_CbfAnbStateChanged(ABP_AnbStateType newAnbState)
{
	constexpr std::array<const char*, 8> stateStrings{{
		"SETUP",
		"NW_INIT",
		"WAIT_PROCESS",
		"IDLE",
		"PROCESS_ACTIVE",
		"ERROR",
		"",
		"EXCEPTION"
	}};

	assert(newAnbState < stateStrings.size());
	UARTprintf("[EtherCAT] ABCC state changed: %s\n",
		stateStrings[newAnbState]);

	switch(newAnbState)
	{
	case ABP_ANB_STATE_SETUP:
		break;

	case ABP_ANB_STATE_PROCESS_ACTIVE:
		ABCC_TriggerWrPdUpdate();
		break;

	default:
		break;
	}
}

UINT16 APPL_GetNumAdi(void)
{
	return(sizeof(APPL_asAdiEntryList) / sizeof(AD_AdiEntryType));
}
