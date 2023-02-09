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

#ifndef Profiler_HPP
#define Profiler_HPP

#ifdef LINA_ENABLE_PROFILING

#define MAX_FRAME_BACKTRACE 500

// Headers here.
#include "Data/String.hpp"
#include "Data/Vector.hpp"
#include "Data/Queue.hpp"
#include "Core/StringID.hpp"
#include "Core/ISingleton.hpp"
#include <source_location>

namespace Lina
{
	struct Scope
	{
	public:
		String		   threadName = "";
		StringID	   threadID	  = 0;
		String		   name		  = "";
		double		   durationNS = 0.0;
		double		   startTime  = 0.0;
		double		   initDiff	  = 0.0;
		Scope*		   parent	  = nullptr;
		Vector<Scope*> children;
	};

	struct Function
	{
		Function(const String& funcName, const String& threadName = "Main");
		~Function();

		String ScopeName  = "";
		String threadName = "";
	};

	struct ThreadBranch
	{
		Scope*		   lastScope = nullptr;
		Vector<Scope*> scopes;
	};

	struct ProfilerFrame
	{
		double						  durationNS = 0.0;
		double						  startTime	 = 0.0;
		HashMap<String, ThreadBranch> threadBranches;
	};

	struct DeviceCPUInfo
	{
		// Total % of CPU used by this process.
		double processUse = 0.0;

		// Total % of CPU utilization by this process.
		double processUtilization = 0.0;

		int						numberOfCores	   = 0;
		double					averageFrameTimeNS = 0.0;
		HashMap<uint64, double> cpuUsages;
	};

	struct DeviceGPUInfo
	{
		uint64 m_memoryHeapSize	 = 0;
		uint32 m_memoryHeapCount = 0;
	};

	class Profiler : public ISingleton
	{
	public:
		static Profiler& Get()
		{
			static Profiler instance;
			return instance;
		}

		DeviceCPUInfo& QueryCPUInfo();
		void		   StartFrame();
		void		   StartScope(const String& scope, const String& thread);
		void		   EndScope(const String& scope, const String& thread);
		void		   DumpFrameAnalysis(const String& path);
		void		   WriteScopeData(String& indent, Scope* scope, std::ofstream& file);

		inline void SetGPUInfo(const DeviceGPUInfo& info)
		{
			m_gpuInfo = info;
		}

		const char* FrameAnalysisFile = "lina_frame_analysis.txt";

	protected:
		virtual void Destroy() override;

	private:
		friend class GlobalAllocatorWrapper;

		Profiler(){};
		virtual ~Profiler()
		{
			Destroy();
		}

		void CleanupFrame(ProfilerFrame& frame);
		void CleanupScope(Scope* s);

	private:
		double		  m_totalFrameTimeNS = 0.0;
		Queue<ProfilerFrame>  m_frames;
		double		  m_lastCPUQueryTime	   = 0.0;
		bool		  m_totalFrameQueueReached = false;
		DeviceCPUInfo m_cpuInfo;
		DeviceGPUInfo m_gpuInfo;
		std::mutex	  m_lock;
	};

#define PROFILER_FRAME_START()						Lina::Profiler::Get().StartFrame()
#define PROFILER_SCOPE_START(SCOPENAME, THREADNAME) Lina::Profiler::Get().StartScope(SCOPENAME, THREADNAME)
#define PROFILER_SCOPE_END(SCOPENAME, THREADNAME)	Lina::Profiler::Get().EndScope(SCOPENAME, THREADNAME)
#define PROFILER_FUNC(...)							Lina::Function func(__FUNCTION__, __VA_ARGS__)
#define PROFILER_SET_FRAMEANALYSIS_FILE(FILE)		Lina::Profiler::Get().FrameAnalysisFile = FILE
#define PROFILER_THREAD_RENDER						"Render"
#define PROFILER_THREAD_MAIN						"Main"

} // namespace Lina

#else

#define PROFILER_FRAME_START()
#define PROFILER_SCOPE_START(SCOPENAME, THREADNAME)
#define PROFILER_SCOPE_END(SCOPENAME, THREADNAME)
#define PROFILER_FUNC(...)
#define PROFILER_ALLOC(PTR, SZ)
#define PROFILER_VRAMALLOC(PTR, SZ)
#define PROFILER_FREE(PTR)
#define PROFILER_VRAMFREE(PTR)
#define PROFILER_SKIPTRACK(skip)
#define PROFILER_DUMP_FRAME_ANALYSIS(PATH)
#define PROFILER_THREAD_RENDER
#define PROFILER_THREAD_INPUT
#define PROFILER_THREAD_MAIN
#define PROFILER_THREAD_MAIN
#define PROFILER_ENABLE_MEMORYLEAK_DUMP(ENABLE)
#endif

#endif