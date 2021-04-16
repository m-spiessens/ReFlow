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

#include <assert.h>

#include "flow/platform.h"
#include "flow/reactor.h"

void Flow::Reactor::add(Component& component)
{
	if(instance().first == nullptr)
	{
	    instance().first = &component;
	    instance().last = &component;
	}
	else
	{
	    instance().last->next = &component;
	    instance().last = &component;
	}
}

void Flow::Reactor::start()
{
    assert(!instance().running);

    Component* current = instance().first;
    while(current != nullptr)
    {
        current->start();

        current = current->next;
    }

    instance().running = true;
}

void Flow::Reactor::stop()
{
    assert(instance().running);

    Component* current = instance().first;
    while(current != nullptr)
    {
        current->stop();

        current = current->next;
    }

    instance().running = false;
}

void Flow::Reactor::run()
{
    assert(instance().running);

	bool ranSomething = false;
	Component* current = instance().first;
	while(current != nullptr)
	{
		if(current->tryRun())
		{
			ranSomething = true;
		}

		current = current->next;
	}

	if(!ranSomething)
	{
		Platform::waitForEvent();
	}
}

void Flow::Reactor::reset()
{
	if(_instance != nullptr)
	{
		delete _instance;
		_instance = nullptr;
	}
}

Flow::Reactor::Reactor()
{
	Platform::configure();
}

Flow::Reactor& Flow::Reactor::instance()
{
	if(_instance == nullptr)
	{
		_instance = new Reactor;
	}

	return *_instance;
}

Flow::Reactor* Flow::Reactor::_instance = nullptr;
