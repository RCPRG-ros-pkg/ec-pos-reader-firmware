/**
 * startup.cpp
 * File with startup configuration of MCU
 * Author: akowalew
 */

#include <cstdlib>

extern "C" {

// configure clock
extern void preinitHardware();

// configure IO
extern void initIO();

// configure hardware peripherials
extern void initHardware();

// The entry point for the application.
extern int main();

// Forward declaration of the default fault handlers.
void resetISR(void);
void nmISR(void);
void faultISR(void);
void defaultISR(void);

// // initial value of stack pointer
extern unsigned long __stack;

__attribute__((section(".stack_start_value"),used))
const unsigned long stackBegin = (unsigned long)&__stack;

// The vector table.
__attribute__((section(".vectors"),used))
void (* const g_pfnVectors[])(void) =
{
    resetISR,                        // The reset handler
    nmISR,                           // The NMI handler
    faultISR,                        // The hard fault handler
    defaultISR,                      // The MPU fault handler
    defaultISR,                      // The bus fault handler
    defaultISR,                      // The usage fault handler
    0,                               // Reserved
    0,                               // Reserved
    0,                               // Reserved
    0,                               // Reserved
    defaultISR,                      // SVCall handler
    defaultISR,                      // Debug monitor handler
    0,                               // Reserved
    defaultISR,                      // The PendSV handler
    defaultISR,                      // The SysTick handler
    defaultISR,                      // GPIO Port A
    defaultISR,                      // GPIO Port B
    defaultISR,                      // GPIO Port C
    defaultISR,                      // GPIO Port D
    defaultISR,                      // GPIO Port E
    defaultISR,                      // UART0 Rx and Tx
    defaultISR,                      // UART1 Rx and Tx
    defaultISR,                      // SSI0 Rx and Tx
    defaultISR,                      // I2C0 Master and Slave
    defaultISR,                      // PWM Fault
    defaultISR,                      // PWM Generator 0
    defaultISR,                      // PWM Generator 1
    defaultISR,                      // PWM Generator 2
    defaultISR,                      // Quadrature Encoder 0
    defaultISR,                      // ADC Sequence 0
    defaultISR,                      // ADC Sequence 1
    defaultISR,                      // ADC Sequence 2
    defaultISR,                      // ADC Sequence 3
    defaultISR,                      // Watchdog timer
    defaultISR,                      // Timer 0 subtimer A
    defaultISR,                      // Timer 0 subtimer B
    defaultISR,                      // Timer 1 subtimer A
    defaultISR,                      // Timer 1 subtimer B
    defaultISR,                      // Timer 2 subtimer A
    defaultISR,                      // Timer 2 subtimer B
    defaultISR,                      // Analog Comparator 0
    defaultISR,                      // Analog Comparator 1
    defaultISR,                      // Analog Comparator 2
    defaultISR,                      // System Control (PLL, OSC, BO)
    defaultISR,                      // FLASH Control
    defaultISR,                      // GPIO Port F
    defaultISR,                      // GPIO Port G
    defaultISR,                      // GPIO Port H
    defaultISR,                      // UART2 Rx and Tcx
    defaultISR,                      // SSI1 Rx and Tx
    defaultISR,                      // Timer 3 subtimer A
    defaultISR,                      // Timer 3 subtimer B
    defaultISR,                      // I2C1 Master and Slave
    defaultISR,                      // Quadrature Encoder 1
    defaultISR,                      // CAN0
    defaultISR,                      // CAN1
    defaultISR,                      // CAN2
    defaultISR,                      // Ethernet
    defaultISR,                      // Hibernate
    defaultISR,                      // USB0
    defaultISR,                      // PWM Generator 3
    defaultISR,                      // uDMA Software Transfer
    defaultISR,                      // uDMA Error
    defaultISR,                      // ADC1 Sequence 0
    defaultISR,                      // ADC1 Sequence 1
    defaultISR,                      // ADC1 Sequence 2
    defaultISR,                      // ADC1 Sequence 3
    defaultISR,                      // I2S0
    defaultISR,                      // External Bus Interface 0
    defaultISR,                      // GPIO Port J
    defaultISR,                      // GPIO Port K
    defaultISR,                      // GPIO Port L
    defaultISR,                      // SSI2 Rx and Tx
    defaultISR,                      // SSI3 Rx and Tx
    defaultISR,                      // UART3 Rx and Tx
    defaultISR,                      // UART4 Rx and Tx
    defaultISR,                      // UART5 Rx and Tx
    defaultISR,                      // UART6 Rx and Tx
    defaultISR,                      // UART7 Rx and Tx
    0,                               // Reserved
    0,                               // Reserved
    0,                               // Reserved
    0,                               // Reserved
    defaultISR,                      // I2C2 Master and Slave
    defaultISR,                      // I2C3 Master and Slave
    defaultISR,                      // Timer 4 subtimer A
    defaultISR,                      // Timer 4 subtimer B
    0,                               // Reserved
    0,                               // Reserved
    0,                               // Reserved
    0,                               // Reserved
    0,                               // Reserved
    0,                               // Reserved
    0,                               // Reserved
    0,                               // Reserved
    0,                               // Reserved
    0,                               // Reserved
    0,                               // Reserved
    0,                               // Reserved
    0,                               // Reserved
    0,                               // Reserved
    0,                               // Reserved
    0,                               // Reserved
    0,                               // Reserved
    0,                               // Reserved
    0,                               // Reserved
    0,                               // Reserved
    defaultISR,                      // Timer 5 subtimer A
    defaultISR,                      // Timer 5 subtimer B
    defaultISR,                      // Wide Timer 0 subtimer A
    defaultISR,                      // Wide Timer 0 subtimer B
    defaultISR,                      // Wide Timer 1 subtimer A
    defaultISR,                      // Wide Timer 1 subtimer B
    defaultISR,                      // Wide Timer 2 subtimer A
    defaultISR,                      // Wide Timer 2 subtimer B
    defaultISR,                      // Wide Timer 3 subtimer A
    defaultISR,                      // Wide Timer 3 subtimer B
    defaultISR,                      // Wide Timer 4 subtimer A
    defaultISR,                      // Wide Timer 4 subtimer B
    defaultISR,                      // Wide Timer 5 subtimer A
    defaultISR,                      // Wide Timer 5 subtimer B
    defaultISR,                      // FPU
    defaultISR,                      // PECI 0
    defaultISR,                      // LPC 0
    defaultISR,                      // I2C4 Master and Slave
    defaultISR,                      // I2C5 Master and Slave
    defaultISR,                      // GPIO Port M
    defaultISR,                      // GPIO Port N
    defaultISR,                      // Quadrature Encoder 2
    defaultISR,                      // Fan 0
    0,                               // Reserved
    defaultISR,                      // GPIO Port P (Summary or P0)
    defaultISR,                      // GPIO Port P1
    defaultISR,                      // GPIO Port P2
    defaultISR,                      // GPIO Port P3
    defaultISR,                      // GPIO Port P4
    defaultISR,                      // GPIO Port P5
    defaultISR,                      // GPIO Port P6
    defaultISR,                      // GPIO Port P7
    defaultISR,                      // GPIO Port Q (Summary or Q0)
    defaultISR,                      // GPIO Port Q1
    defaultISR,                      // GPIO Port Q2
    defaultISR,                      // GPIO Port Q3
    defaultISR,                      // GPIO Port Q4
    defaultISR,                      // GPIO Port Q5
    defaultISR,                      // GPIO Port Q6
    defaultISR,                      // GPIO Port Q7
    defaultISR,                      // GPIO Port R
    defaultISR,                      // GPIO Port S
    defaultISR,                      // PWM 1 Generator 0
    defaultISR,                      // PWM 1 Generator 1
    defaultISR,                      // PWM 1 Generator 2
    defaultISR,                      // PWM 1 Generator 3
    defaultISR                       // PWM 1 Fault
};

extern void __libc_init_array();

/**
 * @brief Reset interrupt service routine
 * @details It will be called after hardware reset.
 *  It initializes most important things, before starting 'main'
 */
void
resetISR(void)
{
    preinitHardware();

    // Copy the data segment initializers from flash to SRAM.
    extern unsigned long __etext;
    extern unsigned long __data_start__;
    extern unsigned long __data_end__;
    auto idata = &__etext;
    auto data = &__data_start__;
    const auto edata = &__data_end__;
    while(data < edata)
    {
        *(data++) = *(idata++);
    }

    // Zero fill the bss segment.
    extern unsigned long __bss_start__;
    extern unsigned long __bss_end__;
    auto bss = &__bss_start__;
    const auto ebss = &__bss_end__;
    while(bss < ebss)
    {
        *(bss++) = 0;
    }

    initIO();

    initHardware();

    __libc_init_array();

    // Call the application's entry point.
    auto code = main();

    std::exit(code);

    while(1)
    {
        // this code will never be executed
    }
}

/**
 * @brief Non maskable interrupt service routine
 * @details It will be called, when processor receives Non Maskable Interrupt
 */
void
nmISR(void)
{
    while(1)
    {
    }
}

/**
 * @brief Fault interrupt service routine
 * @details It will be called, when processor goes into fault state
 */
void
faultISR(void)
{
    while(1)
    {
    }
}

/**
 * @brief Default interrupt service routine
 * @details It will be called, when processor receives interrupt,
 *  but no handler is attached to it.
 */
void
defaultISR(void)
{
    while(1)
    {
    }
}

}
