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

#ifdef LINA_ENABLE_PROFILER

#define MAX_FRAME_BACKTRACE 500
#define BACKTRACE_CLEANUP	200

// Headers here.
#include "Common/Data/String.hpp"
#include "Common/Data/Vector.hpp"
#include "Common/Data/Queue.hpp"
#include "Common/Data/Deque.hpp"
#include "Common/StringID.hpp"
#include "Common/Data/Mutex.hpp"
#include <source_location>

namespace Lina
{
	struct Block
	{
	public:
		const char* name		= "";
		uint64		endCycles	= 0;
		uint64		startCycles = 0;
		bool		open		= false;
		bool		hasParent	= false;
	};

	struct Scope
	{
		Scope(const char* scopeName, StringID thread = "Main"_hs);
		~Scope();

		const char* blockName	= "";
		StringID	blockThread = 0;
		uint32		id			= 0;
	};

	struct ProfilerDrawCall
	{
		const char* category = "";
		StringID	sid		 = 0;
		uint32		tris	 = 0;
		uint32		meta	 = 0;
	};

	struct ProfilerFrame
	{
		uint64													 startCycles = 0;
		uint64													 endCycles	 = 0;
		ParallelHashMapMutex<StringID, Vector<Block>>			 threadBlocks;
		ParallelHashMapMutex<StringID, Vector<ProfilerDrawCall>> drawCalls;
	};

	class Profiler
	{
	public:
		struct DrawInfo
		{
			Atomic<uint32> drawCalls;
			Atomic<uint32> tris;
			Atomic<uint32> vertices;
			Atomic<uint32> indices;
		};

		static Profiler* Get()
		{
			return s_instance;
		}

		void   StartFrame();
		uint32 StartBlock(const char* blockName, StringID threadName);
		void   EndBlock(StringID threadName, uint32 id);
		void   DumpFrameAnalysis(const String& path);
		void   RegisterThread(const char* name, StringID sid);

		void ResetDrawInfo();
		void AddDrawCall(uint32 count);
		void AddTris(uint32 tris);
		void AddVerticesIndices(uint32 vertices, uint32 indices);

		inline ProfilerFrame& GetFrame()
		{
			return m_frameQueue.back();
		}

		inline ProfilerFrame& GetPrevFrame()
		{
			if (m_frameQueue.size() == 1)
				return m_frameQueue.back();

			return *(m_frameQueue.rbegin() + 1);
		}

		inline const DrawInfo& GetDrawInfo() const
		{
			return m_drawInfo;
		}
		const char* FrameAnalysisFile = "lina_frame_analysis.txt";

	protected:
		void Destroy();

	private:
		friend class Application;

		static void Initialize();
		static void Shutdown();

		Profiler() = default;
		~Profiler()
		{
			Destroy();
		}

	private:
		static Profiler*			   s_instance;
		HashMap<StringID, const char*> m_threadNames;
		Deque<ProfilerFrame>		   m_frameQueue;
		double						   m_lastCPUQueryTime		= 0.0;
		bool						   m_totalFrameQueueReached = false;
		DrawInfo					   m_drawInfo;
	};

#define PROFILER_INIT						  Lina::Profiler::Initialize()
#define PROFILER_SHUTDOWN					  Lina::Profiler::Shutdown()
#define PROFILER_FRAME_START()				  Lina::Profiler::Get()->StartFrame()
#define PROFILER_STARTBLOCK(BLOCKNAME)		  Lina::Profiler::Get()->StartBlock(BLOCKNAME, static_cast<StringID>(std::hash<std::thread::id>{}(std::this_thread::get_id())))
#define PROFILER_ENDBLOCK(X)				  Lina::Profiler::Get()->EndBlock(static_cast<StringID>(std::hash<std::thread::id>{}(std::this_thread::get_id())), X)
#define PROFILER_FUNCTION(...)				  Lina::Scope function(__FUNCTION__, static_cast<StringID>(std::hash<std::thread::id>{}(std::this_thread::get_id())))
#define PROFILER_SET_FRAMEANALYSIS_FILE(FILE) Lina::Profiler::Get()->FrameAnalysisFile = FILE
#define PROFILER_REGISTER_THREAD(NAME)		  Lina::Profiler::Get()->RegisterThread(NAME, static_cast<StringID>(std::hash<std::thread::id>{}(std::this_thread::get_id())))

#define PROFILER_RESET_DRAWINFO()		   Lina::Profiler::Get()->ResetDrawInfo()
#define PROFILER_ADD_DRAWCALL(X)		   Lina::Profiler::Get()->AddDrawCall(X)
#define PROFILER_ADD_TRIS(X)			   Lina::Profiler::Get()->AddTris(X)
#define PROFILER_ADD_VERTICESINDICES(X, Y) Lina::Profiler::Get()->AddVerticesIndices(X, Y)

} // namespace Lina

#else

#define PROFILER_INIT
#define PROFILER_SHUTDOWN
#define PROFILER_FRAME_START()
#define PROFILER_STARTBLOCK(BLOCKNAME) 0
#define PROFILER_ENDBLOCK(X)
#define PROFILER_FUNCTION(...)
#define PROFILER_SET_FRAMEANALYSIS_FILE(FILE)
#define PROFILER_REGISTER_THREAD(NAME)

#define PROFILER_RESET_DRAWINFO()
#define PROFILER_ADD_DRAWCALL(X)
#define PROFILER_ADD_TRIS(X)
#define PROFILER_ADD_VERTICESINDICES(X, Y)

#endif
