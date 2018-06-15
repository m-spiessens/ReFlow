/* The MIT License (MIT)
 *
 * Copyright (c) 2018 Cynara Krewe
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

#include "flow_test/data.h"

using Flow::Connection;
using Flow::OutPort;
using Flow::InPort;
using Flow::connect;

TEST_GROUP(Component_Timer_TestBench)
{
	Timer* unitUnderTest;
	Connection* inResponseConnection;
	InPort<Tick> inResponse{&dummy};

	void setup()
	{
		unitUnderTest = new Timer(100);

		inResponseConnection = connect(unitUnderTest->outTick, inResponse);
	}

	void teardown()
	{
		disconnect(inResponseConnection);

		delete unitUnderTest;
	}
};

TEST(Component_Timer_TestBench, DormantWithoutStimulus)
{
	CHECK(!inResponse.peek());

	unitUnderTest->run();

	CHECK(!inResponse.peek());
}

TEST(Component_Timer_TestBench, TickPeriod100)
{
	CHECK(!inResponse.peek());

	for (unsigned int i = 0; i < 100 - 1; i++)
	{
		unitUnderTest->run();
	}

	CHECK(!inResponse.peek());

	unitUnderTest->run();

	Tick tick = 1;
	CHECK(inResponse.receive(tick));
	CHECK(tick == TICK);

	for (unsigned int i = 0; i < 100 - 1; i++)
	{
		unitUnderTest->run();
	}

	CHECK(!inResponse.peek());

	unitUnderTest->run();

	CHECK(inResponse.receive(tick));
	CHECK(tick == TICK);

	for (unsigned int i = 0; i < 100 - 1; i++)
	{
		unitUnderTest->run();
	}

	CHECK(!inResponse.peek());

	unitUnderTest->run();

	CHECK(inResponse.receive(tick));
	CHECK(tick == TICK);
}
