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

#include "flow/flow.h"
#include "flow/platform.h"
#include "flow/reactor.h"
#include "flow/utility.h"

namespace Flow {

void disconnect(Connection* connection)
{
	delete connection;
}

Component::Component()
{
	Reactor::add(*this);
}

bool Component::tryRun()
{
	bool doRun = false;

	Peek* peekable = this->peekable;
	while(!doRun && peekable != nullptr)
	{
		doRun = peekable->peek();
		peekable = peekable->next;
	}

	if(doRun)
	{
		run();
	}

	return doRun;
}

Peek::Peek(Component* owner)
{
	if(owner != nullptr)
	{
		if(owner->peekable == nullptr)
		{
			owner->peekable = this;
		}
		else
		{
			Peek* tail = owner->peekable;
			while(tail->next != nullptr)
			{
				tail = tail->next;
			}
			tail->next = this;
		}
	}
}

ConnectionOfType<void>::ConnectionOfType(OutPort<void>& sender, InPort<void>& receiver) :
			sender(sender), receiver(receiver)
{
	sender.connect(this);
	receiver.connect(this);
}

ConnectionOfType<void>::~ConnectionOfType<void>()
{
	sender.disconnect();
	receiver.disconnect();
}

template<>
Connection* connect(OutPort<void>& sender, InPort<void>& receiver,
		uint16_t size)
{
	(void)size;

	return new ConnectionOfType<void>(sender, receiver);
}

template<>
Connection* connect(OutPort<void>* sender, InPort<void>& receiver,
		uint16_t size)
{
	(void)size;
	assert(sender != nullptr);

	return new ConnectionOfType<void>(*sender, receiver);
}

template<>
Connection* connect(OutPort<void>& sender, InPort<void>* receiver,
		uint16_t size)
{
	(void)size;
	assert(receiver != nullptr);

	return new ConnectionOfType<void>(sender, *receiver);
}

template<>
Connection* connect(OutPort<void>* sender, InPort<void>* receiver,
		uint16_t size)
{
	(void)size;
	assert(sender != nullptr);
	assert(receiver != nullptr);

	return new ConnectionOfType<void>(*sender, *receiver);
}

} // namespace Flow
