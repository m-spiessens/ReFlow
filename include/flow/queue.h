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

#ifndef FLOW_QUEUE_H_
#define FLOW_QUEUE_H_

#include <malloc.h>
#include <string.h>

#include "flow/container.h"

/**
 * \brief Flow is a pipes and filters implementation tailored for (but not exclusive to) microcontrollers.
 */
namespace Flow
{

class GenericQueue :
		public Container
{
private:
	uint8_t* data;
	const uint16_t elementSize;
	volatile uint16_t first = 0;
	volatile uint16_t last = 0;

protected:
	/**
	 * \brief Create a queue.
	 *
	 * The array of DataType will be allocated on the heap.
	 *
	 * \param size The size of the queue in number of DataType.
	 */
	explicit GenericQueue(uint16_t elementCount, uint16_t elementSize) :
			Container(elementCount),
			elementSize(elementSize)
	{
		data = (uint8_t*)malloc(elementCount * elementSize);
	}

	/**
	 * \brief Destructor.
	 *
	 * Deallocates the array of DataType from the heap.
	 */
	~GenericQueue()
	{
		free(data);
	}

	/**
	 * \brief Enqueue an element of DataType.
	 *
	 * Can be called concurrently with respect to dequeue().
	 * If the queue is full the given element is not added.
	 *
	 * \param element The element to be enqueued.
	 * \return The element was successfully enqueued.
	 */
	bool enqueue(const void* element)
	{
		bool success = false;

		if (!full())
		{
			memcpy(&data[last * elementSize], element, elementSize);

			last = (last == size() - 1) ? 0 : last + 1;

			enqueued++;

			success = true;
		}

		return success;
	}

	/**
	 * \brief Dequeue an element of DataType.
	 *
	 * Can be called concurrently with respect to enqueue().
	 *
	 * \param element [output] The dequeued element.
	 * 		The return value indicates whether the element is valid.
	 * \return An element was successfully dequeued.
	 * 		Thus the element output parameter has a valid value.
	 */
	bool dequeue(void* element)
	{
		bool success = false;

		if (!empty())
		{
			memcpy(element , &data[first * elementSize], elementSize);

			first = (first == size() - 1) ? 0 : first + 1;

			dequeued++;

			success = true;
		}

		return success;
	}

	/**
	 * \brief Peek in the queue.
	 *
	 * Does not modify the queue in any way.
	 *
	 * \param element [output] The next element to be dequeued.
	 * 		The return value indicates whether the element is valid.
	 * \return The queue is not empty.
	 * 		Thus the element output parameter has a valid value.
	 */
	bool peek(void* element) const
	{
		bool success = false;

		if (!empty())
		{
			memcpy(element , &data[first * elementSize], elementSize);

			success = true;
		}

		return success;
	}
};

template<typename Type>
class Queue : 
		public GenericQueue
{
public:
	explicit Queue(uint16_t size) :
			GenericQueue(size, sizeof(Type))
	{
	}

	/**
	 * \brief Enqueue an element of DataType.
	 *
	 * Can be called concurrently with respect to dequeue().
	 * If the queue is full the given element is not added.
	 *
	 * \param element The element to be enqueued.
	 * \return The element was successfully enqueued.
	 */
	bool enqueue(const Type& element)
	{
		return GenericQueue::enqueue(&element);
	}
	/**
	 * \brief Dequeue an element of DataType.
	 *
	 * Can be called concurrently with respect to enqueue().
	 *
	 * \param element [output] The dequeued element.
	 * 		The return value indicates whether the element is valid.
	 * \return An element was successfully dequeued.
	 * 		Thus the element output parameter has a valid value.
	 */
	bool dequeue(Type& element)
	{
		return GenericQueue::dequeue(&element);
	}
	
	/**
	 * \brief Peek in the queue.
	 *
	 * Does not modify the queue in any way.
	 *
	 * \param element [output] The next element to be dequeued.
	 * 		The return value indicates whether the element is valid.
	 * \return The queue is not empty.
	 * 		Thus the element output parameter has a valid value.
	 */
	bool peek(Type& element) const
	{
		return GenericQueue::peek(&element);
	}
};

} // namespace Flow

#endif /* FLOW_QUEUE_H_ */
