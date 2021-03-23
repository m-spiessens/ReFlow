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

#include "tm4c/interrupt.h"

#include "driverlib/interrupt.h"

extern "C" void (*__ramVectors[])(void);
extern "C" void isrNotExpected(void);

namespace Interrupt {

VectorTable::VectorTable()
{
}

bool VectorTable::attach(uint8_t vector, void (*isr)(void))
{
	__ramVectors[vector] = isr;

	return true;
}

void VectorTable::detach(uint8_t vector)
{
	__ramVectors[vector] = isrNotExpected;
}

void VectorTable::enable()
{
	IntMasterEnable();
}

void VectorTable::disable()
{
	IntMasterDisable();
}

void VectorTable::enable(uint8_t vector, Priority prio)
{
	IntPrioritySet(vector, (uint8_t)prio);
	IntEnable(vector);
}

void VectorTable::disable(uint8_t vector)
{
	IntDisable(vector);
}

void VectorTable::trigger(uint8_t vector)
{
	IntPendSet(vector);
}

} // namespace Interrupt
