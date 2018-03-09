#include <assert.h>

#include "abcc_drv_cfg.h"
#include "abcc_port.h"

#include "abcc_sys_adapt.h"
#include "abcc_sys_adapt_spi.h"

#include "abcc.h"

#include "tivaware/inc/hw_types.h"
#include "tivaware/inc/hw_memmap.h"
#include "tivaware/driverlib/sysctl.h"
#include "tivaware/inc/hw_ints.h"
#include "tivaware/driverlib/interrupt.h"
#include "tivaware/driverlib/gpio.h"
#include "tivaware/driverlib/pin_map.h"
#include "tivaware/inc/hw_ssi.h"
#include "tivaware/driverlib/ssi.h"
#include "tivaware/driverlib/udma.h"
#include "tivaware/driverlib/rom.h"
#include "tivaware/driverlib/rom_map.h"
#include "tivaware/utils/uartstdio.h"

#define MD0_PIN      GPIO_PIN_1
#define RESET_PIN    GPIO_PIN_2
#define IRQ_PIN      GPIO_PIN_6
#define MI0_SYNC_PIN GPIO_PIN_7
#define MI1_PIN      GPIO_PIN_3

//! Callback function used to inform ABCC about received MISO frame
static ABCC_SYS_SpiDataReceivedCbfType spiDataReceivedCb = 0;

//! Array for DMA control table. 1KB aligned
__attribute__((aligned(1024)))
static uint8_t dmaControlTable[1024];

//! uDMA SSI2RX channel number
#define SSI2RX_CH 12

//! uDMA SSI2RX channel number mask
#define SSI2RX_CH_M (1 << SSI2RX_CH)

//! uDMA SSI2RX channel asignment
#define SSI2RX_ASGN UDMA_CH12_SSI2RX

//! uDMA SSI2TX channel number
#define SSI2TX_CH 13

//! uDMA SSI2TX channel number mask
#define SSI2TX_CH_M (1 << SSI2TX_CH)

//! uDMA SSI2TX channel asignment
#define SSI2TX_ASGN UDMA_CH13_SSI2TX

//! Interrupt Service Routine for Port A.
//! Handles interrupts from IRQ and MI0/SYNC pins
void portA_ISR()
{
   int maskedStatus = GPIOIntStatus(GPIO_PORTA_BASE, true);
   assert(maskedStatus & (IRQ_PIN | MI0_SYNC_PIN)); // valid interrupt occured
   GPIOIntClear(GPIO_PORTA_BASE, IRQ_PIN | MI0_SYNC_PIN);

   UARTprintf("Interrupt occured, status=%x\n", maskedStatus);
   ABCC_ISR();
}

//! Interrupt Service Routine for SSI2
//! It will be invoked, when DMA has finished either TX or RX
//! When RX has been finished, `spiDataReceivedCb` will be called.
void ssi2_ISR()
{
   assert(SSIIntStatus(SSI2_BASE, true) == 0); // only DMA interrupts allowed
   uint32_t dmaIntStatus = uDMAIntStatus();
   assert(dmaIntStatus & (SSI2RX_CH_M | SSI2TX_CH_M)); // valid DMA interrupt occur

   uint32_t dmaIntClearMask = 0;

   if(dmaIntStatus & SSI2RX_CH_M)
   {
      // DMA SSIRX transfer completed. Invoke the callback to the ABCC
      assert(spiDataReceivedCb);
      spiDataReceivedCb();
      dmaIntClearMask |= SSI2RX_CH_M;
   }

   if(dmaIntStatus & SSI2TX_CH_M)
   {
      // DMA SSITX transfer completed. Just clear the interrupt flag
      dmaIntClearMask |= SSI2TX_CH_M;
   }

   if(dmaIntClearMask == (SSI2TX_CH_M | SSI2RX_CH_M))
   {
      // If SSIRX transfer was completed right after SSITX, and during handling
      // interrupt of SSITX, we need to clear pending interrupt of SSIRX,
      // because it is arleady handled
      IntPendClear(INT_SSI2);
   }

   assert(dmaIntClearMask);
   uDMAIntClear(dmaIntClearMask);
}

