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

#ifndef FLOW_DRIVER_UART_H_
#define FLOW_DRIVER_UART_H_

#include "flow/flow.h"

#include "driver/isr.h"

namespace Flow {
namespace Driver {

/**
 * \brief Name space for all UART related classes.
 *
 * This contains target agnostic implementations and interfaces
 * to which target specific peripherals should adhere.
 */
namespace UART {

class Complete
{
public:
	enum class Status
	{
		Idle, TransmitComplete, ReceiveComplete, Error, Overflow
	};
	virtual void complete(Status status)
	{
		(void)status;
		// Should be overloaded.
		assert(false);
	}
};

/**
 * \brief Interface to be implemented by a target specific UART peripheral.
 */
class Peripheral :
		public WithISR
{
public:
	/**
	 * \brief The state of the peripheral.
	 */
	enum class State
	{
		Init, Ready, Overflow
	};

	virtual ~Peripheral() = default;

	/**
	 * \brief Configure and enable the peripheral.
	 */
	virtual void start()
	{
		// Should be overloaded.
		assert(false);
	}

	/**
	 * \brief Disable the peripheral.
	 */
	virtual void stop()
	{
		// Should be overloaded.
		assert(false);
	}

	virtual bool send(const void* const buffer, uint16_t& length)
	{
		(void)buffer;
		(void)length;
		// Should be overloaded.
		assert(false);
		return false;
	}

	virtual bool receive(volatile void* const buffer, uint16_t& length)
	{
		(void)buffer;
		(void)length;
		// Should be overloaded.
		assert(false);
		return false;
	}

	virtual void attach(Complete& complete)
	{
		(void)complete;
		// Should be overloaded.
		assert(false);
	}

	/**
	 * \brief Get the peripheral status.
	 *
	 * \return Peripheral status.
	 */
	virtual State state() const
	{
		return _state;
	}

protected:
	volatile State _state = State::Init;
};

} // namespace UART
} // namespace Driver
} // namespace Flow

#endif /* FLOW_DRIVER_UART_H_ */
