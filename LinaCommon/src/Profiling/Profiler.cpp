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

#ifdef LINA_ENABLE_PROFILING

#include "Profiling/Profiler.hpp"
#include "Platform/PlatformTimeIncl.hpp"
#include "Memory/Memory.hpp"
#include "Log/Log.hpp"
#include "FileSystem/FileSystem.hpp"
#include <fstream>
#include <iostream>

#ifdef LINA_PLATFORM_WINDOWS
#include <Windows.h>
#endif

namespace Lina
{
#define QUERY_CPU_INTERVAL_SECS 2

    DeviceCPUInfo& Profiler::QueryCPUInfo()
    {
#ifdef LINA_PLATFORM_WINDOWS
        static bool           inited        = false;
        static int            numProcessors = 0;
        static HANDLE         self;
        static ULARGE_INTEGER lastCPU, lastSysCPU, lastUserCPU;

        if (!inited)
        {
            // Querying CPU usage
            SYSTEM_INFO sysInfo;
            FILETIME    ftime, fsys, fuser;

            GetSystemInfo(&sysInfo);
            numProcessors = sysInfo.dwNumberOfProcessors;

            GetSystemTimeAsFileTime(&ftime);
            memcpy(&lastCPU, &ftime, sizeof(FILETIME));

            self = GetCurrentProcess();
            if (GetProcessTimes(self, &ftime, &ftime, &fsys, &fuser))
            {
                memcpy(&lastSysCPU, &fsys, sizeof(FILETIME));
                memcpy(&lastUserCPU, &fuser, sizeof(FILETIME));
            }
            inited                  = true;
            m_cpuInfo.numberOfCores = std::thread::hardware_concurrency();
        }

        const double  time    = PlatformTime::GetSeconds();
        static double percent = 0.0;

        if (time - m_lastCPUQueryTime > QUERY_CPU_INTERVAL_SECS)
        {
            m_lastCPUQueryTime = time;

            /* QUERYING CPU INFO */
            FILETIME       ftime, fsys, fuser;
            ULARGE_INTEGER now, sys, user;

            GetSystemTimeAsFileTime(&ftime);
            memcpy(&now, &ftime, sizeof(FILETIME));

            if (GetProcessTimes(self, &ftime, &ftime, &fsys, &fuser))
            {
                memcpy(&sys, &fsys, sizeof(FILETIME));
                memcpy(&user, &fuser, sizeof(FILETIME));
                percent = static_cast<double>((sys.QuadPart - lastSysCPU.QuadPart) + (user.QuadPart - lastUserCPU.QuadPart));

                m_cpuInfo.processUtilization = (percent / 100000.0) / static_cast<double>(numProcessors);

                auto diff = now.QuadPart - lastCPU.QuadPart;
                if (diff != 0)
                    percent /= static_cast<double>(diff);

                percent /= static_cast<double>(numProcessors);
                lastCPU              = now;
                lastUserCPU          = user;
                lastSysCPU           = sys;
                m_cpuInfo.processUse = percent * 100.0f;
            }
        }

        // info.processUse              = percent * 100.0;
        m_cpuInfo.averageFrameTimeNS = m_totalFrameTimeNS / (m_totalFrameQueueReached ? static_cast<double>(MAX_FRAME_BACKTRACE) : static_cast<double>(m_frames.size()));

#else
        LINA_ERR("[Profiler] -> CPU query for other platforms not implemented!");
#endif

        return m_cpuInfo;
    }

    void Profiler::StartFrame()
    {
        const double cpuTimeNow = PlatformTime::GetSeconds();

        if (m_frames.size() == MAX_FRAME_BACKTRACE)
        {
            CleanupFrame(m_frames.front());
            m_totalFrameTimeNS -= m_frames.front().durationNS;
            m_frames.pop();
        }

        if (!m_frames.empty())
        {
            m_frames.back().durationNS = (cpuTimeNow - m_frames.back().startTime) * 1.0e9;
            m_totalFrameTimeNS += m_frames.back().durationNS;
        }

        ProfilerFrame f;
        f.startTime = cpuTimeNow;
        m_frames.emplace(f);
    }

