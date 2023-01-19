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

#ifndef Memory_HPP
#define Memory_HPP

#include "Memory/MemoryManager.hpp"

#ifdef LINA_ENABLE_PROFILING
extern bool g_skipAllocTrack;
#endif

namespace Lina
{

// 64-bit ptr + 1 byte
#define MIN_ALLOC_LIMIT 9

    template <typename T> inline T* GNew()
    {
        const size_t size   = sizeof(T);
        void*        memory = size >= MIN_ALLOC_LIMIT ? MemoryManager::Get()->AllocateGlobal(size) : malloc(size);
        T*           ptr    = new (memory) T();
        return ptr;
    }

    template <typename T> inline void GDelete(T* obj)
    {
        const size_t size   = sizeof(T);
        void*        objPtr = static_cast<void*>(obj);

        if (size >= MIN_ALLOC_LIMIT)
            MemoryManager::Get()->FreeGlobal(objPtr, size);
        else
            free(objPtr);

        obj->~T();
    }

#define MEMCPY(...) memcpy(__VA_ARGS__)
#define MALLOC(...) malloc(__VA_ARGS__)
#define FREE(...)   free(__VA_ARGS__)

} // namespace Lina

#endif
