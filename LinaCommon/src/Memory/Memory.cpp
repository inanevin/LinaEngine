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

#include "Memory/Memory.hpp"
#include "Math/Math.hpp"
#include "Profiling/Profiler.hpp"

#include <cstdlib>
#include <stdio.h>
#include <iostream>

#ifdef LINA_ENABLE_PROFILING
bool g_skipAllocTrack = false;
#endif

// EASTL OPERATOR NEW[] REQUIREMENTS

void* __cdecl operator new[](size_t size, size_t, size_t, const char* name, int flags, unsigned int debugFlags, const char* file, int line)
{
    void* ptr = std::malloc(size);
#ifdef LINA_ENABLE_PROFILING
    if (!g_skipAllocTrack && Lina::Profiler::Get() != nullptr)
        Lina::PROFILER_ALLOC(ptr, size);
#endif
    return ptr;
}

void* __cdecl operator new[](size_t size, const char* name, int flags, unsigned int debugFlags, const char* file, int line)
{
    void* ptr = std::malloc(size);
#ifdef LINA_ENABLE_PROFILING
    if (!g_skipAllocTrack && Lina::Profiler::Get() != nullptr)
        Lina::PROFILER_ALLOC(ptr, size);
#endif
    return ptr;
}

void* operator new(std::size_t size)
{
    void* ptr = std::malloc(size);
#ifdef LINA_ENABLE_PROFILING
    if (!g_skipAllocTrack && Lina::Profiler::Get() != nullptr)
        Lina::PROFILER_ALLOC(ptr, size);
#endif
    return ptr;
}

void* operator new[](size_t size)
{
    void* ptr = std::malloc(size);

#ifdef LINA_ENABLE_PROFILING
    if (!g_skipAllocTrack && Lina::Profiler::Get() != nullptr)
        Lina::PROFILER_ALLOC(ptr, size);
#endif
    return ptr;
}

void operator delete(void* ptr)
{
#ifdef LINA_ENABLE_PROFILING
    if (!g_skipAllocTrack && Lina::Profiler::Get() != nullptr)
        Lina::PROFILER_FREE(ptr);
#endif
    free(ptr);
}
void operator delete(void* ptr, size_t sz)
{
#ifdef LINA_ENABLE_PROFILING
    if (!g_skipAllocTrack && Lina::Profiler::Get() != nullptr)
        Lina::PROFILER_FREE(ptr);
#endif
    free(ptr);
}
void operator delete[](void* ptr)
{
#ifdef LINA_ENABLE_PROFILING
    if (!g_skipAllocTrack && Lina::Profiler::Get() != nullptr)
        Lina::PROFILER_FREE(ptr);
#endif
    free(ptr);
}

void operator delete  (void* ptr, const std::nothrow_t& tag)
{
#ifdef LINA_ENABLE_PROFILING
    if (!g_skipAllocTrack && Lina::Profiler::Get() != nullptr)
        Lina::PROFILER_FREE(ptr);
#endif
    free(ptr);
}
void operator delete[](void* ptr, const std::nothrow_t& tag)
{
#ifdef LINA_ENABLE_PROFILING
    if (!g_skipAllocTrack && Lina::Profiler::Get() != nullptr)
        Lina::PROFILER_FREE(ptr);
#endif
    free(ptr);
}
void operator delete[](void* ptr, std::size_t sz)
{
#ifdef LINA_ENABLE_PROFILING
    if (!g_skipAllocTrack && Lina::Profiler::Get() != nullptr)
        Lina::PROFILER_FREE(ptr);
#endif
    free(ptr);
}

namespace Lina
{
  
} // namespace Lina
