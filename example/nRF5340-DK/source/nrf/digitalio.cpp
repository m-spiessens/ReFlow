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

#include "nrf/digitalio.h"
#include "nrf/interrupt.h"

namespace nRF {

Pin::Pin(uint8_t number) :
        _number(number)
{}

bool Pin::operator==(Pin& other)
{
	bool same = true;
	same = same && (this->_number == other._number);
	return same;
}

uint8_t Pin::number() const
{
    return _number;
}

IRQn_Type Pin::vector() const
{
    return _vector[0];
}

const IRQn_Type Pin::_vector[] = {
    GPIOTE0_IRQn,
    GPIOTE1_IRQn
};

namespace Digital {

Input::Input(uint8_t number, Flow::Driver::Digital::Polarity polarity)
:   Flow::Driver::Digital::Input(polarity),
    pin(number)
{
	nrf_gpio_cfg_input(pin.number(), NRF_GPIO_PIN_NOPULL);
}

bool Input::get()
{
     return applyPolarity(nrf_gpio_pin_read(pin.number()));
}

Output::Output(uint8_t number, Flow::Driver::Digital::Polarity polarity) :
        Flow::Driver::Digital::Output(polarity),
        pin(number)
{
    nrf_gpio_cfg_output(pin.number());

    set(false);
}

void Output::toggle()
{
    // int32_t status = GPIOPinRead(pin.base(), (1 << pin.number()));
    // bool high = (status == (1 << pin.number()));
    // GPIOPinWrite(pin.base(), (1 << pin.number()), high ? 0x00 : 0xFF);
	nrf_gpio_pin_toggle(pin.number());
}

void Output::set(bool active)
{
    // GPIOPinWrite(pin.base(), (1 << pin.number()), applyPolarity(active) ? 0xFF : 0x00);
	nrf_gpio_pin_write(pin.number(), applyPolarity(active));
}

Interrupt::Interrupt(uint8_t number, Trigger trigger, Polarity polarity)
:   Flow::Driver::Digital::Interrupt(trigger, polarity),
    nRF::Digital::Input(number, polarity)
{
}

Interrupt::~Interrupt()
{
}

void Interrupt::start()
{
    // GPIOIntTypeSet(pin.base(), (1 << pin.number()), lutTrigger[(uint8_t)_trigger]);
    // GPIOIntEnable(pin.base(), (1 << pin.number()));

    ::Interrupt::VectorTable::enable(pin.vector());
}

void Interrupt::stop()
{
    ::Interrupt::VectorTable::disable(pin.vector());

    // GPIOIntDisable(pin.base(), (1 << pin.number()));
}

void Interrupt::run()
{
    out.send(get());
}

bool Interrupt::get()
{
    return Input::get();
}

void Interrupt::trigger()
{
    ::Interrupt::VectorTable::trigger(pin.vector());
}

void Interrupt::isr()
{
    // uint32_t status = GPIOIntStatus(pin.base(), true);
    // GPIOIntClear(pin.base(), (1u << pin.number()));

    // if(status == (1u << pin.number()))
    {
        out.send(get());
    }
}

const uint32_t Interrupt::lutTrigger[(uint8_t)Trigger::COUNT] =
{
    // [(uint8_t)Trigger::Change] = GPIO_BOTH_EDGES,
    // [(uint8_t)Trigger::FallingEdge ] = GPIO_FALLING_EDGE,
    // [(uint8_t)Trigger::RisingEdge ] = GPIO_RISING_EDGE
};

} // namespace Digital
} // namespace nRF
