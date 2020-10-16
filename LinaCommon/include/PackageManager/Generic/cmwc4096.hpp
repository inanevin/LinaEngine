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

/*
Class: cmwc4096


Timestamp: 4/8/2019 9:15:19 PM

*/

#pragma once

#ifndef cmwc4096_HPP
#define cmwc4096_HPP

#ifndef CMWC4096_H_INCLUDED
#define CMWC4096_H_INCLUDED

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

// CMWC working parts
#define CMWC_CYCLE 4096 // as Marsaglia recommends
#define CMWC_C_MAX 809430660 // as Marsaglia recommends
struct cmwc_state {
	uint32_t Q[CMWC_CYCLE];
	uint32_t c;	// must be limited with CMWC_C_MAX
	unsigned i;
};

void initCMWC(struct cmwc_state *state, unsigned int seed);
uint32_t randCMWC(struct cmwc_state *state);

#endif


#endif