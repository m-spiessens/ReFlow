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

#include <stdint.h>

#include "nrf/interrupt.h"

extern "C" void (*__ramVectors[])(void);
extern "C" void isrNotExpected(void);

namespace Interrupt {

VectorTable::VectorTable()
{
}

bool VectorTable::attach(IRQn_Type vector, void (*isr)(void))
{
	__ramVectors[IRQ_BASE + vector] = isr;

	return true;
}

void VectorTable::detach(IRQn_Type vector)
{
	__ramVectors[IRQ_BASE + vector] = isrNotExpected;
}

void VectorTable::enable()
{
	__enable_irq();
}

void VectorTable::disable()
{
	__disable_irq();
}

void VectorTable::enable(IRQn_Type vector, Priority prio)
{
	NVIC_SetPriority(vector, (uint8_t)prio);
	NVIC_EnableIRQ(vector);
}

void VectorTable::disable(IRQn_Type vector)
{
	NVIC_DisableIRQ(vector);
}

void VectorTable::trigger(IRQn_Type vector)
{
	NVIC_SetPendingIRQ(vector);
}

} // namespace Interrupt
