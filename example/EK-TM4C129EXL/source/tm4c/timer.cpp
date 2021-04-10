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

#include <algorithm>
#include <numeric>

#include "tm4c/clock.h"

#include "inc/hw_ints.h"
#include "inc/hw_memmap.h"
#include "inc/hw_timer.h"
#include "inc/hw_types.h"

#include "driverlib/interrupt.h"
#include "driverlib/sysctl.h"
#include "driverlib/timer.h"

#include "tm4c/interrupt.h"
#include "tm4c/timer.h"

namespace TM4C {
namespace Timer {

Base::Base(uint8_t number) :
		_number(number)
{
	assert(_number < COUNT);
}

uint8_t Base::number() const
{
	return _number;
}

uint8_t Base::vector(Channel channel) const
{
	return _vector[number()][(unsigned int)channel];
}

uint32_t Base::peripheral() const
{
	return _peripheral[number()];
}

uint32_t Base::base() const
{
	return _base[number()];
}

SingleShot::SingleShot(uint8_t number) :
		Base(number)
{
	while(!SysCtlPeripheralReady(peripheral()))
	{
		SysCtlPeripheralEnable(peripheral());
	}
}

SingleShot::~SingleShot()
{
	SysCtlPeripheralDisable(peripheral());
}

void SingleShot::start()
{
	TimerClockSourceSet(base(), TIMER_CLOCK_SYSTEM);
	TimerConfigure(base(), (TIMER_CFG_ONE_SHOT));
	TimerControlStall(base(), TIMER_A, true);

	Interrupt::VectorTable::enable(vector(Channel::A));
}

void SingleShot::stop()
{
	Interrupt::VectorTable::disable(vector(Channel::A));

	TimerIntDisable(base(), TIMER_TIMA_TIMEOUT);
	TimerDisable(base(), TIMER_A);
}

void SingleShot::run()
{
	uint16_t periodMs;
	if(inStart.receive(periodMs))
	{
		if(periodMs != 0)
		{
			TimerIntDisable(base(), TIMER_TIMA_TIMEOUT);
			TimerDisable(base(), TIMER_A);
			TimerLoadSet(base(), TIMER_A, Clock::instance().getFrequency() / 1000 * periodMs);
			TimerIntEnable(base(), TIMER_TIMA_TIMEOUT);

			TimerEnable(base(), TIMER_A);
		}
		else
		{
			TimerIntDisable(base(), TIMER_TIMA_TIMEOUT);
			TimerDisable(base(), TIMER_A);
		}
	}
}

void SingleShot::isr()
{
	uint32_t status = TimerIntStatus(base(), true);
	TimerIntClear(base(), status);

	if(status & TIMER_TIMA_TIMEOUT)
	{
		outTimeout.send();
	}
}

void SingleShot::trigger()
{
	Interrupt::VectorTable::trigger(vector(Channel::A));
}

Continuous::Continuous(uint8_t number, uint16_t periodMs) :
		Base(number),
		periodMs(periodMs)
{
	while(!SysCtlPeripheralReady(peripheral()))
	{
		SysCtlPeripheralEnable(peripheral());
	}
}

Continuous::~Continuous()
{
	SysCtlPeripheralDisable(peripheral());
}

void Continuous::start()
{
	TimerClockSourceSet(base(), TIMER_CLOCK_SYSTEM);
	TimerConfigure(base(), (TIMER_CFG_PERIODIC));
	TimerControlStall(base(), TIMER_A, true);

	TimerIntDisable(base(), TIMER_TIMA_TIMEOUT);
	TimerDisable(base(), TIMER_A);
	TimerLoadSet(base(), TIMER_A, Clock::instance().getFrequency() / 1000 * periodMs);
	TimerIntEnable(base(), TIMER_TIMA_TIMEOUT);

	Interrupt::VectorTable::enable(vector(Channel::A));

	TimerEnable(base(), TIMER_A);
}

void Continuous::stop()
{
	Interrupt::VectorTable::disable(vector(Channel::A));

	TimerIntDisable(base(), TIMER_TIMA_TIMEOUT);
	TimerDisable(base(), TIMER_A);
}

void Continuous::run()
{
}

void Continuous::isr()
{
	uint32_t status = TimerIntStatus(base(), true);
	TimerIntClear(base(), status);

	if(status & TIMER_TIMA_TIMEOUT)
	{
		outTimeout.send();
	}
}

void Continuous::trigger()
{
	Interrupt::VectorTable::trigger(vector(Channel::A));
}

const uint8_t Base::_vector[COUNT][(unsigned int)Channel::COUNT] =
{
	{ INT_TIMER0A, INT_TIMER0B },
	{ INT_TIMER1A, INT_TIMER1B },
	{ INT_TIMER2A, INT_TIMER2B },
	{ INT_TIMER3A, INT_TIMER3B },
	{ INT_TIMER4A, INT_TIMER4B },
	{ INT_TIMER5A, INT_TIMER5B },
	{ INT_TIMER6A, INT_TIMER6B },
	{ INT_TIMER7A, INT_TIMER7B }
};

const uint32_t Base::_peripheral[] =
{
    SYSCTL_PERIPH_TIMER0,
    SYSCTL_PERIPH_TIMER1,
    SYSCTL_PERIPH_TIMER2,
    SYSCTL_PERIPH_TIMER3,
    SYSCTL_PERIPH_TIMER4,
    SYSCTL_PERIPH_TIMER5,
    SYSCTL_PERIPH_TIMER6,
    SYSCTL_PERIPH_TIMER7
};

const uint32_t Base::_base[] =
{
	TIMER0_BASE,
    TIMER1_BASE,
    TIMER2_BASE,
    TIMER3_BASE,
    TIMER4_BASE,
    TIMER5_BASE,
    TIMER6_BASE,
    TIMER7_BASE
};

} // namespace Timer
} // namespace TM4C
