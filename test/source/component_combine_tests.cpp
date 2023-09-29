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

TEST_GROUP(Component_Combine_TestBench)
{
	constexpr static unsigned int COMBINE_COUNT = 5;

	OutPort<char> outStimulus[COMBINE_COUNT];
	Connect* outStimulusConnection[COMBINE_COUNT];
	Combine<char, COMBINE_COUNT>* unitUnderTest;
	Connect* inResponseConnection;
	InPort<char> inResponse{ nullptr };

	void setup()
	{
		unitUnderTest = new Combine<char, COMBINE_COUNT>();

		for (unsigned int i = 0; i < COMBINE_COUNT; i++)
		{
			outStimulusConnection[i] = connect(outStimulus[i],
					unitUnderTest->in[i]);
		}

		inResponseConnection = connect(unitUnderTest->out, inResponse,
				COMBINE_COUNT * 2);
	}

	void teardown()
	{
		for (unsigned int i = 0; i < COMBINE_COUNT; i++)
		{
			delete outStimulusConnection[i];
		}

		delete inResponseConnection;

		delete unitUnderTest;

		Flow::Reactor::reset();
	}
};

TEST(Component_Combine_TestBench, DormantWithoutStimulus)
{
	CHECK(!inResponse.peek());

	unitUnderTest->run();

	CHECK(!inResponse.peek());
}

TEST(Component_Combine_TestBench, CompleteCombine)
{
	for (unsigned int i = 0; i < COMBINE_COUNT; i++)
	{
		CHECK(outStimulus[i].send(i));
	}

	CHECK(!inResponse.peek());

	unitUnderTest->run();

	char response = 0;
	for (unsigned int i = 0; i < COMBINE_COUNT; i++)
	{
		CHECK(inResponse.receive(response));

		char expected = i;
		CHECK_EQUAL(expected, response);
	}

	unitUnderTest->run();

	CHECK(!inResponse.receive(response));

	for (unsigned int i = 0; i < COMBINE_COUNT; i++)
	{
		CHECK(outStimulus[i].send(COMBINE_COUNT - i));
	}

	unitUnderTest->run();

	for (unsigned int i = 0; i < COMBINE_COUNT; i++)
	{
		CHECK(inResponse.receive(response));

		char expected = COMBINE_COUNT - i;
		CHECK_EQUAL(expected, response);
	}
}

TEST(Component_Combine_TestBench, PartialCombine)
{
	outStimulus[0].send(0);
	outStimulus[2].send(2);
	outStimulus[4].send(4);

	CHECK(!inResponse.peek());

	unitUnderTest->run();

	char response = 0;
	CHECK(inResponse.receive(response));

	char expected = 0;
	CHECK_EQUAL(expected, response);

	CHECK(inResponse.receive(response));

	expected = 2;
	CHECK_EQUAL(expected, response);

	CHECK(inResponse.receive(response));

	expected = 4;
	CHECK_EQUAL(expected, response);

	CHECK(!inResponse.receive(response));

	unitUnderTest->run();

	CHECK(!inResponse.receive(response));

	outStimulus[0].send(0);

	unitUnderTest->run();

	CHECK(inResponse.receive(response));

	expected = 0;
	CHECK_EQUAL(expected, response);

	CHECK(!inResponse.receive(response));

	outStimulus[4].send(4);
	outStimulus[3].send(3);
	outStimulus[1].send(1);

	CHECK(!inResponse.peek());

	unitUnderTest->run();

	CHECK(inResponse.receive(response));

	expected = 1;
	CHECK_EQUAL(expected, response);

	CHECK(inResponse.receive(response));

	expected = 3;
	CHECK_EQUAL(expected, response);

	CHECK(inResponse.receive(response));

	expected = 4;
	CHECK_EQUAL(expected, response);

	CHECK(!inResponse.receive(response));
}
