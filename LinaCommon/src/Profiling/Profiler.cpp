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
#include "Core/Time.hpp"
#include "Memory/Memory.hpp"
#include "Log/Log.hpp"
#include "FileSystem/FileSystem.hpp"
#include <fstream>
#include <iostream>

#ifdef LINA_PLATFORM_WINDOWS
#include <process.h>
#include <iostream>
#include <Windows.h>
#include <psapi.h>
#include <DbgHelp.h>
#include <Pdh.h>
#include <PdhMsg.h>
#pragma comment(lib, "pdh.lib")
#endif

namespace Lina
{
#define QUERY_CPU_INTERVAL_SECS 2
    bool g_skipAllocTrack = false;

    DeviceMemoryInfo Profiler::QueryMemoryInfo()
    {
        DeviceMemoryInfo info;

#ifdef LINA_PLATFORM_WINDOWS
        MEMORYSTATUSEX memInfo;
        memInfo.dwLength = sizeof(MEMORYSTATUSEX);
        GlobalMemoryStatusEx(&memInfo);
        info.totalVirtualMemory     = static_cast<unsigned long>(memInfo.ullTotalPageFile);
        info.totalUsedVirtualMemory = static_cast<unsigned long>(memInfo.ullTotalPageFile - memInfo.ullAvailPageFile);
        info.totalRAM               = static_cast<unsigned long>(memInfo.ullTotalPhys);
        info.totalUsedRAM           = static_cast<unsigned long>(memInfo.ullTotalPhys - memInfo.ullAvailPhys);
        PROCESS_MEMORY_COUNTERS_EX pmc;
        GetProcessMemoryInfo(GetCurrentProcess(), (PROCESS_MEMORY_COUNTERS*)&pmc, sizeof(pmc));
        info.totalProcessVirtualMemory = static_cast<unsigned long>(pmc.PrivateUsage);
        info.totalProcessRAM           = static_cast<unsigned long>(pmc.WorkingSetSize);
#else
        LINA_ERR("[Profiler] -> Memory query for other platforms not implemented!");
#endif

        return info;
    }

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

        const double  time    = Time::GetCPUTime();
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
        g_skipAllocTrack        = true;
        const double cpuTimeNow = Time::GetCPUTime();

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

        Frame f;
        f.startTime = cpuTimeNow;
        m_frames.emplace(f);

