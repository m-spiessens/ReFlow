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

#ifndef FLOW_DRIVER_SSI_H_
#define FLOW_DRIVER_SSI_H_

#include "driver/isr.h"

namespace Flow {
namespace Driver {

/**
 * \brief Name space for all SSI related classes.
 *
 * This contains target agnostic implementations and interfaces
 * to which target specific peripherals should adhere.
 */
namespace SSI {
namespace Master {

class Complete
{
public:
	enum class Status
	{
		Success, Error
	};

	virtual void complete(Status status)
	{
		(void)status;
		// Should be overloaded.
		assert(false);
	}
};

/**
 * \brief Interface to be implemented by a target specific SSI peripheral.
 *
 * The SSI::Bus interacts through this interface with
 * a target specific SSI peripheral.
 */
class Peripheral:
		virtual public WithISR
{
public:
	/**
	 * \brief The state of the peripheral.
	 */
	enum class State
	{
		Init, Busy, Ready
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
	 * \brief Perform a full duplex SSI operation.
	 *
	 * \param slave The number of the slave to transceive to.
	 * \param transmit The buffer to be transmitted.
	 * \param transmitLength The lenght of the buffer to be transmitted.
	 * \param receive The buffer to store received data in.
	 * \param receiveLength The length of the buffer to store received data in.
	 *
	 * \return Operation request was successful.
	 */
	virtual bool transceive(uint8_t slave, uint8_t* const transmit,
	        uint16_t transmitLength, uint8_t* const receive,
	        uint16_t receiveLength)
	{
		(void)slave;
		(void)transmit;
		(void)transmitLength;
		(void)receive;
		(void)receiveLength;
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

} // namespace Master
} // namespace SSI
} // namespace Driver
} // namespace Flow

#endif /* FLOW_DRIVER_SSI_H_ */
