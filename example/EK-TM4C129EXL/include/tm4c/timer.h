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

#ifndef TM4C_TIMER_H_
#define TM4C_TIMER_H_

#include "driver/timer.h"

namespace TM4C {
namespace Timer {

enum class Number :
		unsigned int
{
	_0 = 0,
	_1 = 1,
	_2 = 2,
	_3 = 3,
	_4 = 4,
	_5 = 5,
	_6 = 6,
	_7 = 7,
	COUNT
};

enum class Channel :
		unsigned int
{
	A = 0,
	B = 1,
	COUNT
};

class Base
{
public:
	Base(Number number);

	uint8_t vector(Channel channel) const;

protected:
    Number number() const;
	uint32_t peripheral() const;
	uint32_t base() const;

private:
	const Number _number;
	static const uint8_t _vector[(unsigned int)Number::COUNT][(unsigned int)Channel::COUNT];
	static const uint32_t _peripheral[(unsigned int)Number::COUNT];
	static const uint32_t _base[(unsigned int)Number::COUNT];
};

class Continuous :
		public Flow::Driver::Timer::Continuous,
		public Base
{
public:
	Flow::OutPort<void> outTimeout;

    Continuous(Number number, uint16_t periodMs);
    ~Continuous();

    void start() final override;
    void stop() final override;

    void run() override;

    void isr() final override;

private:
    void trigger() final override;

	uint16_t periodMs;
};

} // namespace Timer
} // namespace TM4C

#endif /* TM4C_TIMER_H_ */
