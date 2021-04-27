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

#include "nrf/interrupt.h"
#include "nrf/timer.h"

#include "nrf_timer.h"

namespace nRF {
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

NRF_TIMER_Type* Base::peripheral() const
{
	return _peripheral[number()];
}

IRQn_Type Base::vector() const
{
	return _vector[number()];
}

SingleShot::SingleShot(uint8_t number) :
		Base(number)
{
}

SingleShot::~SingleShot()
{
}

void SingleShot::start()
{
	nrf_timer_mode_set(peripheral(), NRF_TIMER_MODE_TIMER);
	nrf_timer_bit_width_set(peripheral(), NRF_TIMER_BIT_WIDTH_32);
	nrf_timer_frequency_set(peripheral(), NRF_TIMER_FREQ_125kHz);

	Interrupt::VectorTable::enable(vector());
}

void SingleShot::stop()
{
	Interrupt::VectorTable::disable(vector());

	nrf_timer_int_disable(peripheral(), NRF_TIMER_INT_COMPARE0_MASK);
	nrf_timer_task_trigger(peripheral(), NRF_TIMER_TASK_STOP);
}

void SingleShot::run()
{
	uint16_t periodMs;
	if(inStart.receive(periodMs))
	{
		if(periodMs != 0)
		{
			nrf_timer_int_disable(peripheral(), NRF_TIMER_INT_COMPARE0_MASK);
			nrf_timer_cc_set(peripheral(), NRF_TIMER_CC_CHANNEL0, 125 * periodMs);
			nrf_timer_int_enable(peripheral(), NRF_TIMER_INT_COMPARE0_MASK);

			Interrupt::VectorTable::enable(vector());

			nrf_timer_task_trigger(peripheral(), NRF_TIMER_TASK_CLEAR);
			nrf_timer_task_trigger(peripheral(), NRF_TIMER_TASK_START);
		}
		else
		{
			nrf_timer_int_disable(peripheral(), NRF_TIMER_INT_COMPARE0_MASK);
			nrf_timer_task_trigger(peripheral(), NRF_TIMER_TASK_STOP);
		}
	}
}

void SingleShot::isr()
{
	if(nrf_timer_event_check(peripheral(), NRF_TIMER_EVENT_COMPARE0))
	{
		outTimeout.send();
	}
}

void SingleShot::trigger()
{
	Interrupt::VectorTable::trigger(vector());
}

Continuous::Continuous(uint8_t number, uint16_t periodMs) :
		Base(number),
		periodMs(periodMs)
{
}

Continuous::~Continuous()
{
}

void Continuous::start()
{
	nrf_timer_mode_set(peripheral(), NRF_TIMER_MODE_TIMER);
	nrf_timer_bit_width_set(peripheral(), NRF_TIMER_BIT_WIDTH_32);
	nrf_timer_frequency_set(peripheral(), NRF_TIMER_FREQ_125kHz);

	nrf_timer_cc_set(peripheral(), NRF_TIMER_CC_CHANNEL0, 125 * periodMs);
	nrf_timer_int_enable(peripheral(), NRF_TIMER_INT_COMPARE0_MASK);

	Interrupt::VectorTable::enable(vector());

	nrf_timer_task_trigger(peripheral(), NRF_TIMER_TASK_START);
}

void Continuous::stop()
{
	Interrupt::VectorTable::disable(vector());

	nrf_timer_int_disable(peripheral(), NRF_TIMER_INT_COMPARE0_MASK);
	nrf_timer_task_trigger(peripheral(), NRF_TIMER_TASK_STOP);
}

void Continuous::run()
{
}

void Continuous::isr()
{
	if(nrf_timer_event_check(peripheral(), NRF_TIMER_EVENT_COMPARE0))
	{
		nrf_timer_event_clear(peripheral(), NRF_TIMER_EVENT_COMPARE0);
		nrf_timer_task_trigger(peripheral(), NRF_TIMER_TASK_CLEAR);
		
		outTimeout.send();
	}
}

void Continuous::trigger()
{
	Interrupt::VectorTable::trigger(vector());
}

NRF_TIMER_Type* const Base::_peripheral[] =
{
    NRF_TIMER0_S,
    NRF_TIMER1_S,
    NRF_TIMER2_S
};

const IRQn_Type Base::_vector[] =
{
	TIMER0_IRQn,
	TIMER1_IRQn,
	TIMER2_IRQn
};

} // namespace Timer
} // namespace nRF
