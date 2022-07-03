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
#include <DbgHelp.h>
#endif

namespace Lina
{
    Profiler* Profiler::s_instance = nullptr;

    void Profiler::StartFrame()
    {
        const double cpuTimeNow = Time::GetCPUTime();

        if (m_lastFrame != nullptr)
            m_lastFrame->DurationNS = (cpuTimeNow - m_lastFrame->StartTime) * 1.0e9;

        Frame f;
        m_frames.push_back(f);
        m_lastFrame            = m_frames.end().mpNode;
        m_lastFrame->StartTime = cpuTimeNow;
    }

    void Profiler::StartScope(const String& scope, const String& thread)
    {
        const double cpuTimeNow = Time::GetCPUTime();

        if (m_lastScope == nullptr)
        {
            Scope s;
            m_lastFrame->Scopes.push_back(s);
            m_lastScope = m_lastFrame->Scopes.end().mpNode;
        }
        else
        {
            Scope s;
            m_lastScope->Children.push_back(s);
            Scope* scp  = m_lastScope->Children.end().mpNode;
            scp->Parent = m_lastScope;
            m_lastScope = scp;
        }

        m_lastScope->Name       = scope;
        m_lastScope->ThreadName = thread;
        m_lastScope->StartTime  = cpuTimeNow;
    }

    void Profiler::EndScope()
    {
        m_lastScope->DurationNS = (Time::GetCPUTime() - m_lastScope->StartTime) * 1.0e9;
        if (m_lastScope->Parent != nullptr)
            m_lastScope = m_lastScope->Parent;
        else
            m_lastScope = nullptr;
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
                if(alloc.first == 0)
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

                SYMBOL_INFO* symbol  = static_cast<SYMBOL_INFO*>(symbolAll);
                symbol->MaxNameLen   = 255;
                symbol->SizeOfStruct = sizeof(SYMBOL_INFO);

                DWORD            displacement;
                IMAGEHLP_LINE64* line;
                line               = (IMAGEHLP_LINE64*)std::malloc(sizeof(IMAGEHLP_LINE64));
                line->SizeOfStruct = sizeof(IMAGEHLP_LINE64);

                for (int i = 0; i < alloc.second.StackSize; ++i)
                {
                    file << "------ Stack Trace " << i << "------\n";

                    DWORD64       address = (DWORD64)(alloc.second.Stack[i]);

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
            file << "Total leaked size: " << totalSizeBytes << " (bytes) " << totalSizeKB << " (mkb)\n";
            file << "-------------------------------------------\n";
            file << "\n";

            writeTrace(m_vramAllocations);

            file.close();
        }
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
        LINA_TRACE("[Shutdown] -> Profiler {0}", typeid(*this).name());

#ifdef LINA_PLATFORM_WINDOWS
        HANDLE process = GetCurrentProcess();
        SymCleanup(process);
#endif
    }

} // namespace Lina

#endif