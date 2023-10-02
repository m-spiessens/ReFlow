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

void disconnect(Connect* connection)
{
	delete connection;
}

Component::Component()
{
	Reactor::add(*this);
}

void Component::waitFor(Peek& port)
{
	assert(port.owner == this);
	_waitFor = &port;
}

bool Component::tryRun()
{
	bool doRun = false;

	if(_waitFor != nullptr)
	{
		doRun = _waitFor->peek();
	}
	else
	{
		Peek* peekable = this->peekable;
		while(!doRun && peekable != nullptr)
		{
			doRun = peekable->peek();
			peekable = peekable->next;
		}
	}

	if(doRun)
	{
		_waitFor = nullptr;
		run();
	}

	return doRun;
}

Peek::Peek(Component* owner) :
	owner(owner)
{
	if(owner != nullptr)
	{
		Peek** tail = &owner->peekable;

		while(*tail != nullptr)
		{
			tail = &(*tail)->next;
		}

		*tail = this;
	}
}

InPort<void>::InPort(Component* owner) :
		Peek(owner)
{}

bool InPort<void>::receive()
{
	return this->isConnected() ? this->connection->receive() : false;
}

bool InPort<void>::peek() const
{
	return this->isConnected() ? this->connection->peek() : false;
}

void InPort<void>::connect(Connection<void>* connection)
{
	assert(!isConnected());
	this->connection = connection;
}

void InPort<void>::disconnect()
{
	this->connection = nullptr;
}

bool InPort<void>::full() const
{
	if(connection != nullptr)
	{
		return connection->full();
	}
	else
	{
		return false;
	}
}

bool InPort<void>::isConnected() const
{
	return this->connection != nullptr;
}

bool OutPort<void>::send()
{
	return this->isConnected() ? this->connection->send() : false;
}

bool OutPort<void>::full()
{
	return this->isConnected() ? this->connection->full() : false;
}

void OutPort<void>::connect(Connection<void>* connection)
{
	assert(!isConnected());
	this->connection = connection;
}

void OutPort<void>::disconnect()
{
	this->connection = nullptr;
}

bool OutPort<void>::isConnected() const
{
	return this->connection != nullptr;
}

Connection<void>::Connection(OutPort<void>& sender, InPort<void>& receiver, uint16_t size) :
		_size(size),
		sender(sender), receiver(receiver)
{
	sender.connect(this);
	receiver.connect(this);
}

Connection<void>::~Connection()
{
	sender.disconnect();
	receiver.disconnect();
}

} // namespace Flow
