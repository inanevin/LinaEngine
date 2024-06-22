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

#include "Common/Common.hpp"
#include <thread>

namespace Lina
{
	class Application;
	class Engine;

	class SystemInfo
	{
	public:
		static constexpr float IDEAL_DT_F = 0.016667f;

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
		static inline double GetDeltaTime()
		{
			return s_deltaTime;
		}

		/// <summary>
		/// Time it took to process last frame in seconds. It is affected by frame-rate-smoothing.
		/// </summary>
		/// <returns></returns>
		static inline float GetDeltaTimeF()
		{
			return static_cast<float>(s_deltaTime);
		}

		/// <summary>
		/// Time it took to process last frame in seconds. Unaffected by smoothing.
		/// </summary>
		/// <returns></returns>
		static inline double GetRealDeltaTime()
		{
			return s_realDeltaTime;
		}

		/// <summary>
		/// Time it took to process last frame in microseconds. Unaffected by smoothing.
		/// </summary>
		/// <returns></returns>
		static inline int64 GetRealDeltaTimeMicroSeconds()
		{
			return s_realDeltaTimeMicroseconds;
		}

		/// <summary>
		/// DeltaTime but in precision microseconds.
		/// </summary>
		/// <returns></returns>
		static inline int64 GetDeltaTimeMicroSeconds()
		{
			return s_deltaTimeMicroseconds;
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
		/// Interpolation (t) factor for rendering.
		/// </summary>
		/// <returns></returns>
		static inline double GetInterpolationAlpha()
		{
			return s_interpolationAlpha;
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

		static inline int64 GetAppStartCycles()
		{
			return s_appStartCycles;
		}

		static inline size_t GetCurrentThreadID()
		{
			return std::hash<std::thread::id>{}(std::this_thread::get_id());
		}

		static inline bool IsMainThread()
		{
			return GetCurrentThreadID() == s_mainThreadID;
		}

		static inline int64 GetThrottleTime()
		{
			return s_throttleTime;
		}

	private:
		friend class Application;
		friend class Engine;

		static double CalculateRunningAverageDT(int64 deltaMicroseconds);

		static inline void SetDeltaTimeMicroSeconds(int64 microseconds)
		{
			s_deltaTimeMicroseconds = microseconds;
			s_deltaTime				= static_cast<double>(microseconds) * 0.000001;
		}

		static inline void SetAppTime(double seconds)
		{
			s_appTime = seconds;
		}

		static inline void SetFixedTimestep(int64 microseconds)
		{
			s_fixedTimestepMicroseconds = microseconds;
		}

		static inline void SetFrameCap(int64 microseconds)
		{
			s_frameCapMicroseconds = microseconds;
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

		static inline void SetRealDeltaTimeMicroSeconds(int64 microseconds)
		{
			s_realDeltaTimeMicroseconds = microseconds;
			s_realDeltaTime				= static_cast<double>(microseconds) * 0.000001;
		}

		static inline void SetAppStartCycles(int64 cycles)
		{
			s_appStartCycles = cycles;
		}

		static inline void SetInterpolationAlpha(double alpha)
		{
			s_interpolationAlpha = alpha;
		}

		static inline void SetMainThreadID(size_t id)
		{
			s_mainThreadID = id;
		}

		static inline void SetThrottleTime(int64 time)
		{
			s_throttleTime = time;
		}

	private:
		static double s_appTime;
		static double s_deltaTime;
		static double s_realDeltaTime;
		static double s_timescale;
		static double s_interpolationAlpha;
		static bool	  s_appHasFocus;
		static int64  s_fixedTimestepMicroseconds;
		static int64  s_frameCapMicroseconds;
		static int64  s_deltaTimeMicroseconds;
		static int64  s_realDeltaTimeMicroseconds;
		static int64  s_appStartCycles;
		static int64  s_throttleTime;
		static uint64 s_frames;
		static uint32 s_measuredFPS;
		static size_t s_mainThreadID;
	};
} // namespace Lina

#endif
