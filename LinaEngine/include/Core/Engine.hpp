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
Class: Engine

Responsible for main engine loop.

Timestamp: 12/25/2020 11:42:02 PM
*/

#pragma once

#ifndef Engine_HPP
#define Engine_HPP

// Headers here.
#include <array>
#include <cstdint>
#include "Core/Common.hpp"
#define DELTA_TIME_HISTORY 11

namespace Lina
{
	namespace Event
	{
		class EventSystem;
	}

	class Engine
	{
	private:

		friend class Application;

		Engine() {};
		~Engine() {};

		inline void SetReferences(Event::EventSystem* eventSys)
		{
			m_eventSys = eventSys;
		}

	public:
		
		inline float GetGameTime() { return (float)m_gameTime; }
		inline float GetRawDeltaTime() { return (float)m_rawDeltaTime; }
		inline float GetDeltaTime() { return (float)m_deltaTime; }
		inline float GetTimescale() { return m_timescale; }
		inline void SetTimescale(float ts) { m_timescale = ts; }

	private:

		void Run(ApplicationMode& appMode);
		double SmoothDeltaTime(double dt);
		void RemoveOutliers(bool biggest);

	private:

		Event::EventSystem* m_eventSys;
		bool m_isRunning = false;
		double m_gameStartTime = 0;
		double m_gameTime = 0;
		double m_rawDeltaTime = 0;
		double m_deltaTime = 0;
		float m_timescale = 1.0f;
		int m_ups = 0; // updates per second
		int m_fps = 0; // frames per second

		std::array<double, DELTA_TIME_HISTORY> m_deltaTimeArray = { 0 };
		uint8_t m_deltaTimeArrIndex = 0;
		uint8_t m_deltaTimeArrOffset = 0;
		int m_deltaFirstFill = 0;
		bool m_deltaFilled = false;
	
	};
}

#endif
