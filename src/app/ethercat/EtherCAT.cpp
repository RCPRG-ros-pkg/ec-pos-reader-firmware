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

extern appl_AbccHandlerStateType appl_eAbccHandlerState;


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

	SysTickPeriodSet(12777216);

	assert(_instance != nullptr);
}

EtherCAT::~EtherCAT()
{
	_instance = nullptr;

	UARTprintf("[EtherCAT] deinitialized");

	assert(_instance == nullptr);
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

int iterations = 0;

void
EtherCAT::start()
{
	UARTprintf("[EtherCAT] starting...\n");

	_eventLoop.busyWait(
		[]()
		{
			// next iteration of ABCC handling loop
			const auto abccHandlerStatus = APPL_HandleAbcc();
			assert(abccHandlerStatus == APPL_MODULE_NO_ERROR);
			static_cast<void>(abccHandlerStatus);

			if(appl_eAbccHandlerState == APPL_RUN)
			{
				if(++iterations == 100000)
				{
					return true;
				}

				return false;
			}

			return false;
		},
		[]()
		{
			UARTprintf("RUN\n");
			// assert(_abccHandlerStatus != APPL_MODULE_NO_ERROR);

			// // ABCC handling loop ends, notify about an error
			// UARTprintf("[EtherCAT] error occured, status: %d\n",
			// 	static_cast<int>(_abccHandlerStatus));
			// UARTprintf("[EtherCAT] stopped\n");
		});

	UARTprintf("[EtherCAT] started\n");
}

void
EtherCAT::handleEvent(std::uint16_t)
{
	if(iterations == 100000)
	{
		return;
	}

	const auto postSuccess = _eventLoop.postInterruptCtx(
		[]()
		{
			const auto abccHandlerStatus = APPL_HandleAbcc();
			assert(abccHandlerStatus == APPL_MODULE_NO_ERROR);
			static_cast<void>(abccHandlerStatus);
		});
	assert(postSuccess);
	static_cast<void>(postSuccess);
}

/*------------------------------------------------------------------------------
** If sync is used this function is called to indicate that write process data
** ADI:s shall be updated with the captured input.
**------------------------------------------------------------------------------
** Arguments:
**    None
**
** Returns:
**    None
**------------------------------------------------------------------------------
*/

int cnt = 0;
int sum = 0; int min = 12777216; int max = 0; int totmin = 12777216; int totmax = 0;

void
EtherCAT::captureInputs()
{
	// copy the sensors to the sync input.

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

	// if(!_called)
	// {
	// 	UARTprintf("!\n");
	// }
	// _called = false;
	SysTickClear();
	SysTickEnable();
	const auto postSuccess = _eventLoop.postInterruptCtx(
		[/*this*/]()
		{
			// _called = true;
			const auto abccHandlerStatus = APPL_HandleAbcc();
			assert(abccHandlerStatus == APPL_MODULE_NO_ERROR);
			static_cast<void>(abccHandlerStatus);

			const int value = (12777216 - SysTickValueGet());
			SysTickDisable();
			sum += value;
			if(value < min)
				min = value;
			if(value > max)
				max = value;
			if(value < totmin)
				totmin = value;
			if(value > totmax)
				totmax = value;
			if(++cnt == 100)
			{
				UARTprintf("%d %d %d %d %d\n",
					sum / 100, min, max, totmin, totmax);
				sum = 0;
				max = 0;
				min = 12777216;
				cnt = 0;
			}
		});
	assert(postSuccess);
	static_cast<void>(postSuccess);
}

void
EtherCAT::captureInputsAsync()
{
	if(!_called)
	{
		UARTprintf("!\n");
	}

	_called = false;
	SysTickClear();
	SysTickEnable();
	const auto postSuccess = _eventLoop.postInterruptCtx(
		[this]()
		{
			_called = true;
			if(_encoderMgr.isActive())
			{
				_encoderMgr.asyncCaptureInputs(&_position,
					[this](auto errorCode)
					{
						if(embxx::error::ErrorStatus(errorCode))
						{
							// error occured during read operation
							UARTprintf("[EtherCAT] could not capture inputs, ec=%d\n",
								static_cast<int>(errorCode));
							APPL_UnexpectedError();
							return;
						}

						// inputs capture success
						_encoder0Position = _position;
						ABCC_TriggerWrPdUpdate();

						// const auto postSuccess = _eventLoop.post(
						// 	[]()
						// 	{
								const auto abccHandlerStatus = APPL_HandleAbcc();
								assert(abccHandlerStatus == APPL_MODULE_NO_ERROR);
								static_cast<void>(abccHandlerStatus);

								const int value = (12777216 - SysTickValueGet());
								SysTickDisable();
								sum += value;
								if(value < min)
									min = value;
								if(value > max)
									max = value;
								if(value < totmin)
									totmin = value;
								if(value > totmax)
									totmax = value;
								if(++cnt == 100)
								{
									UARTprintf("%d %d %d %d %d\n",
										sum / 100, min, max, totmin, totmax);
									sum = 0;
									max = 0;
									min = 12777216;
									cnt = 0;
								}
							});
					// 	assert(postSuccess);
					// 	static_cast<void>(postSuccess);
					// });
			}
			else
			{
				// EncoderMgr is not active, so directly update write process data
				ABCC_TriggerWrPdUpdate();
			}
		});
	assert(postSuccess);
	static_cast<void>(postSuccess);
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
	captureInputsAsync();
}

} // namespace ethercat
} // namespace app

void ABCC_CbfSyncIsr(void)
{
	const auto instance = app::ethercat::EtherCAT::_instance;
	assert(instance != nullptr);
	instance->handleSyncISR();
}

extern volatile BOOL appl_fMsgReceivedEvent;
extern volatile BOOL appl_fRdPdReceivedEvent;
extern volatile BOOL appl_fTransmitMsgEvent;
extern volatile BOOL appl_fAbccStatusEvent;

void ABCC_CbfEvent( UINT16 iEvents )
{
   /*
   ** Set flag to indicate that an event has occurred and the corresponding
   ** ABCC_Trigger<event_action> must be called. In the sample code the the
   ** trigger function is called from main loop context.
   */
   if( iEvents & ABCC_ISR_EVENT_RDPD )
   {
      appl_fRdPdReceivedEvent = TRUE;
   }

   if( iEvents & ABCC_ISR_EVENT_RDMSG )
   {
      appl_fMsgReceivedEvent = TRUE;
   }

   if( iEvents & ABCC_ISR_EVENT_WRMSG )
   {
      appl_fTransmitMsgEvent = TRUE;
   }

   if( iEvents & ABCC_ISR_EVENT_STATUS  )
   {
      appl_fAbccStatusEvent = TRUE;
   }

   	const auto instance = app::ethercat::EtherCAT::_instance;
	assert(instance != nullptr);
	instance->handleEvent(iEvents);
}

UINT16 APPL_GetNumAdi(void)
{
	return(sizeof(APPL_asAdiEntryList) / sizeof(AD_AdiEntryType));
}