        g_skipAllocTrack = false;
    }

    void Profiler::StartScope(const String& scope, const String& thread)
    {
        m_lock.lock();
        g_skipAllocTrack         = true;
        const double  cpuTimeNow = Time::GetCPUTime();
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
        g_skipAllocTrack = false;
        m_lock.unlock();
    }

    void Profiler::EndScope(const String& scope, const String& thread)
    {
        m_lock.lock();
        ThreadBranch& branch         = m_frames.back().threadBranches[thread];
        branch.lastScope->durationNS = (Time::GetCPUTime() - branch.lastScope->startTime) * 1.0e9;
        branch.lastScope             = branch.lastScope->parent;
        m_lock.unlock();
    }

    void Profiler::OnAllocation(void* ptr, size_t size)
    {
        LOCK_GUARD(m_lock);

        if (g_skipAllocTrack)
            return;

        g_skipAllocTrack = true;
        MemAllocationInfo info;
        info.size = size;
        CaptureTrace(info);
        m_memAllocations[ptr] = info;
        m_totalMemAllocationSize += size;
        g_skipAllocTrack = false;
    }
    void Profiler::OnVRAMAllocation(void* ptr, size_t size)
    {
        g_skipAllocTrack = true;
        MemAllocationInfo info;
        info.size = size;
        CaptureTrace(info);
        m_memAllocations[ptr] = info;
        m_totalVRAMAllocationSize += size;
        g_skipAllocTrack = false;
    }

    void Profiler::OnFree(void* ptr)
    {
        LOCK_GUARD(m_lock);

        if (g_skipAllocTrack)
            return;

        auto it = m_memAllocations.find(ptr);
        if (it == m_memAllocations.end())
            return;

        g_skipAllocTrack = true;
        m_totalMemAllocationSize -= it->second.size;
        m_memAllocations.erase(ptr);
        g_skipAllocTrack = false;
    }

    void Profiler::OnVRAMFree(void* ptr)
    {
        m_lock.lock();
        g_skipAllocTrack = true;
        m_totalVRAMAllocationSize -= m_memAllocations[ptr].size;
        m_vramAllocations.erase(ptr);
        g_skipAllocTrack = false;
        m_lock.unlock();
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

    void Profiler::DumpMemoryLeaks(const String& path)
    {
        if (FileSystem::FileExists(path))
            FileSystem::DeleteFileInPath(path);

        std::ofstream file(path.c_str());

        auto writeTrace = [&](ParallelHashMap<void*, MemAllocationInfo>& map) {
            for (auto& alloc : map)
            {
                if (alloc.first == 0)
                    continue;
                std::ostringstream ss;

                ss << "****************** LEAK DETECTED ******************\n";
                ss << "Size: " << alloc.second.size << " bytes \n";

#ifdef LINA_PLATFORM_WINDOWS
                HANDLE      process = GetCurrentProcess();
                static bool inited  = false;

                if (!inited)
                {
                    inited = true;
                    SymInitialize(process, nullptr, TRUE);
                }

                void* symbolAll = calloc(sizeof(SYMBOL_INFO) + MAX_SYM_NAME * sizeof(TCHAR), 1);

                if (symbolAll == NULL)
                    return;

                SYMBOL_INFO* symbol  = static_cast<SYMBOL_INFO*>(symbolAll);
                symbol->MaxNameLen   = 255;
                symbol->SizeOfStruct = sizeof(SYMBOL_INFO);

                DWORD            displacement;
                IMAGEHLP_LINE64* line = NULL;
                line                  = (IMAGEHLP_LINE64*)std::malloc(sizeof(IMAGEHLP_LINE64));

                if (line == NULL)
                    return;

                line->SizeOfStruct = sizeof(IMAGEHLP_LINE64);

                for (int i = 0; i < alloc.second.stackSize; ++i)
                {
                    ss << "------ Stack Trace " << i << "------\n";

                    DWORD64 address = (DWORD64)(alloc.second.stack[i]);

                    SymFromAddr(process, address, NULL, symbol);

                    if (SymGetLineFromAddr64(process, address, &displacement, line))
                    {
                        ss << "Location:" << line->FileName << "\n";
                        ss << "Smybol:" << symbol->Name << "\n";
                        ss << "Line:" << line->LineNumber << "\n";
                        ss << "SymbolAddr:" << symbol->Address << "\n";
                    }
                    else
                    {
                        ss << "Smybol:" << symbol->Name << "\n";
                        ss << "SymbolAddr:" << symbol->Address << "\n";
                    }

                    IMAGEHLP_MODULE64 moduleInfo;
                    moduleInfo.SizeOfStruct = sizeof(moduleInfo);
                    if (::SymGetModuleInfo64(process, symbol->ModBase, &moduleInfo))
                        ss << "Module:" << moduleInfo.ModuleName << "\n";
                }
                std::free(line);
                std::free(symbolAll);
#endif

                ss << "\n";
                ss << "\n";

                if (ss.str().find("taskflow\\core\\executor.hpp") == std::string::npos)
                    file << ss.str();
                else
                    alloc.second.skipped = true;
            }
        };

        if (file.is_open())
        {
            size_t totalSizeBytes = 0;
            size_t totalSizeKB    = 0;

            for (auto& alloc : m_memAllocations)
                totalSizeBytes += alloc.second.size;

            totalSizeKB = static_cast<size_t>(static_cast<float>(totalSizeBytes) / 1000.0f);

            file << "-------------------------------------------\n";
            file << "PROCESS MEMORY LEAKS\n";
            file << "Total leaked size: " << totalSizeBytes << " (bytes) " << totalSizeKB << " (kb)\n";
            file << "-------------------------------------------\n";
            file << "\n";

            writeTrace(m_memAllocations);

            totalSizeBytes = totalSizeKB = 0;

            for (auto& alloc : m_vramAllocations)
                totalSizeBytes += alloc.second.size;

            totalSizeKB = static_cast<size_t>(static_cast<float>(totalSizeBytes) / 1000.0f);

            file << "-------------------------------------------\n";
            file << "VRAM MEMORY LEAKS\n";
            file << "Total leaked size: " << totalSizeBytes << " (bytes) " << totalSizeKB << " (mb)\n";
            file << "-------------------------------------------\n";
            file << "\n";

            writeTrace(m_vramAllocations);

            file.close();
        }
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

    void Profiler::CaptureTrace(MemAllocationInfo& info)
    {

#ifdef LINA_PLATFORM_WINDOWS
        info.stackSize = CaptureStackBackTrace(3, MEMORY_STACK_TRACE_SIZE, info.stack, nullptr);
#else

        void*  stack[MEMORY_STACK_TRACE_SIZE];
        int    frames       = backtrace(stack, numElementsInArray(stack));
        char** frameStrings = backtrace_symbols(stack, frames);

        for (int i = 4; i < frames; ++i)
        {
            // TODO: Use backtrace to implement Linux & MacOs functionality.
            LINA_ERR("Backtrace is not implemented yet! Profiler allocation data will be empty.");
        }

        ::free(frameStrings);
#endif
    }

    Profiler::~Profiler()
    {
        Shutdown();
    }

    void Profiler::Shutdown()
    {
        g_skipAllocTrack = true;

        while (!m_frames.empty())
        {
            auto f = m_frames.front();
            CleanupFrame(f);
            m_frames.pop();
        }
        g_skipAllocTrack = false;

        DumpMemoryLeaks("lina_memory_leaks.txt");
        LINA_TRACE("[Shutdown] -> Profiler {0}", typeid(*this).name());

#ifdef LINA_PLATFORM_WINDOWS
        HANDLE process = GetCurrentProcess();
        SymCleanup(process);
#endif
    }

    void Profiler::CleanupFrame(Frame& frame)
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