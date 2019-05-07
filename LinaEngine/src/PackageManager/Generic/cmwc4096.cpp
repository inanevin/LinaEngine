/*
Author: Inan Evin
www.inanevin.com

Copyright 2018 Inan Evin

Licensed under the Apache License, Version 2.0 (the "License"); you may not use this file except in compliance with the License. You may obtain a copy of the License at

http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. See the License for the specific language governing permissions
and limitations under the License.

Class: cmwc4096
Timestamp: 4/8/2019 9:15:19 PM

*/

#include "LinaPch.hpp"
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