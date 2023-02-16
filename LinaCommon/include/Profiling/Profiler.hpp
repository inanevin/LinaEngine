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
	struct Block
	{
	public:
		String		   threadName = "";
		StringID	   threadID	  = 0;
		String		   name		  = "";
		double		   durationNS = 0.0;
		double		   startTime  = 0.0;
		double		   initDiff	  = 0.0;
		Block*		   parent	  = nullptr;
		Vector<Block*> children;
	};

	struct Scope
	{
		Scope(const String& blockName, const String& threadName = "Main");
		~Scope();

		String blockName  = "";
		String threadName = "";
	};

	struct ThreadBranch
	{
		Block*		   lastBlock = nullptr;
		Vector<Block*> blocks;
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
		void		   StartBlock(const String& block, const String& thread);
		void		   EndBlock(const String& block, const String& thread);
		void		   DumpFrameAnalysis(const String& path);
		void		   WriteBlockData(String& indent, Block* block, std::ofstream& file);

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
		void CleanupBlock(Block* s);

	private:
		double				 m_totalFrameTimeNS = 0.0;
		Queue<ProfilerFrame> m_frames;
		double				 m_lastCPUQueryTime		  = 0.0;
		bool				 m_totalFrameQueueReached = false;
		DeviceCPUInfo		 m_cpuInfo;
		DeviceGPUInfo		 m_gpuInfo;
		std::mutex			 m_lock;
	};

#define PROFILER_FRAME_START()						Lina::Profiler::Get().StartFrame()
#define PROFILER_STARTBLOCK(BLOCKENAME, THREADNAME) Lina::Profiler::Get().StartBlock(BLOCKAME, THREADNAME)
#define PROFILER_ENDBLOCK(BLOCKNAME, THREADNAME)	Lina::Profiler::Get().EndBlock(BLOCKNAME, THREADNAME)
#define PROFILER_SCOPE(BLOCKNAME, THREADNAME)		Lina::Scope scope(BLOCKNAME, THREADNAME)
#define PROFILER_FUNCTION(...)						Lina::Scope function(__FUNCTION__, __VA_ARGS__)
#define PROFILER_SET_FRAMEANALYSIS_FILE(FILE)		Lina::Profiler::Get().FrameAnalysisFile = FILE
#define PROFILER_THREAD_RENDER						"Render"
#define PROFILER_THREAD_MAIN						"Main"

} // namespace Lina

#else

#define PROFILER_FRAME_START()
#define PROFILER_STARTBLOCK(BLOCKENAME, THREADNAME)
#define PROFILER_ENDBLOCK(BLOCKNAME, THREADNAME)
#define PROFILER_SCOPE(...)
#define PROFILER_SET_FRAMEANALYSIS_FILE(FILE)
#define PROFILER_THREAD_RENDER "Render"
#define PROFILER_THREAD_MAIN   "Main"
#endif

#endif