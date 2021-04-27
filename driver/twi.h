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

#ifndef FLOW_DRIVER_TWI_H_
#define FLOW_DRIVER_TWI_H_

#include <stdint.h>

#include "driver/isr.h"

namespace Flow {
namespace Driver {

/**
 * \brief Name space for all two wire interface (TWI) related classes.
 *
 * This contains target agnostic implementations and interfaces
 * to which target specific peripherals should adhere.
 */
namespace TWI
{

enum class Direction : uint_fast16_t
{
	TRANSMIT = 0, RECEIVE = 1
};

/**
 * \brief Interface to be implemented by a target specific TWI peripheral.
 *
 * The TWI::Bus interacts through this interface with
 * a target specific TWI peripheral.
 */
class Peripheral:
        virtual public Flow::Driver::WithISR
{
public:
	/**
	 * \brief The state of the peripheral.
	 */
	enum class State
	{
		INIT, ADDRESS, DATA, IDLE, NACK
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

	/**
	 * \brief Perform a TWI operation.
	 *
	 * \param address 7 bit slave address. Bit 6..0 must contain the address.
	 * \param direction Read or write operation.
	 * \param length The length of the data buffer.
	 * \param data The buffer with data to transmit or to store received data in, depending on the direction.
	 *
	 * \return Operation request was successful.
	 */
	virtual bool transceive(uint8_t address, Direction direction, uint32_t length, uint8_t* data)
	{
		(void)address;
		(void)direction;
		(void)length;
		(void)data;
		// Should be overloaded.
		assert(false);
	}

	/**
	 * \brief Get the peripheral status.
	 *
	 * \return Peripheral status.
	 */
	virtual State status() const
	{
		return state;
	}

	virtual void clearNack()
	{
		if(state == State::NACK)
		{
			state = State::IDLE;
		}
	}

protected:
	State state = State::INIT;
};

} // namespace TWI
} // namespace Driver
} // namespace Flow

#endif /* FLOW_DRIVER_TWI_H_ */
