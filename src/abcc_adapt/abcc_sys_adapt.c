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
*/

#include <assert.h>

#include "abcc_drv_cfg.h"
#include "abcc_port.h"

#include "abcc_sys_adapt.h"
#include "abcc_sys_adapt_spi.h"
#include "abcc_sys_adapt_par.h"
#include "abcc_sys_adapt_ser.h"

#include "tivaware/inc/hw_memmap.h"
#include "tivaware/driverlib/sysctl.h"
#include "tivaware/driverlib/interrupt.h"
#include "tivaware/driverlib/gpio.h"
#include "tivaware/driverlib/pin_map.h"
#include "tivaware/driverlib/ssi.h"
#include "tivaware/driverlib/rom.h"
#include "tivaware/driverlib/rom_map.h"
#include "tivaware/utils/uartstdio.h"

#define MD0_PIN GPIO_PIN_1
#define RESET_PIN GPIO_PIN_2
#define IRQ_PIN GPIO_PIN_6
#define MI0_SYNC_PIN GPIO_PIN_7
#define MI1_PIN GPIO_PIN_3

void portaIRQ()
{
   int maskedStatus = GPIOIntStatus(GPIO_PORTA_BASE, true);
   GPIOIntClear(GPIO_PORTA_BASE, GPIO_INT_PIN_6 | GPIO_INT_PIN_7);
   UARTprintf("Interrupt occured, status=%x\n", maskedStatus);
}

BOOL ABCC_SYS_HwInit( void )
{
   MAP_SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOA);
   MAP_SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOB);

   // Configure Reset pin as output
   MAP_GPIOPinTypeGPIOOutput(GPIO_PORTB_BASE, RESET_PIN);

   // Configure Module detection pin 0 as input
   MAP_GPIOPinTypeGPIOInput(GPIO_PORTB_BASE, MD0_PIN);

   // Configure IRQ pin to be interrupt driven input
   MAP_GPIOPinTypeGPIOInput(GPIO_PORTA_BASE, IRQ_PIN);
   MAP_GPIOIntTypeSet(GPIO_PORTA_BASE, IRQ_PIN, GPIO_FALLING_EDGE);
   MAP_GPIOIntEnable(GPIO_PORTA_BASE, GPIO_INT_PIN_6);

   // Configure MI1 pin as input
   MAP_GPIOPinTypeGPIOInput(GPIO_PORTB_BASE, MI1_PIN);

   // Configure MI0/Sync pin to be interrupt driven input
   MAP_GPIOPinTypeGPIOInput(GPIO_PORTA_BASE, MI0_SYNC_PIN);
   MAP_GPIOIntTypeSet(GPIO_PORTA_BASE, MI0_SYNC_PIN, GPIO_RISING_EDGE);
   MAP_GPIOIntEnable(GPIO_PORTA_BASE, GPIO_INT_PIN_7);

   // Register interrupts for Port A
   GPIOIntRegister(GPIO_PORTA_BASE, portaIRQ);

   // Configure GPIO of pins of SSI2 module.
   MAP_GPIOPinTypeSSI(GPIO_PORTB_BASE,
      GPIO_PIN_4 | GPIO_PIN_5 | GPIO_PIN_6 | GPIO_PIN_7);
   MAP_GPIOPinConfigure(GPIO_PB4_SSI2CLK);
   MAP_GPIOPinConfigure(GPIO_PB5_SSI2FSS);
   MAP_GPIOPinConfigure(GPIO_PB6_SSI2RX);
   MAP_GPIOPinConfigure(GPIO_PB7_SSI2TX);

   // configure SSI2: SPI 3 mode, master, 1MHz and 8bits frame width, DMA RX+TX
   MAP_SysCtlPeripheralEnable(SYSCTL_PERIPH_SSI2);
   MAP_SSIConfigSetExpClk(SSI2_BASE, MAP_SysCtlClockGet(),
      SSI_FRF_MOTO_MODE_3, SSI_MODE_MASTER, 1000000, 8);
   SSIEnable(SSI2_BASE);

   // globally enable interrupts
   IntMasterEnable();

   return true;
}


BOOL ABCC_SYS_Init( void )
{
   /*
   ** Implement according to abcc_sys_adapt.h
   */
   return true;
}


void ABCC_SYS_Close( void )
{
   /*
   ** Implement according to abcc_sys_adapt.h
   */
}

//! Sets Reset pin to LOW
void ABCC_SYS_HWReset( void )
{
   GPIOPinWrite(GPIO_PORTB_BASE, RESET_PIN, 0);
}

