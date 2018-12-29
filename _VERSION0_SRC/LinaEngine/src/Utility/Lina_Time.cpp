/*
Author: Inan Evin
www.inanevin.com

MIT License

Lina Engine, Copyright (c) 2018 Inan Evin

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"),
to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense,
and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:
The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.
THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

4.0.30319.42000
10/1/2018 3:34:14 AM

*/

#include "pch.h"
#include "Utility/Lina_Time.hpp"  

static const long SECOND = 1000000000;	// time in nanosecs
double Lina_Time::delta;

double Lina_Time::GetTime()
{
	// Get time traffic
	std::chrono::time_point<std::chrono::system_clock> now = std::chrono::system_clock::now();

	// Get time traffic with presicion of nanoseconds.
	auto now_ns = std::chrono::time_point_cast<std::chrono::nanoseconds>(now);

	// Now get the time as std::chrono::nanoseconds type.
	auto duration = now_ns.time_since_epoch();

	// cast the duration
	auto nanoseconds = std::chrono::duration_cast<std::chrono::nanoseconds>(duration);

	return (double)(nanoseconds.count() / static_cast<double>(SECOND));
}



