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

#ifndef FLOW_CONTAINER_H_
#define FLOW_CONTAINER_H_

#include <stdint.h>

#include <atomic>

namespace Flow
{

class Container
{
public:
	Container(size_t size) :
			_size(size)
	{
	}

	bool empty() const
	{
		return (enqueued == dequeued);
	}

	bool full() const
	{
		return (enqueued == static_cast<uint16_t>(dequeued + size()));
	}

	bool peek() const
	{
		return !empty();
	}

	size_t elements() const
	{
		int32_t delta = static_cast<int32_t>(enqueued) - static_cast<int32_t>(dequeued);

		return static_cast<uint16_t>((delta >= 0) ? delta : delta + UINT16_MAX + 1);
	}

	size_t size() const
	{
		return _size;
	}

protected:
	std::atomic<uint16_t> enqueued = 0;
	std::atomic<uint16_t> dequeued = 0;
	const size_t _size;
};

} // namespace Flow

#endif /* FLOW_CONTAINER_H_ */
