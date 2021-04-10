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
#include <stdint.h>
#include <string.h>

#include "pinmux/pinout.h"

#include "flow/components.h"
#include "flow/reactor.h"
#include "flow/utility.h"

#include "tm4c/clock.h"
#include "tm4c/digitalio.h"
#include "tm4c/interrupt.h"
#include "tm4c/timer.h"

using namespace TM4C;

// Create the components of the application.
Timer::Continuous timer{ 0, 500 /*ms*/ };
Toggle toggle;
Digital::Output led{ Pin::Port::N, 1, Digital::Polarity::Normal };

int main()
{
	// Set up the clock circuit.
	Clock::instance().configure<Device::TM4C129>(80 MHz);

	// Set up the pin mux configuration.
	PinoutSet();

	// Attach timer interrupt handler.
	Interrupt::VectorTable::attach(timer.vector(Timer::Channel::A), [](){ timer.isr(); });

	// Connect the components of the application.
	Flow::connect(timer.outTimeout, toggle.in);
	Flow::connect(toggle.out, led.inState);

	Flow::Reactor::start();

	Interrupt::VectorTable::enable();

	// Run the application.
	while(true)
	{
		Flow::Reactor::run();
	}
}

// An assert will end up here.
extern "C" void __assert_func(const char *file, int line, const char *pretty,
        const char *condition)
{
	(void) file;
	(void) line;
	(void) pretty;
	(void) condition;

	__asm__ __volatile__("bkpt");
	while(true);
}

typedef char *caddr_t;

volatile char* currentHeapTop = nullptr;

// _sbrk - newlib memory allocation routine.
extern "C" caddr_t _sbrk(int increment)
{
	extern char heap __asm("_heap"); // See linker script.
	extern char eheap __asm("_eheap"); // See linker script.

	if(currentHeapTop == NULL)
	{
		currentHeapTop = &heap;
	}
	volatile char* previousHeapTop = currentHeapTop;

	assert(currentHeapTop + increment < &eheap);

	currentHeapTop += increment;

	return (caddr_t)previousHeapTop;
}
