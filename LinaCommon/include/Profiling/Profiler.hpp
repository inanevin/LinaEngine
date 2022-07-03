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

#define MEMORY_STACK_TRACE_SIZE 20
#define MAX_FRAME_BACKTRACE     500

// Headers here.
#include "Data/String.hpp"
#include "Data/HashMap.hpp"
#include "Data/Vector.hpp"
#include "Data/Queue.hpp"
#include "Utility/StringId.hpp"
#include <source_location>

namespace Lina
{
    struct Scope
    {
    public:
        String         ThreadName = "";
        StringIDType   ThreadID   = 0;
        String         Name       = "";
        double         DurationNS = 0.0;
        double         StartTime  = 0.0;
        Scope*         Parent     = nullptr;
        Vector<Scope*> Children;
    };

    struct Function
    {
        Function(const String& funcName, const String& threadName = "Main");
        ~Function();
    };

    struct Frame
    {
        double         DurationNS = 0.0;
        double         StartTime  = 0.0;
        Vector<Scope*> Scopes;
    };

    struct MemAllocationInfo
    {
        size_t         Size      = 0;
        unsigned short StackSize = 0;
        void*          Stack[MEMORY_STACK_TRACE_SIZE];
    };

    struct DeviceMemoryInfo
    {
        unsigned long TotalVirtualMemory        = 0;
        unsigned long TotalUsedVirtualMemory    = 0;
        unsigned long TotalProcessVirtualMemory = 0;
        unsigned long TotalRAM                  = 0;
        unsigned long TotalUsedRAM              = 0;
        unsigned long TotalProcessRAM           = 0;
    };

    struct DeviceCPUInfo
    {
        double ProcessUse = 0;
    };

    class Profiler
    {
    public:
        static Profiler* Get()
        {
            return s_instance;
        }

        DeviceMemoryInfo QueryMemoryInfo();
        DeviceCPUInfo    QueryCPUInfo();
        void             StartFrame();
        void             StartScope(const String& scope, const String& thread = "Main");
        void             EndScope();
        void             OnAllocation(void* ptr, size_t size);
        void             OnVRAMAllocation(void* ptr, size_t size);
        void             OnFree(void* ptr);
        void             OnVRAMFree(void* ptr);
        void             DumpMemoryLeaks(const String& path);
        void             DumpFrameAnalysis(const String& path);
        void             WriteScopeData(String& indent, Scope* scope, std::ofstream& file);
        static Profiler* s_instance;

    private:
        void CaptureTrace(MemAllocationInfo& info);

    private:
        friend class Application;
        friend class Engine;

        void Initialize();
        void Shutdown();
        void CleanupFrame(Frame& frame);
        void CleanupScope(Scope* s);

        double                                    m_totalFrameTimeNS = 0.0;
        Scope*                                    m_lastScope        = nullptr;
        Queue<Frame>                              m_frames;
        ParallelHashMap<void*, MemAllocationInfo> m_memAllocations;
        ParallelHashMap<void*, MemAllocationInfo> m_vramAllocations;
        size_t                                    m_totalMemAllocationSize  = 0;
        size_t                                    m_totalVRAMAllocationSize = 0;
        std::mutex                                m_lock;
    };

#define PROFILER_FRAME_START()               Profiler::Get()->StartFrame()
#define PROFILER_SCOPE_START(SCOPENAME, ...) Profiler::Get()->StartScope(SCOPENAME, __VA_ARGS__)
#define PROFILER_SCOPE_END()                 Profiler::Get()->EndScope()
#define PROFILER_FUNC(...)                   Function func(__FUNCTION__, __VA_ARGS__)
#define PROFILER_DUMP_LEAKS(PATH)            Profiler::Get()->DumpMemoryLeaks(PATH)
#define PROFILER_ALLOC(PTR, SZ)              Profiler::Get()->OnAllocation(PTR, SZ)
#define PROFILER_VRAMALLOC(PTR, SZ)          Profiler::Get()->OnVRAMAllocation(PTR, SZ)
#define PROFILER_FREE(PTR)                   Profiler::Get()->OnFree(PTR)
#define PROFILER_VRAMFREE(PTR)               Profiler::Get()->OnVRAMFree(PTR)
#define PROFILER_DUMP_FRAME_ANALYSIS(PATH)   Profiler::Get()->DumpFrameAnalysis(PATH)

} // namespace Lina

#else

#define PROFILER_FRAME_START()
#define PROFILER_SCOPE_START(SCOPENAME, ...)
#define PROFILER_SCOPE_END()
#define PROFILER_FUNC(...)
#define PROFILER_DUMP_LEAKS(PATH)
#define PROFILER_ALLOC(PTR, SZ)
#define PROFILER_VRAMALLOC(PTR, SZ)
#define PROFILER_FREE(PTR)
#define PROFILER_VRAMFREE(PTR)
#define PROFILER_SKIPTRACK(skip)
#define PROFILER_DUMP_FRAME_ANALYSIS(PATH)
#endif

#endif