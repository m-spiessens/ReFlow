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

#include "flow/flow.h"

#include "driver/digitalio.h"
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
	virtual void complete(Status status) = 0;
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

	virtual ~Peripheral()
	{
	}

	/**
	 * \brief Configure and enable the peripheral.
	 */
	virtual void start() = 0;

	/**
	 * \brief Disable the peripheral.
	 */
	virtual void stop() = 0;

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
	        uint16_t receiveLength, bool multipart = false) = 0;

	virtual void attach(Complete& complete) = 0;

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
	 *
	 * \param slaveSelect The digital output to be asserted during this operation.
	 * If the FSS signal from the SSI peripheral is used slaveSelect can be omitted.
	 */
	Operation(Flow::Driver::Digital::Output* slaveSelect = nullptr) :
			slaveSelect(slaveSelect)
	{
	}

	virtual ~Operation()
	{
	}

	virtual uint32_t size() const = 0;

	/**
	 * \brief Operation status.
	 */
	enum class Status
	{
		TBD, FAIL, SUCCESS
	} status = Status::TBD;

	uint8_t* transmit = nullptr;
	uint16_t transmitLength = 0;
	uint8_t* receive = nullptr;
	uint16_t receiveLength = 0;
	bool multipart = false;

	uint32_t tag[4];

	Flow::Driver::Digital::Output* slaveSelect = nullptr;
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

	Flow::InPort<bool> inEnable{ this };

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
	}

	/**
	 * \brief Stop the SSI bus.
	 *
	 * The SSI peripheral handled by this bus will also be halted.
	 */
	void stop() final override
	{
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
		bool enable = false;
		if(inEnable.receive(enable))
		{
			if(!enabled && enable)
			{
				peripheral.start();
				enabled = true;
			}
			else if(enabled && !enable)
			{
				enabled = false;
				peripheral.stop();

				if(currentOperation != nullptr)
				{
					currentOperation->status = Operation::Status::FAIL;
					endPoint[currentEndPoint]->send(currentOperation);
					currentOperation = nullptr;
					flushEndpoint(currentEndPoint);
				}
			}
		}

		if(enabled && currentOperation == nullptr)
		{
			peripheral.trigger();
		}
		else if(!enabled)
		{
			for(uint_fast8_t e = 0; e < ENDPOINT_COUNT; e++)
			{
				flushEndpoint(e);
			}
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

					if(currentOperation->slaveSelect != nullptr)
					{
						currentOperation->slaveSelect->set(false);
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
						if(currentOperation->slaveSelect != nullptr)
						{
							currentOperation->slaveSelect->set(true);
						}

						if(peripheral.transceive(currentEndPoint, currentOperation->transmit,
						        currentOperation->transmitLength,
						        currentOperation->receive,
						        currentOperation->receiveLength,
								currentOperation->multipart))
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

	bool enabled = false;
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
class Slave:
    public Flow::Component
{
public:
	/**
	 * \brief Create a SSI slave.
	 *
	 * \param slaveSelect The slave select to be asserted during operations.
	 */
	explicit Slave(Flow::Driver::Digital::Output* slaveSelect = nullptr) :
			slaveSelect(slaveSelect)
	{
	}

	/**
	 * \brief The slave end point.
	 *
	 * flow::connect() this to one of the SSI::Bus end points.
	 */
	Flow::InOutPort<Operation*> endPoint
	{ this };

protected:
	Flow::Driver::Digital::Output* slaveSelect = nullptr;
};

} // namespace Master

namespace Slave
{

/**
 * \brief An SSI operation.
 *
 * This implementation is target agnostic.
 */
class Operation
{
public:
	Operation(const uint8_t* const transmit, uint8_t* const receive) :
			transmit(transmit),
			receive(receive)
	{}

	virtual ~Operation()
	{
	}

	/**
	 * \brief Operation status.
	 */
	enum class Status
	{
		TBD, FAIL, SUCCESS
	} status = Status::TBD;

	const uint8_t* const transmit;
	uint8_t transmitLength = 0;
	uint8_t* const receive;
	uint8_t receiveLength = 0;
};

class Peripheral:
    public Flow::Component,
    virtual public Flow::Driver::WithISR
{
public:
	Flow::InOutPort<Operation*> inOutOperation{this};

	virtual ~Peripheral()
	{
	}
};

} // namespace Slave
} // namespace SSI
} // namespace Driver
} // namespace Flow

#endif /* FLOW_DRIVER_SSI_H_ */