//! Sets Reset pin to HIGH
void ABCC_SYS_HWReleaseReset( void )
{
   GPIOPinWrite(GPIO_PORTB_BASE, RESET_PIN, RESET_PIN);
}


#ifndef ABCC_CFG_ABCC_MODULE_ID
//! Reads states of MI0 and MI1 pins and returns received Module ID
UINT8 ABCC_SYS_ReadModuleId( void )
{
   const int mi0State = GPIOPinRead(GPIO_PORTA_BASE, MI0_SYNC_PIN);
   const int mi1State = GPIOPinRead(GPIO_PORTB_BASE, MI1_PIN);

   int result = (mi0State ? 0x1 : 0) | (mi1State ? 0x2 : 0);
   assert(result == 0x2); // CC40 Only
   return result;
}
#endif


#if( ABCC_CFG_MOD_DETECT_PINS_CONN )
//! Checks MD0 pin, if module is present.
BOOL ABCC_SYS_ModuleDetect( void )
{
   const int md0State = GPIOPinRead(GPIO_PORTB_BASE, MD0_PIN);
   const bool moduleDetected = (md0State ? false : true);
   return moduleDetected;
}
#endif

// #if( ABCC_CFG_SYNC_ENABLE && ABCC_CFG_USE_ABCC_SYNC_SIGNAL )
// void ABCC_SYS_SyncInterruptEnable( void )
// {
//    /*
//    ** Implement according to abcc_sys_adapt.h
//    */
// }
// #endif

// #if( ABCC_CFG_SYNC_ENABLE && ABCC_CFG_USE_ABCC_SYNC_SIGNAL )
// void ABCC_SYS_SyncInterruptDisable( void )
// {
//    /*
//    ** Implement according to abcc_sys_adapt.h
//    */
// }
// #endif

// #if( ABCC_CFG_INT_ENABLED )
// void ABCC_SYS_AbccInterruptEnable( void )
// {

//    ** Implement according to abcc_sys_adapt.h

// }


// void ABCC_SYS_AbccInterruptDisable( void )
// {
//    /*
//    ** Implement according to abcc_sys_adapt.h
//    */
// }
// #endif


// #if( ABCC_CFG_POLL_ABCC_IRQ_PIN )
// BOOL ABCC_SYS_IsAbccInterruptActive( void )
// {
//    /*
//    ** Implement according to abcc_sys_adapt.h
//    */
// }
// #endif

#define FIFO_SIZE 8
static ABCC_SYS_SpiDataReceivedCbfType spiDataReceivedCb = 0;

#if( ABCC_CFG_DRV_SPI )
//! Registers callback to be called after MISO frame receive.
void ABCC_SYS_SpiRegDataReceived( ABCC_SYS_SpiDataReceivedCbfType pnDataReceived  )
{
   spiDataReceivedCb = pnDataReceived;
}

//! Stores MOSI and MISO buffers, and starts MOSI transfer.
void ABCC_SYS_SpiSendReceive( void* pxSendDataBuffer, void* pxReceiveDataBuffer, UINT16 iLength )
{
   const uint8_t* txBuffer = (const uint8_t*)(pxSendDataBuffer);
   uint8_t* rxBuffer = (uint8_t*)(pxReceiveDataBuffer);

   size_t txElapsed = iLength;
   size_t rxElapsed = iLength;
   assert(!SSIBusy(SSI2_BASE));

   while(txElapsed--)
   {
      uint32_t txData = (uint32_t)(*txBuffer);
      const int txSuccess = SSIDataPutNonBlocking(SSI2_BASE, txData);
      if(txSuccess)
      {
         ++txBuffer;
      }
      else
      {
         ++txElapsed;
      }

      uint32_t rxData;
      const int rxSuccess = SSIDataGetNonBlocking(SSI2_BASE, &rxData);
      if(rxSuccess)
      {
         *(rxBuffer++) = (uint8_t)(rxData);
         --rxElapsed;
      }
   }

   if(rxElapsed)
   {
      while(rxElapsed--)
      {
         uint32_t data;
         const int rxSuccess = SSIDataGetNonBlocking(SSI2_BASE, &data);
         if(rxSuccess)
         {
            *(rxBuffer++) = (uint8_t)(data);
         }
         else
         {
            ++rxElapsed;
         }
      }
   }

   assert(spiDataReceivedCb);
   spiDataReceivedCb();
}
#endif
