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

#pragma once

#ifndef SystemInfo_HPP
#define SystemInfo_HPP

#include "Core/Common.hpp"

namespace Lina
{

	class Application;

	class SystemInfo
	{
	public:
		static inline ApplicationMode GetApplicationMode()
		{
			return s_appMode;
		}

		static inline double GetCurrentRealtime()
		{
			return s_currentRealTime;
		}

		static inline double GetLastRealtime()
		{
			return s_lastRealTime;
		}

		static inline double GetAppTime()
		{
			return s_appTime;
		}

		static inline double GetAppTimeF()
		{
			return static_cast<float>(s_appTime);
		}

		static inline double GetDeltaTime()
		{
			return s_deltaTime;
		}

		static inline float GetDeltaTimeF()
		{
			return static_cast<float>(s_deltaTime);
		}

		static inline bool UseFixedTimestep()
		{
			return s_useFixedTimestep;
		}

		static inline double GetFixedDeltaTime()
		{
			return s_fixedDeltaTime;
		}

		static inline float GetFixedFrameRate()
		{
			return 1.0f / s_fixedDeltaTime;
		}

		static inline bool UseFrameRateSmoothing()
		{
			return s_useFrameRateSmoothing;
		}

		static inline bool IsEditor()
		{
			return s_appMode == ApplicationMode::Editor;
		}

		static inline double GetIdleTime()
		{
			return s_idleTime;
		}

		static inline double GetMaxDeltaTime()
		{
			return s_maxDeltaTime;
		}

		static inline bool GetAppHasFocus()
		{
			return s_appHasFocus;
		}

		static inline float GetPhysicsDeltaTime()
		{
			return s_physicsDeltaTime;
		}

		static inline uint64 GetFrames()
		{
			return s_frames;
		}

		static inline uint32 GetMeasuredFPS()
		{
			return s_measuredFPS;
		}

	private:
		friend class Application;

		static void	 CalculateRunningAverageDT();
		static float CalculateMaxTickRate();

		static inline void SetCurrentRealTime(double seconds)
		{
			s_currentRealTime = seconds;
		}

		static inline void SetLastRealTime(double seconds)
		{
			s_lastRealTime = seconds;
		}

		static inline void SetAppTime(double seconds)
		{
			s_appTime = seconds;
		}

		static inline void SetDeltaTime(double seconds)
		{
			s_deltaTime = seconds;
		}

		static inline void SetApplicationMode(ApplicationMode mode)
		{
			s_appMode = mode;
		}

		static inline void SetUseFixedTimestep(bool use)
		{
			s_useFixedTimestep = use;
		}

		static inline void SetFixedDeltaTime(double dt)
		{
			s_fixedDeltaTime = dt;
		}

		static inline void SetFramerateSmoothingEnabled(bool enabled)
		{
			s_useFrameRateSmoothing = enabled;
		}

		static inline void SetIdleTime(double time)
		{
			s_idleTime = time;
		}

		static inline void SetMaxDeltaTime(double seconds)
		{
			s_maxDeltaTime = seconds;
		}

		static inline void SetAppHasFocus(bool hasFocus)
		{
			s_appHasFocus = hasFocus;
		}

		static inline void SetPhysicsDeltaTime(float dt)
		{
			s_physicsDeltaTime = dt;
		}

		static inline void SetFrames(uint64 frames)
		{
			s_frames = frames;
		}

		static inline void SetMeasuredFPS(uint32 fps)
		{
			s_measuredFPS = fps;
		}

	private:
		static ApplicationMode s_appMode;
		static bool			   s_useFixedTimestep;
		static bool			   s_useFrameRateSmoothing;
		static double		   s_fixedDeltaTime;
		static double		   s_currentRealTime;
		static double		   s_lastRealTime;
		static double		   s_appTime;
		static double		   s_deltaTime;
		static double		   s_idleTime;
		static double		   s_maxDeltaTime;
		static float		   s_averageDT;
		static float		   s_physicsDeltaTime;
		static bool			   s_appHasFocus;
		static uint64		   s_frames;
		static uint32		   s_measuredFPS;
	};
} // namespace Lina

#endif
