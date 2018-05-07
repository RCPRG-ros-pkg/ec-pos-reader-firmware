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

#if(!SYNC_OBJ_ENABLE)
   #error SYNC_OBJ_ENABLE must be set to TRUE in abcc_obj_cfg.h.h
#endif

#if(!ABCC_CFG_SYNC_ENABLE)
   #error ABCC_CFG_SYNC_ENABLE in abcc_drv_cfg.h must be enabled
#endif

/*------------------------------------------------------------------------------
** Access descriptor for the ADIs
**------------------------------------------------------------------------------
*/
#define APPL_WRITE_MAP_READ_ACCESS_DESC (ABP_APPD_DESCR_GET_ACCESS |          \
                                          ABP_APPD_DESCR_MAPPABLE_WRITE_PD)

/*
** Represent the sampled inputs at the sync pulse. The variable is
** linked to the ADI entry table.
*/
static UINT32 _encoder0Position;

/*------------------------------------------------------------------------------
**  32 bit input/output.
**------------------------------------------------------------------------------
*/
const AD_AdiEntryType APPL_asAdiEntryList[] =
{
	{ 1, (char*)"encoder0Position", ABP_UINT32, 1, APPL_WRITE_MAP_READ_ACCESS_DESC, { { &_encoder0Position, NULL } } }
};

/*------------------------------------------------------------------------------
** Map all adi:s in both directions
**------------------------------------------------------------------------------
** 1. AD instance | 2. Direction | 3. Num elements | 4. Start index |
**------------------------------------------------------------------------------
*/
const AD_DefaultMapType APPL_asAdObjDefaultMap[] =
{
	{ 1, PD_WRITE, AD_DEFAULT_MAP_ALL_ELEM, 0 },
	{ AD_DEFAULT_MAP_END_ENTRY }
};

namespace app {
namespace ethercat {

EtherCAT* EtherCAT::_instance = nullptr;

EtherCAT::EtherCAT(common::EventLoop& eventLoop,
	encoders::EncoderMgr& encoderMgr)
	:	_eventLoop(eventLoop),
		_encoderMgr(encoderMgr)
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

	initialize();

	UARTprintf("[EtherCAT] started\n");
}

void
EtherCAT::initialize()
{
	assert(_state == State::Idle);
	UARTprintf("[EtherCAT] initializing...\n");

	_state = State::Init;

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
	assert(_state == State::Init);

	_state = State::WaitForComm;

	// Execute busy wait, to cyclically check the communication status
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

	_eventLoop.busyWait(
		[this]()
		{
			if(_state != State::Run)
			{
				// Module is no longer active, end busy wait
				return true;
			}

			// Module is active, keep communication with ABCC
			if(SYNC_GetMode() == SYNC_MODE_NONSYNCHRONOUS)
			{
				ABCC_TriggerWrPdUpdate();
			}

			ABCC_RunDriver();

			return false; // Don't end the busy wait

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
	if(_encoderMgr.isActive())
	{
		Position position;
		ErrorCode ec;
		_encoderMgr.captureInputs(position, ec);
		if(embxx::error::ErrorStatus(ec))
		{
			// error occured during read operation
			UARTprintf("[EtherCAT] could not capture inputs, ec=%d\n",
				static_cast<int>(ec));
			APPL_UnexpectedError();
			return;
		}

		// inputs capture success
		_encoder0Position = position;
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
   	// const auto instance = app::ethercat::EtherCAT::_instance;
	// assert(instance != nullptr);
	// instance->handleEvent(iEvents);
}

void ABCC_CbfUserInitReq()
{
	const auto moduleType = ABCC_ModuleType();
	const auto networkType = ABCC_NetworkType();

	// Only active CompactCom B40 module and EtherCAT network supported
	assert(moduleType == ABP_MODULE_TYPE_ABCC_40);
	assert(networkType == ABP_NW_TYPE_ECT);

	// No user init needed, so directly end this phase
	ABCC_UserInitComplete();
}

void ABCC_CbfAnbStateChanged(ABP_AnbStateType newAnbState)
{
	constexpr std::array<const char*, 8> stateStrings{{
		"ABP_ANB_STATE_SETUP",
		"ABP_ANB_STATE_NW_INIT",
		"ABP_ANB_STATE_WAIT_PROCESS",
		"ABP_ANB_STATE_IDLE",
		"ABP_ANB_STATE_PROCESS_ACTIVE",
		"ABP_ANB_STATE_ERROR",
		"",
		"ABP_ANB_STATE_EXCEPTION"
	}};

	assert(newAnbState < stateStrings.size());
	UARTprintf("[EtherCAT] ABCC state changed: %s\n",
		stateStrings[newAnbState]);

	switch(newAnbState)
	{
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
