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

#ifndef FLOW_DRIVER_SSIBUS_H_
#define FLOW_DRIVER_SSIBUS_H_

#include "flow/flow.h"

#include "driver/ssi.h"

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

/**
 * \brief An SSI operation.
 *
 * This implementation is target agnostic.
 */
class Operation
{
public:
	/**
	 * \brief Create a SSI operation.
	 */
	Operation(uint16_t size) : 
			_size(size)
	{
	}

	virtual ~Operation() = default;

	uint16_t size() const
	{
		return _size;
	}

	void transmitLength(uint16_t length)
	{
		assert(length <= size());
		_transmitLength = length;
	}

	uint16_t transmitLength() const
	{
		return _transmitLength;
	}

	void receiveLength(uint16_t length)
	{
		assert(length <= size());
		_receiveLength = length;
	}

	uint16_t receiveLength() const
	{
		return _receiveLength;
	}

	/**
	 * \brief Operation status.
	 */
	enum class Status
	{
		TBD, FAIL, SUCCESS
	} status = Status::TBD;

	uint8_t* transmit = nullptr;
	uint8_t* receive = nullptr;

	uint32_t tag[4];

private:
	const uint16_t _size;
	uint16_t _transmitLength = 0;
	uint16_t _receiveLength = 0;
};

/**
 * \brief SSI bus multiplexer/demultiplexer.
 *
 * This implementation is target agnostic.
 * It uses round robin scheduling if multiple slaves are connected.
 */
template<uint_fast8_t ENDPOINT_COUNT>
class Bus :
    	public Flow::Component, 
		public Complete
{
public:
	/**
	 * \brief Create a SSI bus.
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

		peripheral.attach(*this);
	}

	/**
	 * \brief The bus end points.
	 *
	 * flow::connect() one or more SSI::Slave to these.
	 */
	Flow::InOutPort<Operation*>* endPoint[ENDPOINT_COUNT];

	/**
	 * \brief Start the SSI bus.
	 *
	 * The SSI peripheral handled by this bus will also be started.
	 */
	void start() final override
	{
		peripheral.start();
	}

	/**
	 * \brief Stop the SSI bus.
	 *
	 * The SSI peripheral handled by this bus will also be halted.
	 */
	void stop() final override
	{
		peripheral.stop();
	}

	/**
	 * \brief Let the bus perform its duty.
	 *
	 * The main behavior of the SSI bus & peripheral are running in interrupt context.
	 * If the SSI bus & peripheral are idle (all previous operations are completed,
	 * thus no interrupts will happen) then this will trigger the interrupt.
	 */
	void run() final override
	{
		if(currentOperation == nullptr)
		{
			peripheral.trigger();
		}
	}

	/**
	 * \brief The operation complete handler associated with the SSI bus.
	 *
	 * This is the main behavior of the SSI bus & peripheral.
	 * It runs in interrupt context of the peripheral.
	 */

	void complete(Status status) final override
	{
		(void)status;
		
		switch(peripheral.state())
		{
			case Peripheral::State::Ready:
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

				// Search for end point with a request.
				for(uint_fast8_t i = 0; i < ENDPOINT_COUNT; i++)
				{
					if(++currentEndPoint >= ENDPOINT_COUNT)
					{
						currentEndPoint = 0;
					}

					if(endPoint[currentEndPoint]->receive(currentOperation))
					{
						if(peripheral.transceive(currentEndPoint, currentOperation->transmit,
						        currentOperation->transmitLength(),
						        currentOperation->receive,
						        currentOperation->receiveLength()))
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
			break;
			case Peripheral::State::Init:
			case Peripheral::State::Busy:
			break;
		}
	}

private:
	Peripheral& peripheral;

	uint_fast8_t currentEndPoint = ENDPOINT_COUNT;
	Operation* currentOperation = nullptr;

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
 * \brief The interface drivers for SSI devices should adhere to
 * (on the master side implementation).
 */
class Slave :
    	public Flow::Component
{
public:
	/**
	 * \brief The slave end point.
	 *
	 * flow::connect() this to one of the SSI::Bus end points.
	 */
	Flow::InOutPort<Operation*> endPoint{ this };
};

} // namespace Master
} // namespace SSI
} // namespace Driver
} // namespace Flow

#endif /* FLOW_DRIVER_SSIBUS_H_ */
