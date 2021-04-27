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

#ifndef TM4C_DMA_H_
#define TM4C_DMA_H_

#include <stdint.h>

namespace TM4C {

class DMA
{
public:
	static constexpr uint32_t NONE = 32;

	/**
	 * \brief Access DMA peripheral.
	 */
	static DMA& instance();

	/**
	 * \brief Assign DMA channel to specific peripheral function.
	 *
	 * \param channelMapping The channel to peripheral mapping, e.g. UDMA_CH8_UART0RX.
	 */
    void assign(uint32_t channelMapping);

private:
    DMA();

    static DMA* _instance;

    uint8_t uDmaControlTable[1024] __attribute__ ((aligned(1024)));
    uint32_t inUse = 0;
};

} // namespace TM4C

#endif /* TM4C_DMA_H_ */