//! Performs hardware initialization to work with anybus module.
//! Initializes GPIO pins, DMA and SPI
BOOL ABCC_SYS_HwInit()
{
   // Enable GPIOs peripherals clocks
   MAP_SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOA);
   MAP_SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOB);

   // Configure Reset pin as output
   MAP_GPIOPinTypeGPIOOutput(GPIO_PORTB_BASE, RESET_PIN);

   // Configure Module detection pin 0 as input
   MAP_GPIOPinTypeGPIOInput(GPIO_PORTB_BASE, MD0_PIN);

   // Configure IRQ pin to be interrupt driven input
   MAP_GPIOPinTypeGPIOInput(GPIO_PORTA_BASE, IRQ_PIN);
   MAP_GPIOIntTypeSet(GPIO_PORTA_BASE, IRQ_PIN, GPIO_FALLING_EDGE);

   // Configure MI1 pin as input
   MAP_GPIOPinTypeGPIOInput(GPIO_PORTB_BASE, MI1_PIN);

   // Configure MI0/Sync pin to be interrupt driven input
   MAP_GPIOPinTypeGPIOInput(GPIO_PORTA_BASE, MI0_SYNC_PIN);
   MAP_GPIOIntTypeSet(GPIO_PORTA_BASE, MI0_SYNC_PIN, GPIO_RISING_EDGE);

   // Register interrupts for Port A
   GPIOIntRegister(GPIO_PORTA_BASE, portA_ISR);

   // Enable uDMA and configure its control table
   MAP_SysCtlPeripheralEnable(SYSCTL_PERIPH_UDMA);
   MAP_uDMAEnable();
   MAP_uDMAControlBaseSet(dmaControlTable);

   // Configure SSI2RX uDMA channel:
   // - Source address fixed (SSI2RX FIFO)
   // - Destination address increments by byte (MISO frame)
   MAP_uDMAChannelControlSet(SSI2RX_CH | UDMA_PRI_SELECT,
      UDMA_SIZE_8 | UDMA_SRC_INC_NONE | UDMA_DST_INC_8 | UDMA_ARB_4);
   MAP_uDMAChannelAssign(SSI2RX_ASGN);
   MAP_uDMAChannelAttributeDisable(SSI2RX_CH, UDMA_ATTR_REQMASK);

   // Configure SSI2TX uDMA channel:
   // - Source address increments by byte (MOSI frame)
   // - Destination address fixed (SSI2TX FIFO)
   MAP_uDMAChannelControlSet(SSI2TX_CH | UDMA_PRI_SELECT,
      UDMA_SIZE_8 | UDMA_SRC_INC_8 | UDMA_DST_INC_NONE | UDMA_ARB_4);
   MAP_uDMAChannelAssign(SSI2TX_ASGN);
   MAP_uDMAChannelAttributeDisable(SSI2TX_CH, UDMA_ATTR_REQMASK);

   // Configure GPIO of pins of SSI2 module.
   MAP_GPIOPinTypeSSI(GPIO_PORTB_BASE,
      GPIO_PIN_4 | GPIO_PIN_5 | GPIO_PIN_6 | GPIO_PIN_7);
   MAP_GPIOPinConfigure(GPIO_PB4_SSI2CLK);
   MAP_GPIOPinConfigure(GPIO_PB5_SSI2FSS);
   MAP_GPIOPinConfigure(GPIO_PB6_SSI2RX);
   MAP_GPIOPinConfigure(GPIO_PB7_SSI2TX);

   // configure SSI2: SPI3 mode, master, 5MHz and 8bits frame width, DMA RX+TX
   MAP_SysCtlPeripheralEnable(SYSCTL_PERIPH_SSI2);
   MAP_SSIConfigSetExpClk(SSI2_BASE, MAP_SysCtlClockGet(),
      SSI_FRF_MOTO_MODE_3, SSI_MODE_MASTER, 12500000, 8);
   MAP_SSIDMAEnable(SSI2_BASE, SSI_DMA_TX | SSI_DMA_RX);
   SSIIntRegister(SSI2_BASE, ssi2_ISR);
   SSIEnable(SSI2_BASE);

   return true;
}

