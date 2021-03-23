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

#ifndef TM4C_INTERRUPT_H_
#define TM4C_INTERRUPT_H_

#include <stdint.h>

namespace Interrupt {

enum class Priority :
		uint8_t
{
	Uber = 0,
	Higher = 1,
	High = 2,
	Mediumer = 3,
	Medium = 4,
	Low = 5,
	Lower = 6,
	Lowest = 7
};

class VectorTable
{
public:
	static bool attach(uint8_t vector, void (*isr)(void));
	static void detach(uint8_t vector);

	static void enable();
	static void disable();

	static void enable(uint8_t vector, Priority prio = Priority::Medium);
	static void disable(uint8_t vector);

	static void trigger(uint8_t vector);

private:
	VectorTable();
};

} // namespace Vector

#endif /* TM4C_INTERRUPT_H_ */
