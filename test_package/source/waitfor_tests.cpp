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

#include <stdint.h>
#include <vector>

#include "CppUTest/TestHarness.h"
#include "CppUTestExt/MockSupport.h"

#include "flow/components.h"
#include "flow/reactor.h"
#include "flow/utility.h"

TEST_GROUP(WaitFor_TestBench)
{
	class Waiter :
			public Flow::Component
	{
	public:
		Flow::InPort<void> inWait{ this };
		Flow::InPort<int> in{ this };
		Flow::OutPort<int> out;
		Flow::OutPort<void> outRan;

		void start() final override
		{
			waitFor(inWait);
		}

		void run() final override
		{
			int i;
			while(in.receive(i))
			{
				out.send(i);
			}

			if(inWait.receive())
			{
				waitFor(inWait);
			}

			outRan.send();
		}
	}* waiter;

	std::vector<Flow::Connect*> connections;

	Flow::OutPort<void> stimulusWait;
	Flow::OutPort<int> stimulus;
	Flow::InPort<int> response{ nullptr };
	Flow::InPort<void> responseRan{ nullptr };

	void setup()
	{
		Flow::Reactor::reset();

		waiter = new Waiter;

		connections =
		{
			Flow::connect(stimulusWait, waiter->inWait),
			Flow::connect(stimulus, waiter->in, 3),
			Flow::connect(waiter->out, response, 3),
			Flow::connect(waiter->outRan, responseRan)
		};
	}

	void teardown()
	{
		mock().clear();

		for(auto connection : connections)
		{
			Flow::disconnect(connection);
		}
		connections.clear();

		delete waiter;

		Flow::Reactor::reset();
	}
};

TEST(WaitFor_TestBench, WaitFor)
{
	Flow::Reactor::start();

	stimulus.send(1);
	stimulus.send(2);
	stimulus.send(3);

	mock().expectOneCall("Platform::waitForEvent()");
	Flow::Reactor::run();

	CHECK_FALSE(responseRan.receive());

	stimulusWait.send();

	mock().expectOneCall("Platform::waitForEvent()");
	Flow::Reactor::run();

	CHECK_TRUE(responseRan.receive());
	int i;
	CHECK_TRUE(response.receive(i));
	CHECK_EQUAL(1, i);
	CHECK_TRUE(response.receive(i));
	CHECK_EQUAL(2, i);
	CHECK_TRUE(response.receive(i));
	CHECK_EQUAL(3, i);
	CHECK_FALSE(response.receive(i));

	mock().expectOneCall("Platform::waitForEvent()");
	Flow::Reactor::run();

	CHECK_FALSE(responseRan.receive());

	stimulus.send(4);
	stimulus.send(5);

	mock().expectOneCall("Platform::waitForEvent()");
	Flow::Reactor::run();

	CHECK_FALSE(responseRan.receive());

	stimulusWait.send();

	mock().expectOneCall("Platform::waitForEvent()");
	Flow::Reactor::run();

	CHECK_TRUE(responseRan.receive());
	CHECK_TRUE(response.receive(i));
	CHECK_EQUAL(4, i);
	CHECK_TRUE(response.receive(i));
	CHECK_EQUAL(5, i);
	CHECK_FALSE(response.receive(i));

	Flow::Reactor::stop();
}
