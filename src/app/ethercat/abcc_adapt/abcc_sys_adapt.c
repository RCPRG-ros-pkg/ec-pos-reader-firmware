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

// #define MD0_PIN      GPIO_PIN_1
#define IRQ_PIN      GPIO_PIN_1
#define RESET_PIN    GPIO_PIN_2
#define MI0_SYNC_PIN GPIO_PIN_3
// #define MI1_PIN      GPIO_PIN_3

//! Callback function used to inform ABCC about received MISO frame
static ABCC_SYS_SpiDataReceivedCbfType spiDataReceivedCb = 0;

//! Array for DMA control table. 1KB aligned
__attribute__((aligned(1024)))
static uint8_t dmaControlTable[1024];

//! uDMA SSI1RX channel number
#define SSI1RX_CH 24

//! uDMA SSI1RX channel number mask
#define SSI1RX_CH_M (1 << SSI1RX_CH)

//! uDMA SSI1RX channel asignment
#define SSI1RX_ASGN UDMA_CH24_SSI1RX

//! uDMA SSI1TX channel number
#define SSI1TX_CH 25

//! uDMA SSI1TX channel number mask
#define SSI1TX_CH_M (1 << SSI1TX_CH)

//! uDMA SSI1TX channel asignment
#define SSI1TX_ASGN UDMA_CH25_SSI1TX

//! Interrupt Service Routine for Port E.
//! Handles interrupts from IRQ and MI0/SYNC pins
void portE_ISR()
{
   int maskedStatus = GPIOIntStatus(GPIO_PORTE_BASE, true);
   // UARTprintf("Interrupt occured, status=%x\n", maskedStatus);
   assert(maskedStatus & (IRQ_PIN | MI0_SYNC_PIN)); // valid interrupt occured

   GPIOIntClear(GPIO_PORTE_BASE, IRQ_PIN | MI0_SYNC_PIN);

   if(maskedStatus & MI0_SYNC_PIN)
   {
      // UARTprintf("Sync interrupt\n");
      ABCC_CbfSyncIsr();
   }
   else // (maskedStatus & IRQ_PIN)
   {
      // UARTprintf("IRQ interrupt\n");
      ABCC_ISR();
   }
}

//! Interrupt Service Routine for SSI1
//! It will be invoked, when DMA has finished either TX or RX.
//! When RX has been finished, `spiDataReceivedCb` will be called.
void ssi1_ISR()
{
   assert(SSIIntStatus(SSI1_BASE, true) == 0); // only DMA interrupts allowed
   uint32_t dmaIntStatus = uDMAIntStatus();
   assert(dmaIntStatus & (SSI1RX_CH_M | SSI1TX_CH_M)); // valid DMA interrupt occur

   uint32_t dmaIntClearMask = 0;

   if(dmaIntStatus & SSI1RX_CH_M)
   {
      // DMA SSIRX transfer completed. Invoke the callback to the ABCC
      assert(spiDataReceivedCb);
      spiDataReceivedCb();
      dmaIntClearMask |= SSI1RX_CH_M;
   }

   if(dmaIntStatus & SSI1TX_CH_M)
   {
      // DMA SSITX transfer completed. Just clear the interrupt flag
      dmaIntClearMask |= SSI1TX_CH_M;
   }

   if(dmaIntClearMask == (SSI1TX_CH_M | SSI1RX_CH_M))
   {
      // If SSIRX transfer was completed right after SSITX, and during handling
      // interrupt of SSITX, we need to clear pending interrupt of SSIRX,
      // because it is arleady handled
      IntPendClear(INT_SSI1);
   }

   assert(dmaIntClearMask);
   uDMAIntClear(dmaIntClearMask);
}

