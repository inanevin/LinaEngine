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
	class Engine;

	class SystemInfo
	{
	public:
		static inline ApplicationMode GetApplicationMode()
		{
			return s_appMode;
		}

		/// <summary>
		/// Total elapsed seconds since application start.
		/// </summary>
		/// <returns></returns>
		static inline double GetAppTime()
		{
			return s_appTime;
		}

		/// <summary>
		/// Total elapsed seconds since application start.
		/// </summary>
		/// <returns></returns>
		static inline float GetAppTimeF()
		{
			return static_cast<float>(s_appTime);
		}

		/// <summary>
		/// Time it took to process last frame in seconds. It is affected by frame-rate-smoothing.
		/// </summary>
		/// <returns></returns>
		static inline double GetRealDeltaTime()
		{
			return s_realDeltaTime;
		}

		/// <summary>
		/// Time it took to process last frame in seconds. It is affected by frame-rate-smoothing.
		/// </summary>
		/// <returns></returns>
		static inline float GetRealDeltaTimeF()
		{
			return static_cast<float>(s_realDeltaTime);
		}

		/// <summary>
		/// DeltaTime but in precision microseconds, not affected by frame-rate-smoothing.
		/// </summary>
		/// <returns></returns>
		static inline int64 GetRealDeltaTimeMicroseconds()
		{
			return s_realDeltaTimeMicroseconds;
		}

		/// <summary>
		/// Maximum delta time allowed, 0 for uncapped frame-rates.
		/// </summary>
		/// <returns></returns>
		static inline int64 GetFrameCapMicroseconds()
		{
			return s_frameCapMicroseconds;
		}

		/// <summary>
		/// Fixed-timestepping rate, physics will be updated at this rate. Use EVG_Simulate if you want to update your whole-game as fixed-time-stepped.
		/// </summary>
		/// <returns></returns>
		static inline int64 GetFixedTimestepMicroseonds()
		{
			return s_fixedTimestepMicroseconds;
		}

		/// <summary>
		/// Fixed-timestepping rate in seconds, physics will be updated at this rate. Use EVG_Simulate if you want to update your whole-game as fixed-time-stepped.
		/// </summary>
		/// <returns></returns>
		static inline double GetFixedTimestep()
		{
			return static_cast<double>(s_fixedTimestepMicroseconds) * 0.000001;
		}

		static inline uint32 GetMeasuredFPS()
		{
			return s_measuredFPS;
		}

		static inline bool GetUseFrameRateSmoothing()
		{
			return s_useFrameRateSmoothing;
		}

		static inline bool IsEditor()
		{
			return s_appMode == ApplicationMode::Editor;
		}

		static inline bool GetAppHasFocus()
		{
			return s_appHasFocus;
		}

		static inline uint64 GetFrames()
		{
			return s_frames;
		}

		static inline double GetTimescale()
		{
			return s_timescale;
		}

		static inline void SetTimescale(double timescale)
		{
			s_timescale = timescale;
		}

	private:
		friend class Application;
		friend class Engine;

		static double CalculateRunningAverageDT(double dt);

		static inline void SetAppTime(double seconds)
		{
			s_appTime = seconds;
		}

		static inline void SetDeltaTime(double seconds)
		{
			s_realDeltaTime = seconds;
		}

		static inline void SetApplicationMode(ApplicationMode mode)
		{
			s_appMode = mode;
		}

		static inline void SetFixedTimestep(int64 microseconds)
		{
			s_fixedTimestepMicroseconds = microseconds;
		}

		static inline void SetFrameCap(int64 microseconds)
		{
			s_frameCapMicroseconds = microseconds;
		}

		static inline void SetUseFrameRateSmoothing(bool use)
		{
			s_useFrameRateSmoothing = use;
		}

		static inline void SetAppHasFocus(bool hasFocus)
		{
			s_appHasFocus = hasFocus;
		}

		static inline void SetFrames(uint64 frames)
		{
			s_frames = frames;
		}

		static inline void SetMeasuredFPS(uint32 fps)
		{
			s_measuredFPS = fps;
		}

		static inline void SetRealDeltaTimeMicroseconds(int64 microseconds)
		{
			s_realDeltaTimeMicroseconds = microseconds;
		}

	private:
		static ApplicationMode s_appMode;
		static bool			   s_useFrameRateSmoothing;
		static double		   s_appTime;
		static double		   s_realDeltaTime;
		static double		   s_timescale;
		static bool			   s_appHasFocus;
		static int64		   s_fixedTimestepMicroseconds;
		static int64		   s_frameCapMicroseconds;
		static int64		   s_realDeltaTimeMicroseconds;
		static uint64		   s_frames;
		static uint32		   s_measuredFPS;
	};
} // namespace Lina

#endif
