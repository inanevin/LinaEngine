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

#ifdef LINA_ENABLE_PROFILER

#include "Common/Profiling/Profiler.hpp"
#include "Common/Platform/PlatformTime.hpp"
#include "Common/Memory/Memory.hpp"
#include "Common/Log/Log.hpp"
#include "Common/FileSystem/FileSystem.hpp"
#include "Common/Platform/PlatformInclude.hpp"
#include <fstream>
#include <iostream>

namespace Lina
{

	Profiler* Profiler::s_instance = nullptr;

	void Profiler::Initialize()
	{
		s_instance = new Profiler();
		s_instance->m_frameQueue.resize(MAX_FRAME_BACKTRACE);
	}

	void Profiler::Shutdown()
	{
		delete s_instance;
	}

	void Profiler::StartFrame()
	{
		const int32 diff = MAX_FRAME_BACKTRACE - static_cast<int32>(m_frameQueue.size());
		if (diff < 0)
		{
			for (int i = 0; i < BACKTRACE_CLEANUP; i++)
				m_frameQueue.pop_front();
		}

		const uint64 cyclesNow = PlatformTime::GetCPUCycles();

		if (!m_frameQueue.empty())
		{
			auto& pf	 = m_frameQueue.back();
			pf.endCycles = cyclesNow;
		}

		ProfilerFrame frame;
		frame.startCycles = cyclesNow;
		m_frameQueue.push_back(frame);
	}

	void Profiler::ResetDrawInfo()
	{
		m_drawInfo.drawCalls.store(0);
		m_drawInfo.tris.store(0);
		m_drawInfo.vertices.store(0);
		m_drawInfo.indices.store(0);
	}

	void Profiler::AddDrawCall(uint32 count)
	{
		m_drawInfo.drawCalls.fetch_add(count);
	}

	void Profiler::AddTris(uint32 tris)
	{
		m_drawInfo.tris.fetch_add(tris);
	}

	void Profiler::AddVerticesIndices(uint32 vertices, uint32 indices)
	{
		m_drawInfo.indices.fetch_add(indices);
		m_drawInfo.vertices.fetch_add(vertices);
	}

	uint32 Profiler::StartBlock(const char* blockName, StringID thread)
	{
		const uint64 cycles = PlatformTime::GetCPUCycles();
		auto&		 frame	= m_frameQueue.back();

		Block block;
		block.name		  = blockName;
		block.startCycles = PlatformTime::GetCPUCycles();
		block.open		  = true;

		uint32 id = 0;

		frame.threadBlocks.try_emplace_l(
			thread,
			[&block, &id](auto& blocks) {
				id = static_cast<uint32>(blocks.second.size());

				for (const auto& b : blocks.second)
				{
					if (b.open)
						block.hasParent = true;
				}

				blocks.second.push_back(block);
			},
			Vector<Block>{block});
		return id;
	}

	void Profiler::EndBlock(StringID thread, uint32 id)
	{
		const uint64 cycles = PlatformTime::GetCPUCycles();
		auto&		 frame	= m_frameQueue.back();
		frame.threadBlocks.modify_if(thread, [cycles, id](auto& blocks) {
			blocks.second[id].endCycles = cycles;
			blocks.second[id].open		= false;
		});
	}

	Scope::Scope(const char* funcName, StringID thread)
	{
		blockName	= funcName;
		blockThread = thread;
		id			= Profiler::Get()->StartBlock(funcName, thread);
	}

	Scope::~Scope()
	{
		Profiler::Get()->EndBlock(blockThread, id);
	}

	void Profiler::DumpFrameAnalysis(const String& path)
	{
		if (FileSystem::FileOrPathExists(path))
			FileSystem::DeleteFileInPath(path);

		std::ofstream file(path.c_str());

		if (file.is_open())
		{

			file << "-------------------------------------------\n";
			file << "FRAME ANALYSIS - SHOWING LAST " << m_frameQueue.size() << " FRAMES \n";
			file << "-------------------------------------------\n";
			file << "\n";

			int frameCounter = 0;
			while (!m_frameQueue.empty())
			{
				const auto& f = m_frameQueue.front();

				file << "------------------------------------ FRAME " << frameCounter << "------------------------------------\n";
				const double durationMS = PlatformTime::GetDeltaSeconds64(f.startCycles, f.endCycles) * 1000;
				file << "Duration: " << durationMS << " (MS)\n";
				file << "\n";

				for (const auto& [sid, blocks] : f.threadBlocks)
				{
					double threadduration = 0.0;
					for (auto& b : blocks)
					{
						if (b.hasParent)
							continue;

						const double dur = PlatformTime::GetDeltaSeconds64(b.startCycles, b.endCycles);
						threadduration += dur;
					}

					auto   it		  = m_threadNames.find(sid);
					String threadName = TO_STRING(sid);
					if (it != m_threadNames.end())
						threadName = it->second;

					file << "********************************* THREAD: " << threadName.c_str() << " *********************************\n";
					file << "** Total Duration: " << threadduration * 1000 << " (MS)\n";
					const char* indent = "** ";

					for (auto& b : blocks)
					{
						const double dur = PlatformTime::GetDeltaSeconds64(b.startCycles, b.endCycles) * 1000;
						file << indent << "-------- " << b.name << " --------\n";
						file << indent << "Duration: " << dur << " (MS)\n";
					}

					file << "\n";
				}

				file << "\n";
				m_frameQueue.pop_front();
				frameCounter++;
			}

			file.close();
		}
	}

	void Profiler::RegisterThread(const char* name, StringID sid)
	{
		m_threadNames[sid] = name;
	}

	void Profiler::Destroy()
	{
		if (FrameAnalysisFile != NULL && FrameAnalysisFile[0] != '\0')
			DumpFrameAnalysis(FrameAnalysisFile);

		m_frameQueue = Deque<ProfilerFrame>();
	}

} // namespace Lina

#endif
