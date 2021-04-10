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

#include <assert.h>

#include "tm4c/dma.h"

#include "driverlib/sysctl.h"
#include "driverlib/udma.h"

namespace TM4C {

DMA& DMA::peripheral()
{
    static DMA _instance;
    return _instance;
}

void DMA::assign(uint32_t channelMapping)
{
	assert((inUse & (1 << (channelMapping & 31))) == 0);

	uDMAChannelAssign(channelMapping);

	inUse |= (1 << (channelMapping & 31));
}

DMA::DMA()
{
	SysCtlPeripheralDisable(SYSCTL_PERIPH_UDMA);

    while(!SysCtlPeripheralReady(SYSCTL_PERIPH_UDMA))
    {
        SysCtlPeripheralEnable(SYSCTL_PERIPH_UDMA);
    }

    uDMAEnable();
    uDMAControlBaseSet(uDmaControlTable);
}

} // namespace TM4C
