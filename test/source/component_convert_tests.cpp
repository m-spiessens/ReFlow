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

#include "CppUTest/TestHarness.h"

#include "flow/components.h"
#include "flow/reactor.h"

#include "data.h"

using Flow::Connect;
using Flow::OutPort;
using Flow::InPort;
using Flow::connect;

TEST_GROUP(Component_Convert_TestBench)
{
	OutPort<float> outStimulus;
	Connect* outStimulusConnection;
	Convert<float, int32_t>* unitUnderTest;
	Connect* inResponseConnection;
	InPort<int32_t> inResponse{ nullptr };

	void setup()
	{
		unitUnderTest = new Convert<float, int32_t>();

		outStimulusConnection = connect(outStimulus, unitUnderTest->inFrom);
		inResponseConnection = connect(unitUnderTest->outTo, inResponse);
	}

	void teardown()
	{
		disconnect(outStimulusConnection);
		disconnect(inResponseConnection);

		delete unitUnderTest;

		Flow::Reactor::reset();
	}
};

TEST(Component_Convert_TestBench, Convert)
{
	CHECK(!inResponse.peek());

	unitUnderTest->run();

	CHECK(!inResponse.peek());

	CHECK(outStimulus.send(1.0f));

	unitUnderTest->run();

	int32_t result = 0;
	CHECK(inResponse.receive(result));
	CHECK_EQUAL(1, result);

	CHECK(outStimulus.send(-1.0f));

	unitUnderTest->run();

	result = 0;
	CHECK(inResponse.receive(result));
	CHECK_EQUAL(-1, result);
}
