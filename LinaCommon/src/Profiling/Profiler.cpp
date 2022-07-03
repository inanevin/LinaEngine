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
#include <iostream>
#include <fstream>
#include "Utility/UtilityFunctions.hpp"

#ifdef LINA_PLATFORM_WINDOWS
#include <process.h>
#include <iostream>
#include <Windows.h>
#include <psapi.h>
#include <DbgHelp.h>
#endif

namespace Lina
{
    Profiler* Profiler::s_instance = nullptr;

    DeviceMemoryInfo Profiler::QueryMemoryInfo()
    {
        DeviceMemoryInfo info;

#ifdef LINA_PLATFORM_WINDOWS
        MEMORYSTATUSEX memInfo;
        memInfo.dwLength = sizeof(MEMORYSTATUSEX);
        GlobalMemoryStatusEx(&memInfo);
        info.TotalVirtualMemory     = static_cast<unsigned long>(memInfo.ullTotalPageFile);
        info.TotalUsedVirtualMemory = static_cast<unsigned long>(memInfo.ullTotalPageFile - memInfo.ullAvailPageFile);
        info.TotalRAM               = static_cast<unsigned long>(memInfo.ullTotalPhys);
        info.TotalUsedRAM           = static_cast<unsigned long>(memInfo.ullTotalPhys - memInfo.ullAvailPhys);
        PROCESS_MEMORY_COUNTERS_EX pmc;
        GetProcessMemoryInfo(GetCurrentProcess(), (PROCESS_MEMORY_COUNTERS*)&pmc, sizeof(pmc));
        info.TotalProcessVirtualMemory = static_cast<unsigned long>(pmc.PrivateUsage);
        info.TotalProcessRAM           = static_cast<unsigned long>(pmc.WorkingSetSize);
#else
        LINA_ERR("[Profiler] -> Memory query for other platforms not implemented!");
#endif

        return info;
    }

    DeviceCPUInfo Profiler::QueryCPUInfo()
    {
        DeviceCPUInfo info;

#ifdef LINA_PLATFORM_WINDOWS
        static bool           inited        = false;
        static int            numProcessors = 0;
        static HANDLE         self;
        static ULARGE_INTEGER lastCPU, lastSysCPU, lastUserCPU;

        if (!inited)
        {
            inited = true;
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
        }

        FILETIME       ftime, fsys, fuser;
        ULARGE_INTEGER now, sys, user;
        double         percent = 0.0;

        GetSystemTimeAsFileTime(&ftime);
        memcpy(&now, &ftime, sizeof(FILETIME));

        if (GetProcessTimes(self, &ftime, &ftime, &fsys, &fuser))
        {
            memcpy(&sys, &fsys, sizeof(FILETIME));
            memcpy(&user, &fuser, sizeof(FILETIME));
            percent = static_cast<double>((sys.QuadPart - lastSysCPU.QuadPart) +
                                          (user.QuadPart - lastUserCPU.QuadPart));

            auto diff = now.QuadPart - lastCPU.QuadPart;

            if (diff != 0)
                percent /= static_cast<double>(diff);

            percent /= static_cast<double>(numProcessors);
            lastCPU         = now;
            lastUserCPU     = user;
            lastSysCPU      = sys;
            info.ProcessUse = percent * 100.0;
        }

#else
        LINA_ERR("[Profiler] -> CPU query for other platforms not implemented!");
#endif

        return info;
    }

    void Profiler::StartFrame()
    {
        g_skipAllocTrack        = true;
        const double cpuTimeNow = Time::GetCPUTime();

        if (m_frames.size() == MAX_FRAME_BACKTRACE)
        {
            CleanupFrame(m_frames.front());
            m_totalFrameTimeNS -= m_frames.front().DurationNS;
            m_frames.pop();
        }

        if (!m_frames.empty())
        {
            m_frames.back().DurationNS = (cpuTimeNow - m_frames.back().StartTime) * 1.0e9;
            m_totalFrameTimeNS += m_frames.back().DurationNS;
        }

        Frame f;
        f.StartTime = cpuTimeNow;
        m_frames.emplace(f);

        g_skipAllocTrack = false;
    }

