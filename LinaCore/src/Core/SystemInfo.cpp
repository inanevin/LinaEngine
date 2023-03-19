/*
This file is a part of: Lina Engine
https://github.com/inanevin/LinaEngine

Author: Inan Evin
http://www.inanevin.com

Copyright (c) [2018-] [Inan Evin]

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

#include "Core/SystemInfo.hpp"
#include "Math/Math.hpp"
#include "Data/Array.hpp"
#include "Data/CommonData.hpp"

namespace Lina
{
	ApplicationMode SystemInfo::s_appMode				= ApplicationMode::Editor;
	bool			SystemInfo::s_useFixedTimestep		= false;
	bool			SystemInfo::s_useFrameRateSmoothing = true;
	bool			SystemInfo::s_appHasFocus			= true;
	double			SystemInfo::s_fixedDeltaTime		= 1.0 / 100.0;
	double			SystemInfo::s_currentRealTime		= 0.0;
	double			SystemInfo::s_lastRealTime			= 0.0;
	double			SystemInfo::s_appTime				= 0.0;
	double			SystemInfo::s_deltaTime				= 0.0;
	double			SystemInfo::s_idleTime				= 0.0;
	double			SystemInfo::s_maxDeltaTime			= 1.0;
	float			SystemInfo::s_averageDT				= 1.0f / 100.0f;
	float			SystemInfo::s_physicsDeltaTime		= 1.0f / 100.0f;
	uint64			SystemInfo::s_frames				= 0;
	uint32			SystemInfo::s_measuredFPS			= 0;

	void SystemInfo::CalculateRunningAverageDT()
	{
		// Keep a history of the deltas for the last 11 m_frames
		// Throw away the outliers, two highest and two lowest values
		// Calculate the mean of remaining 7 values
		// Lerp from the time step for th elast frame to calculated mean

		static uint32			 historyIndex = 0;
		static Array<double, 11> dtHistory	  = {{IDEAL_DT, IDEAL_DT, IDEAL_DT, IDEAL_DT, IDEAL_DT, IDEAL_DT, IDEAL_DT, IDEAL_DT, IDEAL_DT, IDEAL_DT, IDEAL_DT}};
		dtHistory[historyIndex]				  = s_deltaTime;
		historyIndex						  = (historyIndex + 1) % 11;

		linatl::quick_sort(dtHistory.begin(), dtHistory.end());

		double mean = 0.0;
		for (uint32 i = 2; i < 9; i++)
			mean += dtHistory[i];

		mean /= 7.0;

		s_averageDT = Math::Lerp(s_averageDT, (float)mean, 0.1f);
	}

	float SystemInfo::CalculateMaxTickRate()
	{
		return 1.0f / s_averageDT;
	}
} // namespace Lina
