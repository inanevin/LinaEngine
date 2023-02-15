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
#include "Profiling/MemoryTracer.hpp"
#include "FileSystem/FileSystem.hpp"
#include "Profiling/Profiler.hpp"
#include "Memory/MemoryAllocatorPool.hpp"

#ifdef LINA_PLATFORM_WINDOWS
#include <process.h>
#include <iostream>
#include "Platform/Win32/Win32WindowsInclude.hpp"
#include <psapi.h>
#include <DbgHelp.h>
#include <Pdh.h>
#include <PdhMsg.h>
#pragma comment(lib, "pdh.lib")
#endif

namespace Lina
{
    void MemoryTracer::RegisterAllocator(MemoryAllocatorPool* alloc)
    {
        m_registeredAllocators.push_back(alloc);
    }

    void MemoryTracer::UnregisterAllocator(MemoryAllocatorPool* alloc)
    {
        for (int i = 0; i < m_registeredAllocators.size(); i++)
        {
            if (m_registeredAllocators[i] == alloc)
            {
                m_registeredAllocators.remove(i);
                break;
            }
        }
    }

    void MemoryTracer::OnAllocation(void* ptr, size_t sz)
    {
        LOCK_GUARD(m_mtx);
        MemoryTrack track = MemoryTrack{
            .ptr  = ptr,
            .size = sz,
        };

        CaptureTrace(track);

        m_defTracks.push_back(track);
    }

    void MemoryTracer::OnFree(void* ptr)
    {
        LOCK_GUARD(m_mtx);
        for (int i = 0; i < m_defTracks.size(); i++)
        {
            if (m_defTracks[i].ptr == ptr)
            {
                m_defTracks.remove(i);
                break;
            }
        }
    }

    void MemoryTracer::OnVramAllocation(void* ptr, size_t sz)
    {
        LOCK_GUARD(m_mtx);
        MemoryTrack track = MemoryTrack{
            .ptr  = ptr,
            .size = sz,
        };

        CaptureTrace(track);

        m_vramTracks.push_back(track);
    }

    void MemoryTracer::OnVramFree(void* ptr)
    {
        LOCK_GUARD(m_mtx);

        for (int i = 0; i < m_vramTracks.size(); i++)
        {
            if (m_vramTracks[i].ptr == ptr)
            {
                m_vramTracks.remove(i);
                break;
            }
        }
    }

    DeviceMemoryInfo MemoryTracer::QueryMemoryInfo()
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

    void MemoryTracer::CaptureTrace(MemoryTrack& track)
    {
#ifdef LINA_PLATFORM_WINDOWS
        track.stackSize = CaptureStackBackTrace(3, MEMORY_STACK_TRACE_SIZE, track.stack, nullptr);
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

    void MemoryTracer::Destroy()
    {
        if (m_destroyed)
            return;

        m_destroyed = true;

        if (MemoryLeaksFile != NULL && MemoryLeaksFile[0] != '\0')
            DumpLeaks(MemoryLeaksFile);

        m_defTracks.clear();

#ifdef LINA_PLATFORM_WINDOWS
        HANDLE process = GetCurrentProcess();
        SymCleanup(process);
#endif
    }

    void MemoryTracer::DumpLeaks(const char* path)
    {
        if (FileSystem::FileExists(path))
            FileSystem::DeleteFileInPath(path);

        std::ofstream file(path);

        auto writeTrace = [&](const SimpleArray<MemoryTrack>& tracks) {
            for (int i = 0; i < tracks.size(); i++)
            {
                const auto& alloc = tracks[i];

                std::ostringstream ss;

                ss << "****************** LEAK DETECTED ******************\n";
                ss << "Size: " << alloc.size << " bytes \n";

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

                for (int i = 0; i < alloc.stackSize; ++i)
                {
                    ss << "------ Stack Trace " << i << "------\n";

                    DWORD64 address = (DWORD64)(alloc.stack[i]);

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

                file << ss.str();
                ss.clear();
            }
        };

        if (file.is_open())
        {
            size_t totalSizeBytes = 0;
            size_t totalSizeKB    = 0;

            for (int i = 0; i < m_defTracks.size(); i++)
                totalSizeBytes += m_defTracks[i].size;

            totalSizeKB = static_cast<size_t>(static_cast<float>(totalSizeBytes) / 1000.0f);

            file << "-------------------------------------------\n";
            file << "PROCESS MEMORY LEAKS\n";
            file << "Total leaked size: " << totalSizeBytes << " (bytes) " << totalSizeKB << " (kb)\n";
            file << "-------------------------------------------\n";
            file << "\n";

            writeTrace(m_defTracks);

            totalSizeBytes = totalSizeKB = 0;

            for (int i = 0; i < m_vramTracks.size(); i++)
                totalSizeBytes += m_vramTracks[i].size;

            totalSizeKB = static_cast<size_t>(static_cast<float>(totalSizeBytes) / 1000.0f);

            file << "-------------------------------------------\n";
            file << "VRAM MEMORY LEAKS\n";
            file << "Total leaked size: " << totalSizeBytes << " (bytes) " << totalSizeKB << " (mb)\n";
            file << "-------------------------------------------\n";
            file << "\n";

            writeTrace(m_vramTracks);

            file.close();
        }
    }
} // namespace Lina
