/* The MIT License (MIT)
 *
 * Copyright (c) 2021 Mathias Spiessens
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software, hardware and associated documentation files (the "Solution"), to deal
 * in the Solution without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Solution, and to permit persons to whom the Solution is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Solution.
 *
 * THE SOLUTION IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOLUTION OR THE USE OR OTHER DEALINGS IN THE
 * SOLUTION.
 */

#include <assert.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>

// Forward declaration of the default fault handlers.
void reset(void);
static void nmi(void);
static void faultHard(void);
static void faultMPU(void);
static void faultBus(void);
static void faultUsage(void);
static void isrNotExpected(void);

// The entry point for the application.
extern int main(void);

// System stack start determined by the linker script.
extern unsigned stack __asm("_stack");

// The vector table. Note: the proper constructs must be placed on this to
// ensure that it ends up at physical address 0x0000.0000.
__attribute__ ((section(".vectors")))
void (* const vectors[])(void) =
{
	(void *)&stack,   // The initial stack pointer
	reset,          // The reset handler
	nmi,// Must be serviced by ROM code for IPC to work!!!
	faultHard,		// The hard fault handler
	faultMPU,		// The MPU fault handler
	faultBus,		// The bus fault handler
	faultUsage,		// The usage fault handler
	0,				// Reserved
	0,				// Reserved
	0,				// Reserved
	0,				// Reserved
	isrNotExpected, // SVCall handler
	isrNotExpected, // Debug monitor handler
	0,                 // Reserved
	isrNotExpected, // The PendSV handler
	isrNotExpected, // The SysTick handler
	//
    /* ----------------------  Museic Specific Interrupt Handlers  ---------------------------------------- */
	isrNotExpected, // GPIO Port A
	isrNotExpected, // GPIO Port B
	isrNotExpected, // GPIO Port C
	isrNotExpected, // GPIO Port D
	isrNotExpected, // GPIO Port E
	isrNotExpected, // UART0 Rx and Tx
	isrNotExpected, // UART1 Rx and Tx
	isrNotExpected, // SSI0 Rx and Tx
    isrNotExpected, // I2C0 Master and Slave
    isrNotExpected, // PWM Fault
    isrNotExpected, // PWM Generator 0
    isrNotExpected, // PWM Generator 1
    isrNotExpected, // PWM Generator 2
    isrNotExpected, // Quadrature Encoder 0
    isrNotExpected, // ADC0 Sequence 0
    isrNotExpected, // ADC0 Sequence 1
    isrNotExpected, // ADC0 Sequence 2
    isrNotExpected, // ADC0 Sequence 3
    isrNotExpected, // Watchdog timer
    isrNotExpected, // Timer 0 subtimer A
    isrNotExpected, // Timer 0 subtimer B
    isrNotExpected, // Timer 1 subtimer A
    isrNotExpected, // Timer 1 subtimer B
    isrNotExpected, // Timer 2 subtimer A
    isrNotExpected, // Timer 2 subtimer B
    isrNotExpected, // Analog Comparator 0
    isrNotExpected, // Analog Comparator 1
    isrNotExpected, // Analog Comparator 2
    isrNotExpected, // System Control (PLL, OSC, BO)
    isrNotExpected, // FLASH Control
	isrNotExpected, // GPIO Port F
	isrNotExpected, // GPIO Port G
	isrNotExpected, // GPIO Port H
	isrNotExpected, // UART2 Rx and Tx
    isrNotExpected, // SSI1 Rx and Tx
    isrNotExpected, // Timer 3 subtimer A
    isrNotExpected, // Timer 3 subtimer B
    isrNotExpected, // I2C1 Master and Slave
    isrNotExpected, // CAN0
    isrNotExpected, // CAN1
	isrNotExpected, // Ethernet
    isrNotExpected, // Hibernate
	isrNotExpected, // USB0
    isrNotExpected, // PWM Generator 3
    isrNotExpected, // uDMA Software Transfer
    isrNotExpected, // uDMA Error
    isrNotExpected, // ADC1 Sequence 0
    isrNotExpected, // ADC1 Sequence 1
    isrNotExpected, // ADC1 Sequence 2
    isrNotExpected, // ADC1 Sequence 3
    isrNotExpected, // External Bus Interface 0
    isrNotExpected, // GPIO Port J
	isrNotExpected, // GPIO Port K
	isrNotExpected, // GPIO Port L
	isrNotExpected, // SSI2 Rx and Tx
    isrNotExpected, // SSI3 Rx and Tx
	isrNotExpected, // UART3 Rx and Tx
	isrNotExpected, // UART4 Rx and Tx
	isrNotExpected, // UART5 Rx and Tx
	isrNotExpected, // UART6 Rx and Tx
	isrNotExpected, // UART7 Rx and Tx
    isrNotExpected, // I2C2 Master and Slave
    isrNotExpected, // I2C3 Master and Slave
	isrNotExpected, // Timer 4 subtimer A
    isrNotExpected, // Timer 4 subtimer B
	isrNotExpected, // Timer 5 subtimer A
    isrNotExpected, // Timer 5 subtimer B
    isrNotExpected, // FPU
    0, // Reserved
    0, // Reserved
    isrNotExpected, // I2C4 Master and Slave
    isrNotExpected, // I2C5 Master and Slave
	isrNotExpected, // GPIO Port M
	isrNotExpected, // GPIO Port N
    0, // Reserved
    isrNotExpected, // Tamper
    isrNotExpected, // GPIO Port P (Summary or P0)
	isrNotExpected, // GPIO Port P1
	isrNotExpected, // GPIO Port P2
	isrNotExpected, // GPIO Port P3
	isrNotExpected, // GPIO Port P4
	isrNotExpected, // GPIO Port P5
	isrNotExpected, // GPIO Port P6
	isrNotExpected, // GPIO Port P7
	isrNotExpected, // GPIO Port Q (Summary or Q0)
	isrNotExpected, // GPIO Port Q1
	isrNotExpected, // GPIO Port Q2
	isrNotExpected, // GPIO Port Q3
	isrNotExpected, // GPIO Port Q4
	isrNotExpected, // GPIO Port Q5
	isrNotExpected, // GPIO Port Q6
	isrNotExpected, // GPIO Port Q7
	isrNotExpected, // GPIO Port R
	isrNotExpected, // GPIO Port S
    isrNotExpected, // SHA/MD5 0
    isrNotExpected, // AES 0
    isrNotExpected, // DES3DES 0
    isrNotExpected, // LCD Controller 0
    isrNotExpected, // Timer 6 subtimer A
    isrNotExpected, // Timer 6 subtimer B
    isrNotExpected, // Timer 7 subtimer A
    isrNotExpected, // Timer 7 subtimer B
    isrNotExpected, // I2C6 Master and Slave
    isrNotExpected, // I2C7 Master and Slave
    isrNotExpected, // HIM Scan Matrix Keyboard 0
    isrNotExpected, // One Wire 0
    isrNotExpected, // HIM PS/2 0
    isrNotExpected, // HIM LED Sequencer 0
    isrNotExpected, // HIM Consumer IR 0
    isrNotExpected, // I2C8 Master and Slave
    isrNotExpected, // I2C9 Master and Slave
	isrNotExpected  // GPIO Port T
};

