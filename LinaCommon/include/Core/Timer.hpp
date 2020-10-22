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
Class: Timer



Timestamp: 10/22/2020 11:04:40 PM
*/

#pragma once

#ifndef Timer_HPP
#define Timer_HPP

#include <chrono>
#include <string>

namespace LinaEngine
{
	class Timer
	{

	public:

		Timer(const char* name, bool startImmediately) : m_name(name) , m_active(false)
		{
			if (startImmediately)
				Start();
		}

		~Timer();

		void Start()
		{
			m_active = true;
			m_startTimePoint = std::chrono::high_resolution_clock::now();
		}

		void Stop()
		{
			auto endPoint = std::chrono::high_resolution_clock::now();
			long long start = std::chrono::time_point_cast<std::chrono::milliseconds>(m_startTimePoint).time_since_epoch().count();
			long long end = std::chrono::time_point_cast<std::chrono::milliseconds>(endPoint).time_since_epoch().count();
			m_duration = end - start;
		}

	private:

		const char* m_name = "";;
		std::chrono::time_point<std::chrono::steady_clock> m_startTimePoint;
		bool m_active = false;
		long long m_duration;

	};
}

#endif
