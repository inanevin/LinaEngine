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

#include "Common/Common.hpp"
#include <thread>

namespace LinaGX
{
	enum class Format;
}

namespace Lina
{
	class Application;
	class Engine;

	class SystemInfo
	{
	public:
		static constexpr float IDEAL_DT_F = 0.016667f;

		static inline int64 GetAppTimeMicroseconds()
		{
			return s_appTimeMicroseconds;
		}

		static inline int64 GetAppStartCPUMicroseconds()
		{
			return s_appStartMicroseconds;
		}

		static inline int64 GetCurrentSystemTimeMicroseconds()
		{
			return s_currentSystemTimeMicroseconds;
		}

		static inline int64 GetAppStartSystemTimeMicroseconds()
		{
			return s_appStartSystemTimeMicroseconds;
		}

		static inline double GetAppTime()
		{
			return s_appTime;
		}

		static inline float GetAppTimeF()
		{
			return static_cast<float>(s_appTime);
		}

		static inline double GetDeltaTime()
		{
			return s_deltaTimeSeconds;
		}

		static inline double GetSmoothedDeltaTime()
		{
			return s_smoothedDeltaSeconds;
		}

		static inline int64 GetSmoothedDeltaMicroseconds()
		{
			return s_smoothedDeltaMicroseconds;
		}

		static inline int64 GetDeltaTimeMicroSeconds()
		{
			return s_deltaTimeMicroseconds;
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

		static inline bool GetRendererBehindFrames()
		{
			return s_rendererBehindFrames;
		}

		static inline LinaGX::Format GetSwapchainFormat()
		{
			return s_swapchainFormat;
		}

		static inline LinaGX::Format GetHDRFormat()
		{
			return s_hdrFormat;
		}

		static inline LinaGX::Format GetLDRFormat()
		{
			return s_ldrFormat;
		}

		static inline LinaGX::Format GetDepthFormat()
		{
			return s_depthFormat;
		}

		static inline uint32 GetAllowedMSAASamples()
		{
			return s_allowedMSAA;
		}

	private:
		friend class Application;
		friend class Engine;

		static double CalculateRunningAverageDT(int64 deltaMicroseconds);

		static inline void SetAppTimeMicroseconds(int64 microseconds)
		{
			s_appTimeMicroseconds = microseconds;
		}

		static inline void SetAppStartCPUMicroseconds(int64 microseconds)
		{
			s_appStartMicroseconds = microseconds;
		}

		static inline void SetCurrentSystemTimeMicroseconds(int64 microseconds)
		{
			s_currentSystemTimeMicroseconds = microseconds;
		}

		static inline void SetAppStartSystemTimeMicroseconds(int64 microseconds)
		{
			s_appStartSystemTimeMicroseconds = microseconds;
		}

		static inline void SetSmoothedDeltaMicroseconds(int64 microseconds)
		{
			s_smoothedDeltaMicroseconds = microseconds;
			s_smoothedDeltaSeconds		= static_cast<double>(microseconds) * 0.000001;
		}

		static inline void SetAppTime(double seconds)
		{
			s_appTime = seconds;
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

		static inline void SetDeltaTimeMicroseconds(int64 microseconds)
		{
			s_deltaTimeMicroseconds = microseconds;
			s_deltaTimeSeconds		= static_cast<double>(microseconds) * 0.000001;
		}

		static inline void SetAppStartCycles(int64 cycles)
		{
			s_appStartCycles = cycles;
		}

		static inline void SetMainThreadID(size_t id)
		{
			s_mainThreadID = id;
		}

		static inline void SetRendererBehindFrames(uint32 frames)
		{
			s_rendererBehindFrames = frames;
		}

		static inline void SetSwapchainFormat(LinaGX::Format format)
		{
			s_swapchainFormat = format;
		}

		static inline void SetHDRFormat(LinaGX::Format format)
		{
			s_hdrFormat = format;
		}

		static inline void SetLDRFormat(LinaGX::Format format)
		{
			s_ldrFormat = format;
		}

		static inline void SetDepthFormat(LinaGX::Format format)
		{
			s_depthFormat = format;
		}

		static inline void SetAllowedMSAASamples(uint32 samples)
		{
			s_allowedMSAA = samples;
		}

	private:
		static double		  s_appTime;
		static double		  s_deltaTimeSeconds;
		static double		  s_timescale;
		static bool			  s_appHasFocus;
		static int64		  s_currentSystemTimeMicroseconds;
		static int64		  s_appTimeMicroseconds;
		static int64		  s_appStartMicroseconds;
		static int64		  s_appStartSystemTimeMicroseconds;
		static int64		  s_smoothedDeltaMicroseconds;
		static double		  s_smoothedDeltaSeconds;
		static int64		  s_deltaTimeMicroseconds;
		static int64		  s_appStartCycles;
		static uint64		  s_frames;
		static uint32		  s_measuredFPS;
		static size_t		  s_mainThreadID;
		static uint32		  s_rendererBehindFrames;
		static LinaGX::Format s_swapchainFormat;
		static LinaGX::Format s_hdrFormat;
		static LinaGX::Format s_ldrFormat;
		static LinaGX::Format s_depthFormat;
		static uint32		  s_allowedMSAA;
	};
} // namespace Lina
