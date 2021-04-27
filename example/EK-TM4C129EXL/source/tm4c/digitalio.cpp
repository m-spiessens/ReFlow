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

#include "tm4c/digitalio.h"
#include "tm4c/interrupt.h"
#include "tm4c/clock.h"

#include "inc/hw_ints.h"
#include "inc/hw_memmap.h"
#include "driverlib/debug.h"
#include "driverlib/gpio.h"
#include "driverlib/interrupt.h"
#include "driverlib/sysctl.h"

namespace TM4C {

Pin::Pin(Port port, uint8_t number)
:	_port(port),
	_number(number)
{}

bool Pin::operator==(Pin& other)
{
	bool same = true;
	same = same && (this->_port == other._port);
	same = same && (this->_number == other._number);
	return same;
}

Pin::Port Pin::port() const
{
    return _port;
}

uint32_t Pin::base() const
{
    return portBase[(uint8_t)_port];
}

uint8_t Pin::number() const
{
    return _number;
}

uint32_t Pin::peripheral() const
{
    return portPeripheral[(uint8_t)_port];
}

uint8_t Pin::vector() const
{
    return portVector[(uint8_t)_port];
}

const uint32_t Pin::portBase[(uint8_t)Port::COUNT] = {
	GPIO_PORTA_BASE,
	GPIO_PORTB_BASE,
	GPIO_PORTC_BASE,
	GPIO_PORTD_BASE,
	GPIO_PORTE_BASE,
	GPIO_PORTF_BASE,
	GPIO_PORTG_BASE,
	GPIO_PORTH_BASE,
	GPIO_PORTJ_BASE,
	GPIO_PORTK_BASE,
	GPIO_PORTL_BASE,
	GPIO_PORTM_BASE,
	GPIO_PORTN_BASE,
	GPIO_PORTP_BASE,
	GPIO_PORTQ_BASE
};

const uint32_t Pin::portPeripheral[(uint8_t)Port::COUNT] = {
    SYSCTL_PERIPH_GPIOA,
    SYSCTL_PERIPH_GPIOB,
    SYSCTL_PERIPH_GPIOC,
    SYSCTL_PERIPH_GPIOD,
    SYSCTL_PERIPH_GPIOE,
    SYSCTL_PERIPH_GPIOF,
    SYSCTL_PERIPH_GPIOG,
    SYSCTL_PERIPH_GPIOH,
    SYSCTL_PERIPH_GPIOJ,
    SYSCTL_PERIPH_GPIOK,
    SYSCTL_PERIPH_GPIOL,
    SYSCTL_PERIPH_GPIOM,
    SYSCTL_PERIPH_GPION,
    SYSCTL_PERIPH_GPIOP,
    SYSCTL_PERIPH_GPIOQ
};

const uint8_t Pin::portVector[(uint8_t)Pin::Port::COUNT] = {
    INT_GPIOA,
    INT_GPIOB,
    INT_GPIOC,
    INT_GPIOD,
    INT_GPIOE,
    INT_GPIOF,
    INT_GPIOG,
    INT_GPIOH,
    INT_GPIOJ,
    INT_GPIOK,
    INT_GPIOL,
    INT_GPIOM,
    INT_GPION,
    INT_GPIOP0,
    INT_GPIOQ0
};

namespace Digital {

Input::Input(Pin::Port port, uint8_t number, Flow::Driver::Digital::Polarity polarity)
:   Flow::Driver::Digital::Input(polarity),
    pin(port, number)
{
    while(!SysCtlPeripheralReady(pin.peripheral()))
    {
        SysCtlPeripheralEnable(pin.peripheral());
    }
}

bool Input::get()
{
    int32_t status = GPIOPinRead(pin.base(), (1 << pin.number()));
    bool high = (status == (1 << pin.number()));
    return applyPolarity(high);
}

Output::Output(Pin::Port port, uint8_t number, Flow::Driver::Digital::Polarity polarity) :
        Flow::Driver::Digital::Output(polarity),
        pin(port, number)
{
    while(!SysCtlPeripheralReady(pin.peripheral()))
    {
        SysCtlPeripheralEnable(pin.peripheral());
    }

    set(false);
}

void Output::toggle()
{
    int32_t status = GPIOPinRead(pin.base(), (1 << pin.number()));
    bool high = (status == (1 << pin.number()));
    GPIOPinWrite(pin.base(), (1 << pin.number()), high ? 0x00 : 0xFF);
}

void Output::set(bool active)
{
    GPIOPinWrite(pin.base(), (1 << pin.number()), applyPolarity(active) ? 0xFF : 0x00);
}

Interrupt::Interrupt(Pin::Port port, uint8_t number, Trigger trigger, Polarity polarity)
:   Flow::Driver::Digital::Interrupt(trigger, polarity),
    TM4C::Digital::Input(port, number, polarity)
{
}

Interrupt::~Interrupt()
{
}

void Interrupt::start()
{
    GPIOIntTypeSet(pin.base(), (1 << pin.number()), lutTrigger[(uint8_t)_trigger]);
    GPIOIntEnable(pin.base(), (1 << pin.number()));

    ::Interrupt::VectorTable::enable(pin.vector());
}

void Interrupt::stop()
{
    ::Interrupt::VectorTable::disable(pin.vector());

    GPIOIntDisable(pin.base(), (1 << pin.number()));
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
    uint32_t status = GPIOIntStatus(pin.base(), true);
    GPIOIntClear(pin.base(), (1u << pin.number()));

    if(status == (1u << pin.number()))
    {
        out.send(get());
    }
}

const uint32_t Interrupt::lutTrigger[(uint8_t)Trigger::COUNT] =
{
    [(uint8_t)Trigger::Change] = GPIO_BOTH_EDGES,
    [(uint8_t)Trigger::FallingEdge ] = GPIO_FALLING_EDGE,
    [(uint8_t)Trigger::RisingEdge ] = GPIO_RISING_EDGE
};

} // namespace Digital
} // namespace TM4C
