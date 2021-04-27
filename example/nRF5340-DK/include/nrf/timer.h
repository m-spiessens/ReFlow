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

#ifndef nRF_TIMER_H_
#define nRF_TIMER_H_

#include "driver/timer.h"

namespace nRF {
namespace Timer {

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
	Base(uint8_t number);

	NRF_TIMER_Type* peripheral() const;
	IRQn_Type vector() const;

protected:
    uint8_t number() const;

private:
	static constexpr uint8_t COUNT = 3;

	const uint8_t _number;

	static NRF_TIMER_Type* const _peripheral[COUNT];
	static const IRQn_Type _vector[COUNT];
};

class SingleShot :
		public Flow::Driver::Timer::SingleShot,
		public Base
{
public:
	SingleShot(uint8_t number);
    ~SingleShot();

    void start() final override;
    void stop() final override;

    void run() override;

    void isr() final override;

private:
    void trigger() final override;
};

class Continuous :
		public Flow::Driver::Timer::Continuous,
		public Base
{
public:
    Continuous(uint8_t number, uint16_t periodMs);
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
} // namespace nRF

#endif /* nRF_TIMER_H_ */
