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