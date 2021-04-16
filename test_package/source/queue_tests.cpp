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
#include <thread>

#include "CppUTest/TestHarness.h"

#include "flow/queue.h"

#include "data.h"

using Flow::Queue;

const static unsigned int UNITS = 3;
const static unsigned int QUEUE_SIZE[UNITS] =
{ 1, 10, 1000 };

TEST_GROUP(Queue_TestBench)
{
	Queue<Data>* unitUnderTest[UNITS];

	void setup()
	{
		for (unsigned int i = 0; i < UNITS; i++)
		{
			unitUnderTest[i] = new Queue<Data>(QUEUE_SIZE[i]);
		}
	}

	void teardown()
	{
		for (unsigned int i = 0; i < UNITS; i++)
		{
			delete unitUnderTest[i];
		}
	}
};

TEST(Queue_TestBench, IsEmptyAfterCreation)
{
	for(unsigned int i = 0; i < UNITS; i++)
	{
		CHECK(unitUnderTest[i]->empty());
		Data response;
		CHECK(!unitUnderTest[i]->dequeue(response));
		CHECK(!unitUnderTest[i]->peek(response));
	}
}

TEST(Queue_TestBench, EnqueueDequeueItem)
{
	for(unsigned int i = 0; i < UNITS; i++)
	{
		CHECK(unitUnderTest[i]->empty());
		Data stimulus = Data(123, true);
		CHECK(unitUnderTest[i]->enqueue(stimulus));
		CHECK(!unitUnderTest[i]->empty());
		Data response = Data();
		CHECK(unitUnderTest[i]->elements() == 1);
		CHECK(unitUnderTest[i]->peek(response));
		CHECK_EQUAL(stimulus, response);
		response = Data();
		CHECK(unitUnderTest[i]->dequeue(response));
		CHECK_EQUAL(stimulus, response);
		CHECK(unitUnderTest[i]->empty());
		CHECK(!unitUnderTest[i]->dequeue(response));
	}
}

TEST(Queue_TestBench, FullQueue)
{
	for(unsigned int i = 0; i < UNITS; i++)
	{
		// Queue should be empty.
		CHECK(unitUnderTest[i]->empty());

		for(unsigned int c = 0; c < (QUEUE_SIZE[i] - 1); c++)
		{
			Data stimulus = Data(c, true);
			// Queue should accept another item.
			CHECK(unitUnderTest[i]->enqueue(stimulus));

			// Queue should not be empty.
			CHECK(!unitUnderTest[i]->empty());

			// Queue shouldn't be full.
			CHECK(!unitUnderTest[i]->full());
		}

		CHECK_EQUAL(QUEUE_SIZE[i] - 1, unitUnderTest[i]->elements());

		Data lastStimulus = Data(QUEUE_SIZE[i], false);
		// Queue should accept another item.
		CHECK(unitUnderTest[i]->enqueue(lastStimulus));

		// Queue should not be empty.
		CHECK(!unitUnderTest[i]->empty());

		// Queue should be full.
		CHECK(unitUnderTest[i]->full());

		CHECK_EQUAL(QUEUE_SIZE[i], unitUnderTest[i]->elements());

		// Queue shouldn't accept any more items.
		CHECK(!unitUnderTest[i]->enqueue(lastStimulus));

		CHECK(unitUnderTest[i]->elements() == QUEUE_SIZE[i]);

		Data response;

		for(unsigned int c = 0; c < (QUEUE_SIZE[i] - 1); c++)
		{
			// Should get another item from the Queue.
			CHECK(unitUnderTest[i]->dequeue(response));

			// Item should be the expected.
			Data expectedResponse = Data(c, true);
			CHECK_EQUAL(expectedResponse, response);

			// Queue should not be empty.
			CHECK(!unitUnderTest[i]->empty());

			// Queue shouldn't be full.
			CHECK(!unitUnderTest[i]->full());
		}

		// Should get another item from the Queue.
		CHECK(unitUnderTest[i]->dequeue(response));

		// Item should be the expected.
		CHECK_EQUAL(lastStimulus, response);

		// Queue shouldn't be full.
		CHECK(!unitUnderTest[i]->full());

		// Queue should be empty.
		CHECK(unitUnderTest[i]->empty());

		// Shouldn't get another item from the Queue.
		CHECK(!unitUnderTest[i]->dequeue(response));
	}
}

static void producer(Queue<Data>* queue, const unsigned long long count)
{
	unsigned long long c = 0;
	while (c < count)
	{
		if (queue->enqueue(Data(c, ((c % 2) == 0))))
		{
			c++;
		}
	}
}

static void consumer(Queue<Data>* queue, const unsigned long long count,
		bool* success)
{
	unsigned long long c = 0;
	while (c < count)
	{
		Data response;
		if (queue->dequeue(response))
		{
			Data expectedResponse = Data(c, ((c % 2) == 0));
			*success = *success && (response == expectedResponse);
			c++;
		}
	}
}

TEST(Queue_TestBench, Threadsafe)
{
	for (unsigned int i = 0; i < UNITS; i++)
	{
		CHECK(unitUnderTest[i]->empty());

		const unsigned long long numberOfItems = 1000;
		bool success = true;

		std::thread producerThread(producer, unitUnderTest[i], numberOfItems);
		std::thread consumerThread(consumer, unitUnderTest[i], numberOfItems,
				&success);

		producerThread.join();
		consumerThread.join();

		CHECK(success);

		CHECK(unitUnderTest[i]->empty());
	}
}

TEST(Queue_TestBench, ElementsOverflow)
{
	for(int32_t i = 0; i <= UINT16_MAX; i++)
	{
		for(unsigned int u = 0; u < UNITS; u++)
		{
			CHECK(unitUnderTest[u]->empty());
			Data stimulus = Data(123, true);
			CHECK(unitUnderTest[u]->enqueue(stimulus));
			CHECK(!unitUnderTest[u]->empty());
			Data response = Data();
			CHECK(unitUnderTest[u]->elements() == 1);
			CHECK(unitUnderTest[u]->peek(response));
			CHECK_EQUAL(stimulus, response);
			response = Data();
			CHECK(unitUnderTest[u]->dequeue(response));
			CHECK_EQUAL(stimulus, response);
			CHECK(unitUnderTest[u]->empty());
			CHECK(!unitUnderTest[u]->dequeue(response));
		}
	}

	for(unsigned int u = 0; u < UNITS; u++)
	{
		CHECK(unitUnderTest[u]->empty());
	}

	for(unsigned int u = 0; u < UNITS; u++)
	{
		Data stimulus = Data(123, true);
		for(unsigned int j = 0; j < QUEUE_SIZE[u]; j++)
		{
			CHECK(unitUnderTest[u]->enqueue(stimulus));
		}
		CHECK_EQUAL(QUEUE_SIZE[u], unitUnderTest[u]->elements());
		CHECK(unitUnderTest[u]->full());
	}
}