    void Profiler::StartScope(const String& scope, const String& thread)
    {
        m_lock.lock();
        g_skipAllocTrack        = true;
        const double cpuTimeNow = Time::GetCPUTime();

        Scope* s      = new Scope();
        s->Name       = scope;
        s->ThreadName = thread;
        s->StartTime  = cpuTimeNow;
        s->Parent     = m_lastScope;
        s->ThreadID   = StringID(thread.c_str()).value();

        if (m_lastScope == nullptr || m_lastScope->ThreadID != s->ThreadID)
            m_frames.back().Scopes.push_back(s);
        else
            m_lastScope->Children.push_back(s);

        m_lastScope      = s;
        g_skipAllocTrack = false;
        m_lock.unlock();
    }

    void Profiler::EndScope()
    {
        m_lock.lock();
        m_lastScope->DurationNS = (Time::GetCPUTime() - m_lastScope->StartTime) * 1.0e9;
        m_lastScope             = m_lastScope->Parent;
        m_lock.unlock();
    }

    void Profiler::OnAllocation(void* ptr, size_t size)
    {
        m_lock.lock();
        g_skipAllocTrack = true;
        MemAllocationInfo info;
        info.Size = size;
        CaptureTrace(info);
        m_memAllocations[ptr] = info;
        m_totalMemAllocationSize += size;
        g_skipAllocTrack = false;
        m_lock.unlock();
    }
    void Profiler::OnVRAMAllocation(void* ptr, size_t size)
    {
        g_skipAllocTrack = true;
        MemAllocationInfo info;
        info.Size = size;
        CaptureTrace(info);
        m_memAllocations[ptr] = info;
        m_totalVRAMAllocationSize += size;
        g_skipAllocTrack = false;
    }

    void Profiler::OnFree(void* ptr)
    {
        m_lock.lock();
        g_skipAllocTrack = true;
        m_totalMemAllocationSize -= m_memAllocations[ptr].Size;
        m_memAllocations.erase(ptr);
        g_skipAllocTrack = false;
        m_lock.unlock();
    }

    void Profiler::OnVRAMFree(void* ptr)
    {
        m_lock.lock();
        g_skipAllocTrack = true;
        m_totalVRAMAllocationSize -= m_memAllocations[ptr].Size;
        m_vramAllocations.erase(ptr);
        g_skipAllocTrack = false;
        m_lock.unlock();
    }

    Function::Function(const String& funcName, const String& threadName)
    {
        Profiler::Get()->StartScope(funcName, threadName);
        
    }

    Function::~Function()
    {
        Profiler::Get()->EndScope();
    }

