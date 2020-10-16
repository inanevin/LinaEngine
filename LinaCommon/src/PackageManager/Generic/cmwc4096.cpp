/* 
This file is a part of: Lina Engine
https://github.com/inanevin/LinaEngine

Author: Inan Evin
http://www.inanevin.com

Copyright (c) [2018-2020] [Inan Evin]

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/


#include "PackageManager/Generic/cmwc4096.hpp"  

// Make 32 bit random number (some systems use 16 bit RAND_MAX [Visual C 2012 uses 15 bits!])
uint32_t rand32(void)
{
	uint32_t result = rand();
	return (result << 16) ^ rand();
}

// Init the state with seed
void initCMWC(struct cmwc_state *state, unsigned int seed)
{
	int i;
	srand(seed);
	for (i = 0; i < CMWC_CYCLE; i++)
		state->Q[i] = rand32();
	do
		state->c = rand32();
	while (state->c >= CMWC_C_MAX);
	state->i = CMWC_CYCLE - 1;
}

// CMWC renderDevice
uint32_t randCMWC(struct cmwc_state *state)  //EDITED parameter *state was missing
{
	uint64_t const a = 18782; // as Marsaglia recommends
	uint32_t const m = 0xfffffffe; // as Marsaglia recommends
	uint64_t t;
	uint32_t x;

	state->i = (state->i + 1) & (CMWC_CYCLE - 1);
	t = a * state->Q[state->i] + state->c;
	/* Let c = t / 0xfffffff, x = t mod 0xffffffff */
	state->c = t >> 32;
	x = (uint32_t)t + state->c;
	if (x < state->c) {
		x++;
		state->c++;
	}
	return state->Q[state->i] = m - x;
}