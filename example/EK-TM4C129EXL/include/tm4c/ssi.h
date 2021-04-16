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

#ifndef TM4C_SSI_H_
#define TM4C_SSI_H_

#include "driver/digitalio.h"
#include "driver/ssi.h"

namespace TM4C {
namespace SSI {

class Base:
		virtual public Flow::Driver::WithISR
{
public:

	Base(uint8_t number);
	virtual ~Base() = default;

	void trigger() override;

	uint8_t vector() const;

protected:
	uint8_t number() const;
    uint32_t peripheral() const;
    uint32_t base() const;
    uint32_t dmaTransmitChannel() const;
    uint32_t dmaReceiveChannel() const;
    uint32_t dmaTransmitChannelAssign() const;
    uint32_t dmaReceiveChannelAssign() const;

private:
    static constexpr uint8_t COUNT = 4;
	const uint8_t _number;

    static const uint8_t _vector[COUNT];
    static const uint32_t _peripheral[COUNT];
    static const uint32_t _base[COUNT];
    static const uint32_t _dmaTransmitChannel[COUNT];
    static const uint32_t _dmaReceiveChannel[COUNT];
    static const uint32_t _dmaTransmitChannelAssign[COUNT];
    static const uint32_t _dmaReceiveChannelAssign[COUNT];
};

class Master:
        public Flow::Driver::SSI::Master::Peripheral,
        public Base
{
public:
	Master(uint8_t ssiNumber, Flow::Driver::Digital::Output* slaveSelect[], uint8_t slaves);
	~Master();

	void start() final override;
	void stop() final override;

	bool transceive(uint8_t slave, uint8_t* const transmit,
	        uint16_t transmitLength, uint8_t* const receive,
	        uint16_t receiveLength) final override;
    
    virtual void attach(Flow::Driver::SSI::Master::Complete& complete) final override;

    void isr() final override;

private:
    uint8_t slave = 0;
    const uint8_t slaves;
    Flow::Driver::Digital::Output** slaveSelect;

    Flow::Driver::SSI::Master::Complete* complete = nullptr;
    
    uint8_t dummy = 0;
};

} // namespace SSI
} // namespace TM4C

#endif /* TM4C_SSI_H_ */