//! Performs hardware initialization to work with anybus module.
//! Initializes GPIO pins, DMA and SPI
BOOL ABCC_SYS_HwInit()
{
   // Enable GPIOs peripherals clocks
   MAP_SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOE);
   MAP_SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOD);

   // Configure Reset pin as output
   MAP_GPIOPinTypeGPIOOutput(GPIO_PORTE_BASE, RESET_PIN);

   // // Configure Module detection pin 0 as input
   // MAP_GPIOPinTypeGPIOInput(GPIO_PORTE_BASE, MD0_PIN);

   // Configure IRQ pin to be interrupt driven input
   MAP_GPIOPinTypeGPIOInput(GPIO_PORTE_BASE, IRQ_PIN);
   MAP_GPIOIntTypeSet(GPIO_PORTE_BASE, IRQ_PIN, GPIO_FALLING_EDGE);

   // // Configure MI1 pin as input
   // MAP_GPIOPinTypeGPIOInput(GPIO_PORTE_BASE, MI1_PIN);

   // Configure MI0/Sync pin to be interrupt driven input
   MAP_GPIOPinTypeGPIOInput(GPIO_PORTE_BASE, MI0_SYNC_PIN);
   MAP_GPIOIntTypeSet(GPIO_PORTE_BASE, MI0_SYNC_PIN, GPIO_RISING_EDGE);

   // Register interrupts for Port E
   GPIOIntRegister(GPIO_PORTE_BASE, portE_ISR);

   // Enable uDMA and configure its control table
   MAP_SysCtlPeripheralEnable(SYSCTL_PERIPH_UDMA);
   MAP_uDMAEnable();
   MAP_uDMAControlBaseSet(dmaControlTable);

   // Configure SSI1RX uDMA channel:
   // - Source address fixed (SSI1RX FIFO)
   // - Destination address increments by byte (MISO frame)
   MAP_uDMAChannelControlSet(SSI1RX_CH | UDMA_PRI_SELECT,
      UDMA_SIZE_8 | UDMA_SRC_INC_NONE | UDMA_DST_INC_8 | UDMA_ARB_4);
   MAP_uDMAChannelAssign(SSI1RX_ASGN);
   MAP_uDMAChannelAttributeDisable(SSI1RX_CH, UDMA_ATTR_REQMASK);

   // Configure SSI1TX uDMA channel:
   // - Source address increments by byte (MOSI frame)
   // - Destination address fixed (SSI1TX FIFO)
   MAP_uDMAChannelControlSet(SSI1TX_CH | UDMA_PRI_SELECT,
      UDMA_SIZE_8 | UDMA_SRC_INC_8 | UDMA_DST_INC_NONE | UDMA_ARB_4);
   MAP_uDMAChannelAssign(SSI1TX_ASGN);
   MAP_uDMAChannelAttributeDisable(SSI1TX_CH, UDMA_ATTR_REQMASK);

   // Configure GPIO of pins of SSI1 module.
   MAP_GPIOPinTypeSSI(GPIO_PORTD_BASE,
      GPIO_PIN_0 | GPIO_PIN_1 | GPIO_PIN_2 | GPIO_PIN_3);
   MAP_GPIOPinConfigure(GPIO_PD0_SSI1CLK);
   MAP_GPIOPinConfigure(GPIO_PD1_SSI1FSS);
   MAP_GPIOPinConfigure(GPIO_PD2_SSI1RX);
   MAP_GPIOPinConfigure(GPIO_PD3_SSI1TX);

   // configure SSI1: SPI3 mode, master, 5MHz and 8bits frame width, DMA RX+TX
   MAP_SysCtlPeripheralEnable(SYSCTL_PERIPH_SSI1);
   MAP_SSIConfigSetExpClk(SSI1_BASE, MAP_SysCtlClockGet(),
      SSI_FRF_MOTO_MODE_3, SSI_MODE_MASTER, 12500000, 8);
   MAP_SSIDMAEnable(SSI1_BASE, SSI_DMA_TX | SSI_DMA_RX);
   SSIIntRegister(SSI1_BASE, ssi1_ISR);
   SSIEnable(SSI1_BASE);

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
   GPIOPinWrite(GPIO_PORTE_BASE, RESET_PIN, 0);
}

