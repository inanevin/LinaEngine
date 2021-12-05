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

#include "Core/Engine.hpp"
#include "Profiling/Profiler.hpp"
#include "Core/Log.hpp"
#include "Utility/EngineUtility.hpp"
#include "EventSystem/EventSystem.hpp"
#include <iostream>

namespace Lina
{
#define RAW_DT_LIMIT 0.2f

	void Engine::Run(ApplicationMode& appMode)
	{
		LINA_TRACE("[Engine] -> Main Loop Started");

		m_eventSys->Trigger<Event::EPreMainLoop>({ appMode });

		m_gameStartTime = Utility::GetCPUTime();
		m_isRunning = true;
		double lastTime = m_gameTime;
		double timer = lastTime;
		int updates = 0;
		int frames = 0;
		double lastCountTime = 0;
		while (m_isRunning)
		{
			PROFILER_FUNC();

			// Calculate elapsed, raw delta and smoothed delta time.
			double currentGameTime = Utility::GetCPUTime() - m_gameStartTime;
			double gameTimeDiff = currentGameTime - m_gameTime;

			// Prevent spikes & break points from affecting game time.
			if (gameTimeDiff > 0.2f)
				m_gameStartTime += gameTimeDiff;
			else
				m_gameTime = currentGameTime;

			// Keep calculating deltas
			double now = m_gameTime;
			m_rawDeltaTime = now - lastTime;

			if (m_rawDeltaTime > RAW_DT_LIMIT)
				m_rawDeltaTime = RAW_DT_LIMIT;

			m_deltaTime = SmoothDeltaTime(m_rawDeltaTime);
			lastTime = now;
			updates++;

			m_eventSys->Trigger<Event::EPreTick>(Event::EPreTick{ (float)m_deltaTime, true });
			m_eventSys->Trigger<Event::ETick>(Event::ETick{ (float)m_deltaTime, true });
			m_eventSys->Trigger<Event::EPostTick>(Event::EPostTick{ (float)m_deltaTime, true });
			m_eventSys->Trigger<Event::ERender>();

			// Calculate updates & frames per second.
			frames++;
			if (now > lastCountTime + 1.0)
			{
				lastCountTime = now;
				m_ups = updates;
				m_fps = frames;
				updates = frames = 0;
			}

			//if (m_gameTime > 15.8f)
				//break;

		}

		LINA_TRACE("[Engine] -> Main Loop Stopped");
		m_eventSys->Trigger<Event::EPostMainLoop>();
	}

	double Engine::SmoothDeltaTime(double dt)
	{
		if (m_deltaFirstFill < DELTA_TIME_HISTORY)
		{
			m_deltaFirstFill++;
		}
		else if (!m_deltaFilled)
			m_deltaFilled = true;

		m_deltaTimeArray[m_deltaTimeArrOffset] = dt;
		m_deltaTimeArrOffset++;

		if (m_deltaTimeArrOffset == DELTA_TIME_HISTORY)
			m_deltaTimeArrOffset = 0;

		if (!m_deltaFilled)
			return dt;

		// Remove the biggest & smalles 2 deltas.
		RemoveOutliers(true);
		RemoveOutliers(true);
		RemoveOutliers(false);
		RemoveOutliers(false);

		double avg = 0.0;
		int index = 0;
		for (double d : m_deltaTimeArray)
		{
			if (d < 0.0)
			{
				m_deltaTimeArray[index] = m_deltaTimeArray[index] * -1.0;
				index++;
				continue;
			}

			avg += d;
			index++;
		}

		avg /= DELTA_TIME_HISTORY - 4;

		return avg;
	}


	void Engine::RemoveOutliers(bool biggest)
	{
		double outlier = biggest ? 0 : 10;
		int outlierIndex = -1;
		int indexCounter = 0;
		for (double d : m_deltaTimeArray)
		{
			if (d < 0)
			{
				indexCounter++;
				continue;
			}

			if (biggest)
			{
				if (d > outlier)
				{
					outlierIndex = indexCounter;
					outlier = d;
				}
			}
			else
			{
				if (d < outlier)
				{
					outlierIndex = indexCounter;
					outlier = d;
				}
			}

			indexCounter++;
		}

		if (outlierIndex != -1)
			m_deltaTimeArray[outlierIndex] = m_deltaTimeArray[outlierIndex] * -1.0;
	}
}