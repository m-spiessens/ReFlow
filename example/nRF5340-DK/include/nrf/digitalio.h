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

#ifndef nRF_DIGITALIO_H_
#define nRF_DIGITALIO_H_

#include "driver/digitalio.h"

#include "nrf_gpio.h"

namespace nRF {

class Pin
{
public:
    Pin(uint8_t number);

    uint8_t number() const;

    IRQn_Type vector() const;

    bool operator==(Pin& other);

private:
    const uint8_t _number;

    static const IRQn_Type _vector[2];
};

/**
 * \brief Digital i/o related classes.
 *
 * Contains target agnostic implementations and interfaces
 * to which target specific peripherals should adhere.
 */
namespace Digital {

typedef Flow::Driver::Digital::Polarity Polarity;

class Input
:   public Flow::Driver::Digital::Input
{
public:
    Input(uint8_t number, Flow::Driver::Digital::Polarity polarity);

    bool get() override;

protected:
    const Pin pin;
};

class Output
:   public Flow::Driver::Digital::Output
{
public:
    Output(uint8_t number, Flow::Driver::Digital::Polarity polarity);

    void toggle() final override;

private:
    const Pin pin;

    void set(bool state) final override;
};

class Interrupt
:   public Flow::Driver::Digital::Interrupt,
    public nRF::Digital::Input
{
public:
    Interrupt(uint8_t number, Trigger trigger, Polarity polarity);
    ~Interrupt();

    void start() final override;
    void stop() final override;

    void run() final override;

    void isr() final override;

private:
    static const uint32_t lutTrigger[];

    bool get() final override;

    void trigger() final override;
};

} // namespace Digital
} // namespace nRF

#endif // nRF_DIGITALIO_H_
