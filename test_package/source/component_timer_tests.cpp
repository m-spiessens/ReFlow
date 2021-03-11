/* The MIT License (MIT)
 *
 * Copyright (c) 2021 Cynara Krewe
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

#include <stdint.h>

#include "CppUTest/TestHarness.h"

#include "flow/components.h"
#include "flow/reactor.h"

#include "data.h"

using Flow::Connection;
using Flow::OutPort;
using Flow::InPort;
using Flow::connect;

TEST_GROUP(Component_SoftwareTimer_TestBench)
{
	SoftwareTimer* unitUnderTest;
	Connection* inResponseConnection;
	InPort<void> inResponse{ nullptr };

	void setup()
	{
		unitUnderTest = new SoftwareTimer(100);

		inResponseConnection = connect(unitUnderTest->outTimeout, inResponse);
	}

	void teardown()
	{
		disconnect(inResponseConnection);

		delete unitUnderTest;

		Flow::Reactor::reset();
	}
};

TEST(Component_SoftwareTimer_TestBench, DormantWithoutStimulus)
{
	CHECK(!inResponse.peek());

	unitUnderTest->isr();

	CHECK(!inResponse.peek());
}

TEST(Component_SoftwareTimer_TestBench, TickPeriod100)
{
	CHECK(!inResponse.peek());

	for (unsigned int i = 0; i < 100 - 1; i++)
	{
		unitUnderTest->isr();
	}

	CHECK(!inResponse.peek());

	unitUnderTest->isr();

	CHECK(inResponse.receive());

	for (unsigned int i = 0; i < 100 - 1; i++)
	{
		unitUnderTest->isr();
	}

	CHECK(!inResponse.peek());

	unitUnderTest->isr();

	CHECK(inResponse.receive());

	for (unsigned int i = 0; i < 100 - 1; i++)
	{
		unitUnderTest->isr();
	}

	CHECK(!inResponse.peek());

	unitUnderTest->isr();

	CHECK(inResponse.receive());
}