__attribute__ ((aligned(1024)))
__attribute__ ((section(".ramVectors")))
void (*__ramVectors[sizeof(vectors) / sizeof(vectors[0])])(void);

// The following are constructs created by the linker, indicating where the
// the "data" and "bss" segments reside in memory. The initializers for the
// for the "data" segment resides immediately following the "text" segment.
extern unsigned long _etext;
extern unsigned long _data;
extern unsigned long _edata;
extern unsigned long _bss;
extern unsigned long _ebss;
extern void (*__preinit_array_start[])(void);
extern void (*__preinit_array_end[])(void);
extern void (*__init_array_start[])(void);
extern void (*__init_array_end[])(void);
extern void _init(void);

// This is the code that gets called when the processor first starts execution
// following a reset event.  Only the absolutely necessary set is performed,
// after which the application supplied entry() routine is called.  Any fancy
// actions (such as making decisions based on the reset cause register, and
// resetting the bits in that register) are left solely in the hands of the
// application.
void reset(void)
{
	// Fill data with initial values.
	uint32_t* source = &_etext;
	uint32_t* sink = &_data;
	while(sink < &_edata)
	{
		*sink++ = *source++;
	}

	// Zero initialize.
	sink = &_bss;
	while(sink < &_ebss)
	{
		*sink++ = 0;
	}

	// Copy vector table.
	source = (uint32_t*)vectors;
	sink = (uint32_t*)__ramVectors;

	for(unsigned int i = 0; i < sizeof(vectors) / sizeof(vectors[0]); i++)
	{
		*sink++ = *source++;
	}

	// Reset stack pointer.
	__ramVectors[0] = (void*)&stack;

#define VTOR *(uint32_t*)0xE000ED08

	VTOR = (uint32_t)__ramVectors;

    // Call any global C++ constructors.
	uint32_t size = __preinit_array_end - __preinit_array_start;
    for(uint32_t i = 0; i < size; i++)
    {
        __preinit_array_start[i]();
    }

    _init();

    size = __init_array_end - __init_array_start;
    for(uint32_t i = 0; i < size; i++)
    {
        __init_array_start[i]();
    }

    // Run the actual application.
    main();
}

static void nmi(void)
{
	__asm volatile("bkpt");
    while(true);
}

// This is the code that gets called when the processor receives a fault
// interrupt.  This simply enters an infinite loop, preserving the system state
// for examination by a debugger.
static void faultHard(void)
{
	__asm volatile("bkpt");
    while(true);
}

static void faultMPU(void)
{
	__asm volatile("bkpt");
	while(true);
}

static void faultBus(void)
{
	__asm volatile("bkpt");
    while(true);
}

static void faultUsage(void)
{
	__asm volatile("bkpt");
    while(true);
}

// This is the code that gets called when the processor receives an unexpected
// interrupt.  This simply enters an infinite loop, preserving the system state
// for examination by a debugger.
static void isrNotExpected(void)
{
	__asm volatile("bkpt");
    while(true);
}