    void Profiler::DumpMemoryLeaks(const String& path)
    {
        if (Utility::FileExists(path))
            Utility::DeleteFileInPath(path);

        std::ofstream file(path.c_str());

        auto writeTrace = [&](const ParallelHashMap<void*, MemAllocationInfo>& map) {
            for (const auto& alloc : map)
            {
                if (alloc.first == 0)
                    continue;

                file << "****************** LEAK DETECTED ******************\n";
                file << "Size: " << alloc.second.Size << " bytes \n";

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

                for (int i = 0; i < alloc.second.StackSize; ++i)
                {
                    file << "------ Stack Trace " << i << "------\n";

                    DWORD64 address = (DWORD64)(alloc.second.Stack[i]);

                    SymFromAddr(process, address, NULL, symbol);

                    if (SymGetLineFromAddr64(process, address, &displacement, line))
                    {
                        file << "Location:" << line->FileName << "\n";
                        file << "Smybol:" << symbol->Name << "\n";
                        file << "Line:" << line->LineNumber << "\n";
                        file << "SymbolAddr:" << symbol->Address << "\n";
                    }
                    else
                    {
                        file << "Smybol:" << symbol->Name << "\n";
                        file << "SymbolAddr:" << symbol->Address << "\n";
                    }

                    IMAGEHLP_MODULE64 moduleInfo;
                    moduleInfo.SizeOfStruct = sizeof(moduleInfo);
                    if (::SymGetModuleInfo64(process, symbol->ModBase, &moduleInfo))
                        file << "Module:" << moduleInfo.ModuleName << "\n";
                }
                std::free(line);
                std::free(symbolAll);
#endif

                file << "\n";
                file << "\n";
            }
        };

        if (file.is_open())
        {
            size_t totalSizeBytes = 0;
            size_t totalSizeKB    = 0;

            for (auto& alloc : m_memAllocations)
                totalSizeBytes += alloc.second.Size;

            totalSizeKB = static_cast<size_t>(static_cast<float>(totalSizeBytes) / 1000.0f);

            file << "-------------------------------------------\n";
            file << "PROCESS MEMORY LEAKS\n";
            file << "Total leaked size: " << totalSizeBytes << " (bytes) " << totalSizeKB << " (kb)\n";
            file << "-------------------------------------------\n";
            file << "\n";

            writeTrace(m_memAllocations);

            totalSizeBytes = totalSizeKB = 0;

            for (auto& alloc : m_vramAllocations)
                totalSizeBytes += alloc.second.Size;

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
        if (Utility::FileExists(path))
            Utility::DeleteFileInPath(path);

        std::ofstream file(path.c_str());

        if (file.is_open())
        {

            const double avgTimeNS = m_totalFrameTimeNS / static_cast<double>(MAX_FRAME_BACKTRACE);
            const double avgTimeMS = avgTimeNS * 0.000001;

            file << "-------------------------------------------\n";
            file << "FRAME ANALYSIS - SHOWING LAST " << MAX_FRAME_BACKTRACE << " FRAMES \n";
            file << "-------------------------------------------\n";
            file << "Average Frame Time: " << avgTimeNS << " (NS) " << avgTimeMS << " (MS) \n";
            file << "\n";

            int frameCounter = 0;
            while (!m_frames.empty())
            {
                auto f = m_frames.front();
                
                file << "------------------------------------ FRAME " << frameCounter << "------------------------------------\n";
                file << "Duration: " << f.DurationNS << " (NS) " << f.DurationNS * 0.000001 << " (MS)\n";

                String indent = "";
                for(auto s : f.Scopes)
                    WriteScopeData(indent, s, file);

                file << "\n";
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
        file << "\n";
        file << indent.c_str() << "-------- " << scope->Name.c_str() << " --------\n";
        file << indent.c_str() << "Duration: " << scope->DurationNS * 0.000001 << " (MS)\n";
        file << indent.c_str() << "Thread: " << scope->ThreadName.c_str()  << "\n";
        newIndent += "    ";
        for(auto s : scope->Children)
            WriteScopeData(newIndent, s, file);

    }

    void Profiler::CaptureTrace(MemAllocationInfo& info)
    {

#ifdef LINA_PLATFORM_WINDOWS
        info.StackSize = CaptureStackBackTrace(3, MEMORY_STACK_TRACE_SIZE, info.Stack, nullptr);
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

    void Profiler::Initialize()
    {
        LINA_TRACE("[Initialization] -> Profiler {0}", typeid(*this).name());
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

        LINA_TRACE("[Shutdown] -> Profiler {0}", typeid(*this).name());

#ifdef LINA_PLATFORM_WINDOWS
        HANDLE process = GetCurrentProcess();
        SymCleanup(process);
#endif
    }

    void Profiler::CleanupFrame(Frame& frame)
    {
        for (auto scope : frame.Scopes)
            CleanupScope(scope);

        frame.Scopes.clear();
    }

    void Profiler::CleanupScope(Scope* scope)
    {
        for (auto s : scope->Children)
            CleanupScope(s);

        delete scope;
    }


} // namespace Lina

#endif