    void Profiler::StartScope(const String& scope, const String& thread)
    {
        LOCK_GUARD(m_lock);
        const double  cpuTimeNow = PlatformTime::GetSeconds();
        ThreadBranch& branch     = m_frames.back().threadBranches[thread];

        Scope* s      = new Scope();
        s->name       = scope;
        s->threadName = thread;
        s->startTime  = cpuTimeNow;
        s->parent     = branch.lastScope;
        s->threadID   = TO_SID(thread);

        if (branch.lastScope == nullptr)
            branch.scopes.push_back(s);
        else
            branch.lastScope->children.push_back(s);

        branch.lastScope = s;
        s->initDiff      = PlatformTime::GetSeconds(); - cpuTimeNow;
    }

    void Profiler::EndScope(const String& scope, const String& thread)
    {
        LOCK_GUARD(m_lock);
        ThreadBranch& branch         = m_frames.back().threadBranches[thread];
        branch.lastScope->durationNS = (PlatformTime::GetSeconds() - branch.lastScope->startTime + branch.lastScope->initDiff) * 1.0e9;
        branch.lastScope             = branch.lastScope->parent;
    }

    Function::Function(const String& funcName, const String& thread)
    {
        ScopeName  = funcName;
        threadName = thread;
        Profiler::Get().StartScope(funcName, thread);
    }

    Function::~Function()
    {
        Profiler::Get().EndScope(ScopeName, threadName);
    }

    void Profiler::DumpFrameAnalysis(const String& path)
    {
        if (FileSystem::FileExists(path))
            FileSystem::DeleteFileInPath(path);

        std::ofstream file(path.c_str());

        if (file.is_open())
        {

            const double avgTimeNS = m_totalFrameTimeNS / static_cast<double>(m_frames.size());
            const double avgTimeMS = avgTimeNS * 0.000001;

            file << "-------------------------------------------\n";
            file << "FRAME ANALYSIS - SHOWING LAST " << m_frames.size() << " FRAMES \n";
            file << "-------------------------------------------\n";
            file << "Average Frame Time: " << avgTimeNS << " (NS) " << avgTimeMS << " (MS) \n";
            file << "Average FPS: " << static_cast<int>(1000.0 / avgTimeMS) << "\n";
            file << "\n";

            int frameCounter = 0;
            while (!m_frames.empty())
            {
                auto f = m_frames.front();

                file << "------------------------------------ FRAME " << frameCounter << "------------------------------------\n";
                file << "Duration: " << f.durationNS << " (NS) " << f.durationNS * 0.000001 << " (MS)\n";
                file << "\n";

                for (const auto& t : f.threadBranches)
                {
                    double threaddurationNS = 0.0;
                    for (auto s : t.second.scopes)
                        threaddurationNS += s->durationNS;

                    file << "********************************* THREAD: " << t.first.c_str() << " *********************************\n";
                    file << "** Total Duration: " << threaddurationNS << " (NS) " << threaddurationNS * 0.000001 << " (MS)\n";
                    String indent = "** ";
                    for (auto s : t.second.scopes)
                        WriteScopeData(indent, s, file);

                    file << "\n";
                }

                file << "\n";
                CleanupFrame(f);
                m_frames.pop();
                frameCounter++;
            }

            file.close();
        }
    }

    void Profiler::WriteScopeData(String& indent, Scope* scope, std::ofstream& file)
    {
        String newIndent = indent;
        file << "**\n";
        file << indent.c_str() << "-------- " << scope->name.c_str() << " --------\n";
        file << indent.c_str() << "Duration: " << scope->durationNS * 0.000001 << " (MS)\n";
        file << indent.c_str() << "Thread: " << scope->threadName.c_str() << "\n";
        newIndent += "    ";
        for (auto s : scope->children)
            WriteScopeData(newIndent, s, file);
    }

    void Profiler::Destroy()
    {
        if (m_destroyed)
            return;

        m_destroyed = true;
        if (FrameAnalysisFile != NULL && FrameAnalysisFile[0] != '\0')
            DumpFrameAnalysis(FrameAnalysisFile);

        while (!m_frames.empty())
        {
            auto f = m_frames.front();
            CleanupFrame(f);
            m_frames.pop();
        }
    }

    void Profiler::CleanupFrame(ProfilerFrame& frame)
    {
        for (auto& [threadName, threadInfo] : frame.threadBranches)
        {
            for (auto* scope : threadInfo.scopes)
                CleanupScope(scope);

            threadInfo.scopes.clear();
        }
    }

    void Profiler::CleanupScope(Scope* scope)
    {
        for (auto s : scope->children)
            CleanupScope(s);

        delete scope;
    }

} // namespace Lina

#endif