//! Sets Reset pin to HIGH
void ABCC_SYS_HWReleaseReset()
{
   GPIOPinWrite(GPIO_PORTE_BASE, RESET_PIN, RESET_PIN);
}

//! Reads states of MI0 and MI1 pins and returns received Module ID
UINT8 ABCC_SYS_ReadModuleId()
{
// {
//    const int mi0State = GPIOPinRead(GPIO_PORTE_BASE, MI0_SYNC_PIN);
//    const int mi1State = GPIOPinRead(GPIO_PORTE_BASE, MI1_PIN);

//    int result = (mi0State ? 0x1 : 0) | (mi1State ? 0x2 : 0);
   return 0x2;
}

//! Performs module detection, by checking MD0 pin
//! If it is LOW, then module is available, otherwise not.
BOOL ABCC_SYS_ModuleDetect()
{
   // const int md0State = GPIOPinRead(GPIO_PORTE_BASE, MD0_PIN);
   // const bool moduleDetected = (md0State ? false : true);
   // return moduleDetected;
   return true;
}

void ABCC_SYS_SyncInterruptEnable()
{
   UARTprintf("Enabling SYNC interrupt\n");
   GPIOIntEnable(GPIO_PORTE_BASE, MI0_SYNC_PIN);
}

void ABCC_SYS_SyncInterruptDisable()
{
   UARTprintf("Disabling SYNC interrupt\n");
   GPIOIntDisable(GPIO_PORTE_BASE, MI0_SYNC_PIN);
}

//! Enables interrupt from IRQ pin
void ABCC_SYS_AbccInterruptEnable()
{
   GPIOIntEnable(GPIO_PORTE_BASE, IRQ_PIN);
}

//! Disables interrupt from IRQ pin
void ABCC_SYS_AbccInterruptDisable()
{
   GPIOIntDisable(GPIO_PORTE_BASE, IRQ_PIN);
}

//! Registers callback to be called after MISO frame receive.
void ABCC_SYS_SpiRegDataReceived(ABCC_SYS_SpiDataReceivedCbfType pnDataReceived )
{
   spiDataReceivedCb = pnDataReceived;
}

//! Sends MOSI frame and simultaneously receives MISO frame using DMA.
//! At the end, SSI1/DMA ISR will invoke `spiDataReceivedCb` callback
void ABCC_SYS_SpiSendReceive(void* pxSendDataBuffer, void* pxReceiveDataBuffer, UINT16 iLength)
{
   assert(iLength < 1024); // valid length to use DMA
   assert(!SSIBusy(SSI1_BASE));

   // Prepare SSIRX DMA channel buffers. Source=SSIRX, Destination=MISO frame
   void* rxSrcBuffer = (void*)(SSI_O_DR + SSI1_BASE);
   void* rxDstBuffer = ((uint8_t*)(pxReceiveDataBuffer));

   // Configure SSIRX DMA channel to receive MISO frame
   uDMAChannelTransferSet(SSI1RX_CH | UDMA_PRI_SELECT, UDMA_MODE_BASIC,
      rxSrcBuffer, rxDstBuffer, iLength);

   // Prepare SSITX DMA channel buffer. Source=MOSI frame, Destination=SSITX
   void* txSrcBuffer = ((uint8_t*)(pxSendDataBuffer));
   void* txDstBuffer = (void*)(SSI_O_DR + SSI1_BASE);

   // Configure SSITX DMA channel to transmit MOSI frame
   uDMAChannelTransferSet(SSI1TX_CH | UDMA_PRI_SELECT, UDMA_MODE_BASIC,
      txSrcBuffer, txDstBuffer, iLength);

   // Enable SSIRX and then SSITX DMA channels
   uDMAChannelEnable(SSI1RX_CH);
   uDMAChannelEnable(SSI1TX_CH);
}