//! Should perform additional to HwInit work. In this case, does nothing.
BOOL ABCC_SYS_Init()
{
   return true;
}

//! Should release all alocated resources. In this case, does nothing.
void ABCC_SYS_Close()
{
   /* do nothing */
}

//! Sets Reset pin to LOW
void ABCC_SYS_HWReset()
{
   GPIOPinWrite(GPIO_PORTB_BASE, RESET_PIN, 0);
}

//! Sets Reset pin to HIGH
void ABCC_SYS_HWReleaseReset()
{
   GPIOPinWrite(GPIO_PORTB_BASE, RESET_PIN, RESET_PIN);
}

//! Reads states of MI0 and MI1 pins and returns received Module ID
UINT8 ABCC_SYS_ReadModuleId()
{
   const int mi0State = GPIOPinRead(GPIO_PORTA_BASE, MI0_SYNC_PIN);
   const int mi1State = GPIOPinRead(GPIO_PORTB_BASE, MI1_PIN);

   int result = (mi0State ? 0x1 : 0) | (mi1State ? 0x2 : 0);
   return result;
}

//! Performs module detection, by checking MD0 pin
//! If it is LOW, then module is available, otherwise not.
BOOL ABCC_SYS_ModuleDetect()
{
   const int md0State = GPIOPinRead(GPIO_PORTB_BASE, MD0_PIN);
   const bool moduleDetected = (md0State ? false : true);
   return moduleDetected;
}

// void ABCC_SYS_SyncInterruptEnable()
// {
//    /*
//    ** Implement according to abcc_sys_adapt.h
//    */
// }

// void ABCC_SYS_SyncInterruptDisable()
// {
//    /*
//    ** Implement according to abcc_sys_adapt.h
//    */
// }

//! Enables interrupt from IRQ pin
void ABCC_SYS_AbccInterruptEnable()
{
   GPIOIntEnable(GPIO_PORTA_BASE, IRQ_PIN);
}

//! Disables interrupt from IRQ pin
void ABCC_SYS_AbccInterruptDisable()
{
   GPIOIntDisable(GPIO_PORTA_BASE, IRQ_PIN);
}

//! Registers callback to be called after MISO frame receive.
void ABCC_SYS_SpiRegDataReceived(ABCC_SYS_SpiDataReceivedCbfType pnDataReceived )
{
   spiDataReceivedCb = pnDataReceived;
}

//! Sends MOSI frame and simultaneously receives MISO frame using DMA.
//! At the end, SSI2/DMA ISR will invoke `spiDataReceivedCb` callback
void ABCC_SYS_SpiSendReceive(void* pxSendDataBuffer, void* pxReceiveDataBuffer, UINT16 iLength)
{
   assert(iLength < 1024); // valid length to use DMA
   assert(!SSIBusy(SSI2_BASE));

   // Prepare SSIRX DMA channel buffers. Source=SSIRX, Destination=MISO frame
   void* rxSrcBuffer = (void*)(SSI_O_DR + SSI2_BASE);
   void* rxDstBuffer = ((uint8_t*)(pxReceiveDataBuffer));

   // Configure SSIRX DMA channel to receive MISO frame
   uDMAChannelTransferSet(SSI2RX_CH | UDMA_PRI_SELECT, UDMA_MODE_BASIC,
      rxSrcBuffer, rxDstBuffer, iLength);

   // Prepare SSITX DMA channel buffer. Source=MOSI frame, Destination=SSITX
   void* txSrcBuffer = ((uint8_t*)(pxSendDataBuffer));
   void* txDstBuffer = (void*)(SSI_O_DR + SSI2_BASE);

   // Configure SSITX DMA channel to transmit MOSI frame
   uDMAChannelTransferSet(SSI2TX_CH | UDMA_PRI_SELECT, UDMA_MODE_BASIC,
      txSrcBuffer, txDstBuffer, iLength);

   // Enable SSIRX and then SSITX DMA channels
   uDMAChannelEnable(SSI2RX_CH);
   uDMAChannelEnable(SSI2TX_CH);
}
