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

#include "flow/flow.h"

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

/**
 * \brief A TWI operation.
 *
 * This implementation is target agnostic.
 */
class Operation
{
public:
	/**
	 * \brief Create a TWI operation.
	 *
	 * \param slaveSelect The digital output to be asserted during this operation.
	 */
	Operation(uint8_t address) :
		address(address)
	{
	}

	void setDirection(Direction direction)
	{
		this->direction = direction;
	}

	virtual ~Operation() = default;

	/**
	 * \brief Operation status.
	 */
	enum class Status
	{
		TBD, FAIL, SUCCESS
	} status = Status::TBD;

	Direction direction = Direction::RECEIVE;
	uint8_t address;

	uint8_t* data = nullptr;
	uint8_t length = 0;

	uint32_t tag[4];
};

/**
 * \brief TWI bus multiplexer/demultiplexer.
 *
 * This implementation is target agnostic.
 * It uses round robin scheduling if multiple slaves are connected.
 */
template<uint_fast8_t ENDPOINT_COUNT>
class Bus :
		public Flow::Component, 
		public Flow::Driver::WithISR
{
public:
	/**
	 * \brief Create a TWI bus.
	 *
	 * \param peripheral The peripheral this bus is connected to.
	 */
	explicit Bus(Peripheral& peripheral) :
			peripheral(peripheral)
	{
		for(uint_fast8_t i = 0; i < ENDPOINT_COUNT; i++)
		{
			endPoint[i] = new Flow::InOutPort<Operation*>(this);
		}
	}

	/**
	 * \brief The bus end points.
	 *
	 * flow::connect() one or more TWI::Slave to these.
	 */
	Flow::InOutPort<Operation*>* endPoint[ENDPOINT_COUNT];

	/**
	 * \brief Start the TWI bus.
	 *
	 * The TWI peripheral handled by this bus will also be started.
	 */
	void start() final override
	{
		peripheral.start();
	}

	/**
	 * \brief Stop the TWI bus.
	 *
	 * The TWI peripheral handled by this bus will also be halted.
	 */
	void stop() final override
	{
		peripheral.stop();
	}

	/**
	 * \brief Let the bus perform its duty.
	 *
	 * The main behavior of the TWI bus & peripheral are running in interrupt context.
	 * If the TWI bus & peripheral are idle (all previous operations are completed,
	 * thus no interrupts will happen) then this will trigger the interrupt.
	 */
	void run() final override
	{
		if(currentOperation == nullptr)
		{
			trigger();
		}
	}

	/**
	 * \brief The interrupt service routine associated with the TWI bus.
	 *
	 * This is the main behavior of the TWI bus & peripheral and should run in interrupt context.
	 * It should be called in the associated interrupt handler.
	 */
	void isr() final override
	{
		peripheral.isr();

		switch(peripheral.status())
		{
			case Peripheral::State::IDLE:
			{
				if(currentOperation != nullptr)
				{
					if(currentOperation->status == Operation::Status::TBD)
					{
						currentOperation->status = Operation::Status::SUCCESS;
					}
					endPoint[currentEndPoint]->send(currentOperation);
					currentOperation = nullptr;
				}
				findNextOperation();
			}
			break;
			case Peripheral::State::NACK:
			{
				if(currentOperation != nullptr)
				{
					currentOperation->status = Operation::Status::FAIL;
					endPoint[currentEndPoint]->send(currentOperation);
					currentOperation = nullptr;
					flushEndpoint(currentEndPoint);
				}

				peripheral.clearNack();

				findNextOperation();
			} break;
			default: break;
		}
	}

private:
	Peripheral& peripheral;

	uint_fast8_t currentEndPoint = ENDPOINT_COUNT;
	Operation* currentOperation = nullptr;

	/**
	 * \brief Trigger the interrupt so the interrupt service routine will execute.
	 */
	void trigger() final override
	{
		peripheral.trigger();
	}

	void findNextOperation()
	{
		// Search for end point with a request.
		for(uint_fast8_t i = 0; i < ENDPOINT_COUNT; i++)
		{
			if(++currentEndPoint >= ENDPOINT_COUNT)
			{
				currentEndPoint = 0;
			}
			if(endPoint[currentEndPoint]->receive(currentOperation))
			{
				if(peripheral.transceive(currentOperation->address,
						currentOperation->direction,
						currentOperation->length,
						currentOperation->data))
				{
					break;
				}
				else
				{
					currentOperation->status = Operation::Status::FAIL;
					endPoint[currentEndPoint]->send(currentOperation);
					currentOperation = nullptr;
					flushEndpoint(currentEndPoint);
				}
			}
		}
	}

	void flushEndpoint(uint_fast8_t e)
	{
		Operation* operation;
		while(endPoint[e]->receive(operation))
		{
			operation->status = Operation::Status::FAIL;
			endPoint[e]->send(operation);
		}
	}

};

/**
 * \brief The interface TWI slaves should adhere to.
 */
class Slave: public Flow::Component
{
public:
	/**
	 * \brief Create a TWI slave.
	 *
	 * \param slaveSelect The slave select to be asserted during operations.
	 */
	explicit Slave(uint8_t address) :
			address(address)
	{
	}

	/**
	 * \brief The slave end point.
	 *
	 * flow::connect() this to one of the TWI::Bus end points.
	 */
	Flow::InOutPort<Operation*> endPoint
	{ this };

protected:
	uint8_t address;
};

} // namespace TWI
} // namespace Driver
} // namespace Flow

#endif /* FLOW_DRIVER_TWI_H_ */
