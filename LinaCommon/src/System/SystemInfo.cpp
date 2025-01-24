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

#include "Common/System/SystemInfo.hpp"
#include "Common/Math/Math.hpp"
#include "Common/Data/Array.hpp"
#include "Common/Data/CommonData.hpp"
#include "Common/Platform/LinaGXIncl.hpp"

namespace Lina
{
#define IDEAL_DT 16667

	bool		   SystemInfo::s_appHasFocus					= true;
	int64		   SystemInfo::s_currentSystemTimeMicroseconds	= 0;
	int64		   SystemInfo::s_appTimeMicroseconds			= 0;
	int64		   SystemInfo::s_appStartMicroseconds			= 0;
	int64		   SystemInfo::s_appStartSystemTimeMicroseconds = 0;
	int64		   SystemInfo::s_smoothedDeltaMicroseconds		= 0;
	double		   SystemInfo::s_smoothedDeltaSeconds			= 0;
	int64		   SystemInfo::s_deltaTimeMicroseconds			= 0;
	int64		   SystemInfo::s_appStartCycles					= 0;
	uint64		   SystemInfo::s_frames							= 0;
	uint32		   SystemInfo::s_measuredFPS					= 0;
	size_t		   SystemInfo::s_mainThreadID					= 0;
	double		   SystemInfo::s_appTime						= 0.0;
	double		   SystemInfo::s_deltaTimeSeconds				= 0.0;
	double		   SystemInfo::s_timescale						= 1.0;
	uint32		   SystemInfo::s_rendererBehindFrames			= 0;
	LinaGX::Format SystemInfo::s_swapchainFormat				= LinaGX::Format::UNDEFINED;
	LinaGX::Format SystemInfo::s_hdrFormat						= LinaGX::Format::UNDEFINED;
	LinaGX::Format SystemInfo::s_ldrFormat						= LinaGX::Format::UNDEFINED;
	LinaGX::Format SystemInfo::s_depthFormat					= LinaGX::Format::UNDEFINED;
	uint32		   SystemInfo::s_allowedMSAA					= 1;

	double SystemInfo::CalculateRunningAverageDT(int64 deltaMicroseconds)
	{

		// Keep a history of the deltas for the last 11 m_frames
		// Throw away the outliers, two highest and two lowest values
		// Calculate the mean of remaining 7 values
		// Lerp from the time step for th elast frame to calculated mean

		static uint32			historyIndex = 0;
		static Array<int64, 11> dtHistory	 = {{IDEAL_DT, IDEAL_DT, IDEAL_DT, IDEAL_DT, IDEAL_DT, IDEAL_DT, IDEAL_DT, IDEAL_DT, IDEAL_DT, IDEAL_DT, IDEAL_DT}};
		dtHistory[historyIndex]				 = deltaMicroseconds;
		historyIndex						 = (historyIndex + 1) % 11;

		linatl::sort(dtHistory.begin(), dtHistory.end());

		int64 mean = 0;
		for (uint32 i = 2; i < 9; i++)
			mean += dtHistory[i];

		return static_cast<double>(mean) / 7.0;
	}

} // namespace Lina
