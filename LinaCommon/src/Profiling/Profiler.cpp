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
        g_skipAllocTrack = true;
        m_totalVRAMAllocationSize -= m_memAllocations[ptr].Size;
        m_vramAllocations.erase(ptr);
        g_skipAllocTrack = false;
    }

    Function::Function(const String& funcName, const String& threadName)
    {
        Profiler::Get()->StartScope(funcName, threadName);
    }

    Function::~Function()
    {
        Profiler::Get()->EndScope();
    }

    void Profiler::DumpToText(const String& path)
    {
    }

    void Profiler::CaptureTrace(MemAllocationInfo& info)
    {

#ifdef LINA_PLATFORM_WINDOWS
        HANDLE process = GetCurrentProcess();
        SymInitialize(process, nullptr, TRUE);

        void* stack[10];
        int   frames = (int)CaptureStackBackTrace(3, 1, stack, nullptr);

        void* symbolAll = calloc(sizeof(SYMBOL_INFO) + MAX_SYM_NAME * sizeof(TCHAR), 1);

        if (symbolAll == nullptr)
            return;

        SYMBOL_INFO* symbol = static_cast<SYMBOL_INFO*>(symbolAll);
        symbol->MaxNameLen = 255;
        symbol->SizeOfStruct = sizeof(SYMBOL_INFO);

        DWORD           displacement;
        IMAGEHLP_LINE64 line;

        for (int i = 0; i < frames; ++i)
        {
            DWORD64 address = (DWORD64)(stack[i]);
            MemStackTrace trace;

            SymFromAddr(process, address, NULL, symbol);

            if (SymGetLineFromAddr64(process, address, &displacement, &line))
            {
                trace.Line       = static_cast<int>(line.LineNumber);
                trace.Location   = static_cast<String>(line.FileName);
                trace.SymbolName = static_cast<String>(symbol->Name);
                trace.SmybolAddr = static_cast<uint64>(symbol->Address);
            }
            else
            {
                trace.SymbolName = static_cast<String>(symbol->Name);
                trace.SmybolAddr = static_cast<uint64>(symbol->Address);
            }

            IMAGEHLP_MODULE64 moduleInfo;
            moduleInfo.SizeOfStruct = sizeof(moduleInfo);
            if (::SymGetModuleInfo64(process, symbol->ModBase, &moduleInfo))
            {
               trace.ModuleName = static_cast<String>(moduleInfo.ModuleName);
            }

            info.StackTrace.push_back(trace);
        }

#else

        void*  stack[5];
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

} // namespace Lina

#endif